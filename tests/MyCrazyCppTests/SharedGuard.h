#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <cassert>
#include <optional>

//Protect a shared_ptr for multithreaded access.

#define VERBOSE_FLAG

template <typename T>
struct shared_guard_t : public std::shared_ptr<T>
{
	using shared_ptr_t = std::shared_ptr<T>;

	shared_guard_t() = default;

	//Delete all copy / movable constructor
	shared_guard_t(shared_guard_t&&) = delete;
	shared_guard_t& operator=(shared_guard_t&&) = delete;
	shared_guard_t(const shared_guard_t&) = delete;
	shared_guard_t& operator=(const shared_guard_t&) = delete;

	// Parameter forwarding to the constructor shared_ptr
	template<typename ... Args>
	shared_guard_t(Args&& ...args) :
		shared_ptr_t(std::forward<Args>(args)...) {	}

	shared_guard_t(shared_ptr_t&& raw) :
		shared_ptr_t(std::move(raw)) {	}

	shared_guard_t(const shared_ptr_t& raw) :
		shared_ptr_t(raw) {	}

	shared_guard_t& operator=(const shared_ptr_t& raw) {
		shared_ptr_t::operator=(std::shared_ptr<T>(raw));
		return *this;
	}

	shared_guard_t& operator=(T* raw) {
		shared_ptr_t::operator=(std::shared_ptr<T>(raw));
		return *this;
	}

	shared_guard_t(T* raw) :
		shared_ptr_t(raw) 
	{  }

	//Access overload
	//---------------------

	auto operator->() {
		return lock_guard();
	}

	auto operator->() const {
		return lock_guard();
	}

	auto lock_guard() const {
		return locker_t{ m_protector, shared_ptr_t::get() };
	}

	bool has_value() const {
		return lock_guard();
	}

	auto value() {
		return lock_guard();
	}

	auto operator*() {
		return value();
	}

private:
	struct locker_t
	{
		std::unique_lock<std::mutex> m_locker;
		T* m_ptr;

		locker_t() = delete;

		locker_t(locker_t&&) = delete;
		locker_t& operator=(locker_t&&) = delete;

		locker_t(const locker_t&) = delete;
		locker_t& operator=(const locker_t&) = delete;

		template <typename Locker, typename Ptr>
		locker_t(Locker&& lc, Ptr&& ptr) :
			m_locker(std::forward<Locker>(lc), std::defer_lock),
			m_ptr(std::forward<Ptr>(ptr))
		{
			assert(ptr != nullptr && "invalid pointer to protect");
			if (!m_locally_lock.has_value())  // Already Lock in this thread  ?  (m_locally_lock is thread_local -> once instance by thread)
			{
				//If not, lock it !
				m_locally_lock = this;  //lock by this
				m_locker.lock();
#ifdef VERBOSE_FLAG
				std::cout << "Lock [" << std::this_thread::get_id() <<']' << std::endl;
				return;
#endif
			}
#ifdef VERBOSE_FLAG
			std::cout << "Already Locked by [" << std::this_thread::get_id() <<']' << std::endl;
			return;
#endif

		}

		~locker_t() {
			if (m_locally_lock.has_value() && m_locally_lock.value() == this) {  //Unlock if it was the locker thread
				m_locally_lock = {};
#ifdef VERBOSE_FLAG
				std::cout << "UnLock [" << std::this_thread::get_id() << "]\n" << std::endl;
#endif
			}
		}

		T* operator->() {
			return m_ptr;
		}

		const T* operator->() const {
			return m_ptr;
		}

		T* ptr() {
			return m_ptr;
		}
		
		const T* ptr() const {
			return m_ptr;
		}

		template <class Value>
		const locker_t& operator=(Value&& value) const {
			*m_ptr = std::forward<Value>(value);
			return *this;
		}

		template <class Value>
		locker_t& operator=(Value&& value) {
			*m_ptr = std::forward<Value>(value);
			return *this;
		}

		bool has_value() const {
			return m_ptr != nullptr;
		}

		operator T&() {
			return *m_ptr;
		}

		operator const T& () const {
			return *m_ptr;
		}

		T& operator*() {
			return *m_ptr;
		}

		T& operator*() const {
			return *m_ptr;
		}

		operator bool() const {
			return has_value();
		}
	};

protected:
	mutable std::mutex m_protector;
	inline thread_local static std::optional<locker_t*> m_locally_lock;
};

#if _DEBUG
namespace unit_test
{

	void multithread_test()
	{
		struct Foo
		{
			void f1() {
				std::cout << "f1 wait: " << std::this_thread::get_id();
				std::this_thread::sleep_for(std::chrono::seconds(2));
				std::cout << " - end" << std::endl;
			}

			void f2() const {
				std::cout << "f2 wait: " << std::this_thread::get_id();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << " - end" << std::endl;
			}
		};
		shared_guard_t myprotectedData = std::make_shared<Foo>();

		std::thread th1 = std::thread([&] {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			myprotectedData->f1();
		});

		std::thread th2 = std::thread([&] {
			myprotectedData->f2();
		});

		std::thread th3 = std::thread([&] {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			auto locker = myprotectedData.lock_guard();
			locker->f2();
			locker->f1();
		});

		th1.join();

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		myprotectedData->f1();

		th2.join();

		myprotectedData->f2();

		th3.join();
	}

	void access_test()
	{
		struct FooStr
		{
			std::string m_data;
			FooStr& operator=(const std::string & str) {
				m_data = str;
				std::cout << "New data: " << m_data << std::endl;
				return *this;
			}

			FooStr& operator=(const FooStr& foo) {
				m_data = foo.m_data;
				std::cout << "Copy/Read data: " << m_data << std::endl;
				return *this;
			}
		};
		const char* bidon = "123456";

		shared_guard_t mydata = std::make_shared<FooStr>();
		*mydata = bidon;

		FooStr read;
		read = *mydata;
		assert(read.m_data == bidon);
		assert(mydata.has_value() == true);

	}

	void samethreadlock_test()
	{
		shared_guard_t my_data = std::make_shared<int>();

		auto same_thread_multi_lock = [&] {
			try
			{
				auto guarded_data = my_data.lock_guard();
				guarded_data = 10;
				{
					assert(guarded_data == 10);
					auto already_guarded = my_data.lock_guard();
				}
				return true;
			}
			catch (...) {
				return false;
			}
		};
		assert(same_thread_multi_lock() == true);
	}
}
#endif