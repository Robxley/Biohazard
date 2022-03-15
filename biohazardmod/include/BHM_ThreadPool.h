#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <type_traits>

namespace bhd
{
	namespace details
	{
		//Result trick when result is 'void' type
		template<class T>
		struct threaded_task_result {
		public:
			T m_result;
			using TRef = T&;
			using TCRef = const T&;
		};

		template <>
		struct threaded_task_result<void> {
			using TRef = void;
			using TCRef = void;
		};
	}

	template<class T>
	class threaded_task : public details::threaded_task_result<T>
	{
		friend class thread_pool;
		using TRef = details::threaded_task_result<T>::TRef;	//reference on the result T or void

		std::function<void()> m_fct;
		std::future<T> m_future;

		//private constructor
		threaded_task(const threaded_task&) = delete;
		threaded_task& operator=(const threaded_task&) = delete;

	public:

		threaded_task() {};

		template<class F, class... Args>
		threaded_task(F&& f, Args&&... args) {
			set(std::forward<F>(f), std::forward<Args>(args)...);
		}

		threaded_task(threaded_task&& task)
		{
			m_fct = std::move(task.m_fct);
			m_future = std::move(task.m_future);
		}

		void operator=(threaded_task&& task)
		{
			m_fct = std::move(task.m_fct);
			m_future = std::move(task.m_future);
		}

		template<class F, class... Args>
		void set(F&& f, Args&&... args)
		{
			using result_t = std::invoke_result_t<F, Args...>;
			using packaged_tr = std::packaged_task<result_t()>;

			auto task = packaged_tr(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

			m_future = task.get_future();

			using atomic_task = std::pair<std::atomic_bool, packaged_tr >;
			auto a_task = std::make_shared<atomic_task>(false, std::move(task));

			m_fct = [a_task]() mutable
			{
				if (a_task->first.exchange(true) == false)
					a_task->second();
			};
		}

		auto get()
		{
			m_fct(); //Start the function in the current thread if the function is not yet called
			if constexpr (std::is_void_v<T>)
				m_future.get();
			else
				return m_future.get();
		}
	};

	class thread_pool
	{
		// need to keep track of threads so we can join them
		std::vector< std::thread > m_workers;

		// the task queue
		std::queue< std::function<void()> > m_tasks;

		// synchronization
		std::mutex m_queue_mutex;
		std::condition_variable m_condition;
		bool m_stop = false;

		//Number of workers (threads)
		size_t m_pool_size = 0;

	public:

		~thread_pool();
		thread_pool(size_t);
		thread_pool() : thread_pool(std::thread::hardware_concurrency()) {};

		static thread_pool& instance(size_t size = std::thread::hardware_concurrency())
		{
			static thread_pool singleton(size);
			return singleton;
		}


		template<class T>
		void enqueue(const threaded_task<T>& task)
		{
			// don't allow enqueue after stopping the pool
			if (m_stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");
			{
				std::unique_lock<std::mutex> lock(m_queue_mutex);
				m_tasks.emplace(task.m_fct);
			}
			m_condition.notify_one();
		}


		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args) -> threaded_task<std::invoke_result_t<F, Args...>>
		{
			using result_t = std::invoke_result_t<F, Args...>;
			threaded_task<result_t> new_task(std::forward<F>(f), std::forward<Args>(args)...);
			this->enqueue<result_t>(new_task);
			return new_task;
		}

	};

}