#include <iostream>


#include "DataGuard.h"


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

	CDataGuard myprotectedData = std::make_shared<Foo>();

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

	return EXIT_SUCCESS;
}