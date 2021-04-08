
#include "BHM_Terminal.h"

#include <cassert>

#ifdef _WIN32
#include <Windows.h>
namespace bhd
{
	void init_win32_console_for_ansi_mode(bool verbose)
	{
		using namespace ansi::COLOR;
		try {
			static bool call_once_init_console = [=]		//Use the properties of static variables to initialize console mode once.
			{
				auto hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

				DWORD outMode = 0;
#ifdef _DEBUG
				assert(GetConsoleMode(hConsoleHandle, &outMode));
#else
				GetConsoleMode(hConsoleHandle, &outMode);
#endif

				// Enable ANSI escape codes
				outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

#ifdef _DEBUG
				assert(SetConsoleMode(hConsoleHandle, outMode));
#else
				SetConsoleMode(hConsoleHandle, outMode);
#endif
				if (verbose)
					std::cout << BOLDGREEN << "init_win32_console_for_ansi_mode was init correctly" << DEFAULT << std::endl;
				return true;
			}();
		}
		catch (...) {}
	}

}

#endif