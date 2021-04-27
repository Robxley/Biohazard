
#include <iostream>
#include <type_traits>
#include <string>
#include <filesystem>
#include <cassert>
#include <mutex>
#include <memory>
#include <vector>

template <typename T>
struct CProcessControler
{
	struct CLocker
	{
		std::lock_guard<std::mutex> m_locker;
		T* m_ptr;

		template <typename Locker, typename Ptr>
		CLocker(Locker&& lc, Ptr&& ptr) :
			m_locker(std::forward<Locker>(lc)),
			m_ptr(std::forward<Ptr>(ptr))
		{
			assert(ptr != nullptr && "invalid pointer to protect");
			std::cout << "CLocker: " << std::this_thread::get_id() << std::endl;
		}

		T* operator->() {
			return m_ptr;
		}
		const T* operator->() const {
			return m_ptr;
		}

		~CLocker() {
			std::cout << "~CLocker: " << std::this_thread::get_id() << '\n' << std::endl;
		}
	};

	CProcessControler(std::shared_ptr<T>&& ptr) :
		m_proc(ptr) { 
	}

	CProcessControler(std::shared_ptr<T>& ptr) :
		m_proc(ptr) { }

	std::shared_ptr<T> m_proc;
	auto operator->() {
		return lock_guard();
	}

	auto operator->() const {
		return lock_guard();
	}

	auto lock_guard() {
		return CLocker{ m_protector, m_proc.get() };
	}

protected:
	std::mutex m_protector;
};


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



int main(int argc, char* argv[])
{

	CProcessControler myprotectedData = std::make_shared<Foo>();

	std::thread th1 = std::thread([&] {
		myprotectedData->f1();
	});

	std::thread th2 = std::thread([&] {
		myprotectedData->f2();
	});

	std::thread th3 = std::thread([&] {
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
	return EXIT_SUCCESS;
}