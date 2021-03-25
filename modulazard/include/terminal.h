#pragma once

#include "exception.h"
#include "util_traits.h"
#include <memory>
#include <regex>

namespace mzd
{

	// ANSI Terminal manipulation based on :
	// https://en.wikipedia.org/wiki/ANSI_escape_code
	// http://ascii-table.com/ansi-escape-sequences.php

	namespace ansi
	{

		//auto ansi_escape = std::regex("[\033]\\[[[:alnum:]]?[[:alnum:]]\\m");
		constexpr auto ansi_escape = R"(\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~]))";

		inline std::string remove_ansi_escape(const std::string & str) {
			std::regex r(ansi_escape);
			return std::regex_replace(str, r, "");
		}

		constexpr auto* ESC = "\033";

		namespace CURSOR
		{
			constexpr auto* RESET = "\033[;H";
			constexpr auto* ERASE_LINE = "\033[K";
		}

		namespace COLOR
		{
			constexpr auto* DEFAULT = "\033[0m";	/* DEFAULT color */
			constexpr auto* BLACK = "\033[30m";	/* Black */
			constexpr auto* RED = "\033[31m";	/* Red */
			constexpr auto* GREEN = "\033[32m";	/* Green */
			constexpr auto* YELLOW = "\033[33m";	/* Yellow */
			constexpr auto* BLUE = "\033[34m";	/* Blue */
			constexpr auto* MAGENTA = "\033[35m";	/* Magenta */
			constexpr auto* CYAN = "\033[36m";	/* Cyan */
			constexpr auto* WHITE = "\033[37m";	/* White */
			constexpr auto* BOLDBLACK = "\033[1m\033[30m";	/* Bold Black */
			constexpr auto* BOLDRED = "\033[1m\033[31m";	/* Bold Red */
			constexpr auto* BOLDGREEN = "\033[1m\033[32m";	/* Bold Green */
			constexpr auto* BOLDYELLOW = "\033[1m\033[33m";	/* Bold Yellow */
			constexpr auto* BOLDBLUE = "\033[1m\033[34m";	/* Bold Blue */
			constexpr auto* BOLDMAGENTA = "\033[1m\033[35m";	/* Bold Magenta */
			constexpr auto* BOLDCYAN = "\033[1m\033[36m";	/* Bold Cyan */
			constexpr auto* BOLDWHITE = "\033[1m\033[37m";	/* Bold White */
		}

	}

	namespace logging
	{
		namespace color
		{
			constexpr auto* TEXT = ansi::COLOR::DEFAULT;			/* Message Text */
			constexpr auto* TITLE = ansi::COLOR::BOLDCYAN;			/* Message Title */
			constexpr auto* OK = ansi::COLOR::BOLDGREEN;			/* Message OK */
			constexpr auto* KO = ansi::COLOR::BOLDRED;				/* Message KO */
			constexpr auto* ERROR = KO;								/* Message Error */
			constexpr auto* WARNING = ansi::COLOR::BOLDYELLOW;		/* Message warning*/
			constexpr auto* SEPARATOR = "\033[0m--------------------------------------";
			constexpr auto* RESET = TEXT;							/* Reset as message text */
			constexpr auto* ENDL = "\033[0m \n";					/* New line with message text */
		}

		//auto make_error_msg()
	}


#ifdef _WIN32
	/// <summary>
	/// Enable the ANSI mode for windows
	/// It's not natively enable for Window console.
	/// It's a "call once" function. Only the first execution of this function do something
	/// </summary>
	void init_win32_console_for_ansi_mode(bool verbose = false);
#else
	inline void init_win32_console_for_ansi_mode(bool verbose = false) { void; }
#endif


	/// <summary>
	/// _terminal object namespace
	/// </summary>
	namespace terminal_stuffs
	{
		/// <summary>
		/// Anonymous terminal mutex
		/// </summary>
		/// <returns>terminal mutex</returns>
		inline auto& terminal_mutex() {
			static std::mutex terminal_mutex;
			return terminal_mutex;
		}

		/// <summary>
		/// Anonymous terminal lock guard
		/// </summary>
		/// <returns>lock guard mutex for terminal</returns>
		inline auto terminal_lock_guard() {
			return std::lock_guard(terminal_mutex());
		}


