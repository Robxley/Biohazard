#include <iostream>
#include <string>

#include "SharedGuard.h"

int main(int argc, char* argv[])
{
	
	unit_test::samethreadlock_test();
	unit_test::access_test();
	unit_test::multithread_test();

	return EXIT_SUCCESS;
}