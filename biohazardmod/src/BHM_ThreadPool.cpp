#include "BHM_ThreadPool.h"

namespace bhd
{

	// the constructor just launches some amount of workers
	thread_pool::thread_pool(size_t threads)
		: m_stop(false), m_pool_size(threads)
	{
		m_workers.reserve(threads);

		for (size_t i = 0; i < threads; ++i)
		{
			m_workers.emplace_back(
				[this, i]
				{
					for (;;)
					{
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(m_queue_mutex);
							m_condition.wait(lock,
								[this] { return m_stop || !m_tasks.empty(); });

							if (m_stop && m_tasks.empty())
								return;

							task = std::move(m_tasks.front());
							m_tasks.pop();
						}
						task();
					}
				}
				);
		}


	}

	thread_pool::~thread_pool()
	{
		{
			const std::lock_guard<std::mutex> lock(m_queue_mutex);
			m_stop = true;
		}
		m_condition.notify_all();
		for (std::thread& worker : m_workers)
			worker.join();
	}

}