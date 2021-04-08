#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <any>
#include <algorithm>
#include <chrono>

#include "BHM_Common.h"
#include "BHM_Terminal.h"

#define ASSERT_TEST(test) mzd::try_catch_terminal("test \"" ## #test ## "\": ", [&]{ if(!(test)) throw std::runtime_error("Invalid Test");});

namespace bhd
{

	namespace unit_test  //unit test tools
	{

		//Unit test manager
		class unit_test_manager
		{
			using clock = std::chrono::high_resolution_clock;
			clock::time_point t1 = clock::now();
			std::any m_expected_result;

		public:

			enum class TIMER {
				ON = 0,
				OFF
			};

			std::string m_text;
			int32_t m_setw = 64;
			char m_fillc = '.';
			TIMER m_flag_timer = TIMER::OFF;

			template<class T>
			auto& operator<<(T&& arg)
			{
				using namespace logging::color;
				using TArg = typename std::decay_t<decltype(arg)>;

				auto & terminal = sync_terminal();

				if constexpr (std::is_same_v<TArg, TIMER>)
				{
					m_flag_timer = arg;
					time_reset();
				}
				else if constexpr (type_traits::is_string_like<TArg>::value)		//test text
				{
					m_text = arg;
					terminal << TEXT << m_text << std::flush;
					time_reset();
				}
				else if constexpr (std::is_same_v<TArg, bool>)
				{
					if (arg == false)
						terminal_ko("false");
					else
						terminal_ok("true");
				}
				else if constexpr (std::is_invocable_v<TArg>)	//Invocable code to check
				{
					try
					{
						if constexpr (std::is_void_v<std::invoke_result_t<TArg>>)
						{
							//Check invocable code without exception
							std::invoke(arg);
							terminal_ok("done");
						}
						else if constexpr (std::is_same_v<std::invoke_result_t<TArg>, bool>)
						{
							//Check invocable code with boolean checking
							auto result = std::invoke(arg);
							if (result == false)
								terminal_ko("false");
							else
								terminal_ok("true");
						}
						else
						{
							//Check invocable code with result checking
							auto result = std::invoke(arg);
							if (m_expected_result.has_value())
							{
								bool compare = std::any_cast<decltype(result)>(m_expected_result) == result;
								if (compare == false)
									terminal_ko("different value");
								else
									terminal_ok("same value");
							}
						}

					}
					catch (const std::exception& e)
					{
						terminal_ko(std::string("Exception:") + e.what());
					}
					catch (...)
					{
						terminal_ko("Exception unknown");
					}
				}
				else
				{
					//Set a waiting result before test
					terminal << TEXT << " (" << typeid(arg).name() << ')';
					m_expected_result = std::make_any<T>(std::forward<T>(arg));
				}

				time_span();

				return *this;
			}

			//Output alignment
			std::ostream & terminal_setw()
			{
				auto& terminal = sync_terminal();
				m_text.resize(m_setw, m_fillc);
				terminal << '\r' << logging::color::TEXT << m_text;
				m_text.clear();
				return (terminal);
			}

			//Output message result
			std::ostream& terminal_ok(const std::string& msg) {
				return (terminal_setw() << logging::color::OK << "-> OK (" << msg << ") " << time_span() << logging::color::TEXT << std::endl);
			}
			std::ostream& terminal_ko(const std::string& msg) {
				return (terminal_setw() << logging::color::KO << "-> KO (" << msg << ") " << time_span() << logging::color::TEXT << std::endl);
			}

			std::string time_span()
			{
				std::string stime;
				if (m_flag_timer == TIMER::ON)
				{
					clock::time_point t2 = clock::now();
					std::chrono::milliseconds int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
					t1 = std::move(t2);
					stime = " (" + std::to_string(int_ms.count()) + "ms) ";
				}
				return stime;
			}

			void time_reset() {
				t1 = clock::now();
			}
		};


		template<typename ...Args>
		inline void Title(Args&&... args)
		{
			using namespace logging::color;
			auto terminal = sync_terminal();
			terminal << '\n' << SEPARATOR << TITLE;
			(terminal << ... << std::forward<Args>(args));
			terminal << SEPARATOR << std::endl;
		}

		template<typename ...Args>
		inline void Title2(Args&&... args)
		{
			using namespace logging::color;
			auto terminal = sync_terminal();
			terminal << TITLE;
			(terminal << ... << std::forward<Args>(args));
			terminal << ENDL << std::flush;
		}


		inline void unit_test_tests()
		{

			ansi::unit_test_ainsi_color();

			Title("Title test");

			unit_test_manager test;

			auto fake = std::exception("Fake exception");


			test << "Test const char* title" << [] { int i = 0; };
			test << std::string("Test string title") << [] { int i = 0; };
			test << "False boolean test" << false;
			test << "True boolean test" << true;
			test << "Trow exception test" << [=] {throw fake; };
			test << "Invocable return true" << [] { return true; };
			test << "Invocable return false" << [] { return false; };

			test
				<< "Test same float value"
				<< 3.14f
				<< [] { return 3.14f; };

			test
				<< "Test different int value"
				<< 5
				<< [] { return 7; };

			test
				<< "Single title -  Multi test"
				<< [] { int i = 0; }					//Perform a piece of code without exception
			<< [=] { throw fake; }						//Perform a piece of code with exception
			<< [] { return true; }						//Invocable code with boolean result (test true)
			<< [] { return false; }						//Invocable code with boolean result (test false)
				<< true									//True boolean test 
				<< false								//False boolean test 	
				<< [=] { throw fake; }					//Exception test 
				<< 3.15f								//Expected value
				<< [] {return 3.15f; }					//Invocable return value
			<< [] {return 3.16f; };						//Invocable return value
		}

	}  //end namespace 
}	//end namespace 