		/// <summary>
		/// anonymous default terminal stream.
		/// With Window console activation ansi mode as "call once" object
		/// </summary>
		/// <returns></returns>
		inline auto& terminal_stream()
		{
#ifdef _WIN32
			static const bool init_win32_console_for_ansi_mode_call_once = [] { init_win32_console_for_ansi_mode(); return true; }();
#endif
			return std::cout;
		}

	}


	/// <summary>
	/// Printing terminal function.
	/// Thread safe. 
	/// </summary>
	/// <param name="...args">What you want to print. Ex: terminal("my int: ", 10, "my float: ", 5.0f)</param>
	template<typename ...Args> void terminal(Args&&... args)
	{
		using namespace terminal_stuffs;
		if constexpr (sizeof...(args) == 1)
		{
			auto lckgd = terminal_lock_guard();
			(terminal_stream() << ... << std::forward<Args>(args)) << std::flush;
		}
		else if (sizeof...(args) > 1)
		{
			auto message = concat_to_string(std::forward<Args>(args)...);
			{
				auto lckgd = terminal_lock_guard();
				terminal_stream() << message << std::flush;
			}
		}
	}

	/// <summary>
	/// Overload of "terminal" function Useful.
	/// A new line is automatically add at the end.
	/// Thread safe. 
	/// </summary>
	/// <param name="...args">What you want to print. Ex: terminal("my int: ", 10, "my float: ", 5.0f)</param>
	template<typename ...Args> void terminal_endl(Args&&... args) {
		terminal(std::forward<Args>(args)..., '\n');
	}

	/// <summary>
	/// Synchronized terminal stream object. Working like std::cout.
	/// Thread safe.
	/// </summary>
	/// <returns></returns>
	inline sync_stream& sync_terminal() {
		using namespace terminal_stuffs;
		thread_local static sync_stream terminal = { terminal_stream(), terminal_mutex() };
		return terminal;
	}

	namespace core
	{
		template <class...Ts>
		auto try_catch_terminal_invoke(Ts&&... args)
		{
			using namespace logging::color;
			auto[result, chrono] = try_catch_invoke_chrono(std::forward<Ts>(args)...);
			if (result.has_error()) {
				sync_terminal() << ERROR << "error (" << result.what() << ")" << ENDL << std::flush;
			}
			else {
				sync_terminal() << OK << "done (" << chrono.count() << "ms)" << ENDL << std::flush;
			}
			return std::make_tuple(std::move(result), std::move(chrono));
		}

		template <class T, class...Ts>
		auto terminal_until_invocable(T&& first, Ts&&... args)
		{
			if constexpr (std::is_invocable_v<T, Ts...> == false)
			{
				sync_terminal() << std::forward<T>(first) << std::flush;
				return terminal_until_invocable(std::forward<Ts>(args)...);
			}
			else
			{
				sync_terminal() << std::flush;
				return try_catch_terminal_invoke(std::forward<T>(first), std::forward<Ts>(args)...);
			}
		}
	}

	template <class...Ts>
	auto try_catch_terminal(Ts&&... args)
	{
		return core::terminal_until_invocable(std::forward<Ts>(args)...);
	}

#ifdef _DEBUG
	inline void compile_test_try_catch_terminal()
	{
		auto test0 = core::try_catch_terminal_invoke([] { return std::string("fake"); });
		auto test1 = try_catch_terminal([] { return std::string("fake"); });
		auto test2 = try_catch_terminal("title fake", [] { return std::string("fake"); });
		auto test3 = try_catch_terminal(std::string("float fake"), 32.0f, [] { return std::string("fake"); });
	}
#endif


	namespace ansi
	{

		inline void unit_test_ainsi_color()
		{
#ifdef _DEBUG
			using namespace logging::color;
			terminal_endl(TITLE, "Random Colors\n");
			terminal_endl(SEPARATOR);
			terminal_endl(COLOR::BLUE, "Blue Color");
			terminal_endl(COLOR::RED, "RED Color");
			terminal_endl(COLOR::DEFAULT, "Reset Color");
			terminal_endl(SEPARATOR);
			terminal_endl(TITLE, "Message Tests:");
			terminal_endl(SEPARATOR);
			terminal_endl(OK, "Message OK");
			terminal_endl(KO, "Message Fail/KO:");
			terminal_endl(WARNING, "Message Warning");
#endif
		}

	}
}