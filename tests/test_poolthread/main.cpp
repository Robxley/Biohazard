
#include <iostream>
#include <mutex>

#include "BHM_ThreadPool.h"

//Make cout thread safe
std::mutex m_safe_cout;
#define safe_cout(msg) { std::unique_lock<std::mutex> lck(m_safe_cout);  std::cout << msg << std::endl; }

void thread_sleep(int i) {
	std::this_thread::sleep_for(std::chrono::seconds(i));
}

class ThreadVerbose
{
	public:
	ThreadVerbose(const std::string& cmsg) : smsg(cmsg) {
		auto id = std::this_thread::get_id();
		safe_cout("Start :" << smsg.c_str() << " : " << id);
	};
	~ThreadVerbose() {
		safe_cout("End :" << smsg.c_str());
	}
	std::string smsg;
};


using Foo = struct
{
	int x, y;
};

/// <summary>
/// Simple test function.
/// Create a task and collect results.
/// </summary>
void SimpleTasks()
{

	std::cout << "Simple Tasks:" << std::endl;

	//Create/Get a pool thread (singleton) with a thread size set to 2.
	auto& pool = bhd::thread_pool::instance(2);

	//Classic task creation - Boring task creation...
	//--------------------------------------------
	using task_void_t = bhd::threaded_task<void>;
	task_void_t task_constructor([] {
		ThreadVerbose vth("Task constructor");
		thread_sleep(3);
	});
	pool.enqueue(task_constructor);

	using task_float_t = bhd::threaded_task<float>;
	task_float_t task_setter;
	task_setter.set([] {
		ThreadVerbose vth("Task setter");
		thread_sleep(2);
		return 3.14f;
	});
	pool.enqueue(task_setter);

	//Task creation by pool enqueue - faster / easier way
	//--------------------------------------------
	auto task_void = pool.enqueue([] {
			ThreadVerbose vth("Task void");
			thread_sleep(3);
	});

	auto task_int = pool.enqueue([] {
			ThreadVerbose vth("Task int");
			thread_sleep(2);
			return 123;
	});

	auto task_foo = pool.enqueue([] {
			ThreadVerbose vth("Task Foo");
			thread_sleep(1);
			return Foo{ 2,3 };
	});

	auto task_vector = pool.enqueue([] {
			ThreadVerbose vth("Task Foo");
			thread_sleep(1);
			return std::string{ "abcd" };
	});

	//Join party
	{
		task_constructor.get();						// task returning a "void value"
		float setter_result = task_setter.get();	// task returning a float value

		task_void.get();							// task returning a void value
		int int_result = task_int.get();			// task returning a int value
		Foo foo_result = task_foo.get();			// task returning a custom structure value
		std::string str = task_vector.get();        // task returning a string structure value

		safe_cout("Task using setter : " << setter_result);
		safe_cout("Task int result: " << int_result);
		safe_cout("Task foo structure result: " << foo_result.x << ":" << foo_result.y);
		safe_cout("Task vector result: " << str.c_str());
	}

}



/// <summary>
/// Advanced tests
/// Try a dead lock in the cascading task call.
///	Of course, we expect that to be not possible.
/// </summary>
void TryDeadLock()
{
	std::cout << "Try dead locks:" << std::endl;

	//Create/Get a pool thread (singleton) with a thread size set to 2.
	auto& pool = bhd::thread_pool::instance(2);

	//Fill completely the thread pool (of size 2) with two tasks.
	//Each of these tasks creates also two new tasks.
	//At the end, the thread pool is fed with 6 tasks, 4 of them are inside 2 of them.

	//Create a task containing two tasks
	auto thA = pool.enqueue([&pool]
		{
			ThreadVerbose vth1("thA");
			thread_sleep(1);

			auto thA_1 = pool.enqueue([] {
				ThreadVerbose vth1("thA_1");
				thread_sleep(3);
				});

			auto thA_2 = pool.enqueue([] {
				ThreadVerbose vth1("thA_2");
				thread_sleep(2);
				});

			thA_1.get();
			thA_2.get();
		});

	//Create a task containing two tasks
	auto thB = pool.enqueue([&pool]
		{
			ThreadVerbose vth1("thB");
			thread_sleep(1);

			auto thB_1 = pool.enqueue([] {
				ThreadVerbose vth1("thB_1");
				thread_sleep(3);
				});

			auto thB_2 = pool.enqueue([] {
				ThreadVerbose vth1("thB_2");
				thread_sleep(2);
				});

			thB_1.get();
			thB_2.get();
		});


	thB.get();
	thA.get();
}



int main()
{
	//Simple task testing
	SimpleTasks();

	//Some task create new tasks. Check if deadlock is avoided
	TryDeadLock();

	system("Pause");
	return 0;
}