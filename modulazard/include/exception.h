#pragma once

#include <optional>
#include <string>
#include <chrono>
#include <stdexcept>

namespace mzd
{

	namespace type_traits
	{
		/// <summary>
		/// Template object as result of function try_catch_invoke.
		/// </summary> 
		template<typename T>
		struct try_catch_invoke_result_t : std::optional<T>
		{
			std::string m_error;

			template<class TOpt, class TStr>
			try_catch_invoke_result_t(TOpt&& opt, TStr&& str) :
				std::optional<T>(std::forward<TOpt>(opt)),
				m_error(std::forward<TStr>(str))
			{}

			template<class TOpt>
			try_catch_invoke_result_t(TOpt&& opt) :
				std::optional<T>(std::forward<TOpt>(opt))
			{}

			/// <summary>
			/// Get the error message
			/// </summary>
			/// <returns>Error message</returns>
			const std::string& error() const {
				return m_error;
			}

			/// <summary>
			/// Get the error message as "const char*"
			/// </summary>
			/// <returns>Error message</returns>
			const char* what() const {
				return this->error().c_str();
			}

			/// <summary>
			/// Check if a error was occurred
			/// </summary>
			/// <returns>true if a error was occurred, else false</returns>
			bool has_error() const { return !this->has_value(); }

			/// <summary>
			/// Re-throw if a error was occurred
			/// </summary>
			void rethrow_exception() const {
				if (has_error())
					throw std::runtime_error(error());
			}
		};

		/// <summary>
		/// void specialization
		/// </summary>
		template<>
		struct try_catch_invoke_result_t<void> : std::string
		{
			template<class...Ts>
			try_catch_invoke_result_t(Ts&&... args) : std::string(std::forward<Ts>(args)...) {}

			operator bool() const { return error().empty(); }

			/// <summary>
			/// Get the error message
			/// </summary>
			/// <returns>Error message</returns>
			const std::string& error() const {
				return *this;
			}

			/// <summary>
			/// Get the error message as "const char*"
			/// </summary>
			/// <returns>Error message</returns>
			const char* what() const {
				return this->error().c_str();
			}

			/// <summary>
			/// Check if a error was occurred
			/// </summary>
			/// <returns>true if a error was occurred, else false</returns>
			bool has_error() const { return !error().empty(); }

			/// <summary>
			/// Re-throw if a error was occurred
			/// </summary>
			void rethrow_exception() const {
				if (has_error())
					throw std::runtime_error(error());
			}
		};
	}

	/// <summary>
	/// Encapsulation of invokable code with try catch (working around std::invoke)
	/// Code Example :
	/// if( auto test = try_catch_invoke([]{ std::cout << "Really, my ingenious code cannot fail !\n"; }) ; test.has_error() ) 
	/// { 
	///		std::cout << "Well, Well, maybe not that so genius.";
	/// }
	/// </summary>
	/// <param name="...args">Invocable argument list. See std::invoke</param>
	/// <returns>Return try_catch_invoke_result_t object. Use bool operator convertion to check</returns>
	template <class...Ts> auto try_catch_invoke(Ts && ...args)
	{
		using namespace type_traits;

		//Get the type of invoked function
		using result_t = std::invoke_result_t<Ts...>;

		//Generate the result type of try_catch_invoke
		using try_catch_invoke_result_t = try_catch_invoke_result_t<result_t>;

		auto make_error_result = [](auto&& arg) -> try_catch_invoke_result_t
		{
			if constexpr (std::is_void_v<result_t>)
				return  std::forward<decltype(arg)>(arg);
			else
				return { std::nullopt, std::forward<decltype(arg)>(arg) };
		};

		try
		{
			if constexpr (std::is_void_v<result_t>) {
				std::invoke(std::forward<Ts>(args)...);
				return try_catch_invoke_result_t{};
			}
			else {
				return try_catch_invoke_result_t(std::invoke(std::forward<Ts>(args)...));
			}

		}
		catch (const std::exception& e) {
			return make_error_result(e.what());
		}
		catch (...) {
			return make_error_result("Unknown exception");
		}

	}

	/// <summary>
	/// Encapsulation of try_catch_invoke with a timer.
	/// Code Example :
	/// if( auto [test, chrono] = try_catch_invoke([]{ std::cout << "Really, my ingenious code cannot fail !\n"; }) ; test.has_error() ) 
	/// { 
	///		std::cout << "Well, maybe not that so ingenious.";
	/// }
	/// else
	/// {
	///		std::cout << "So ingenious, so fast with a execution time: " << chrono.count();
	/// }
	/// </summary>
	/// <param name="...args">Invocable argument list. See std::invoke</param>
	/// <returns>Tuple as { try_catch_invoke_result_t, time value } </returns>
	template <class...Ts> auto try_catch_invoke_chrono(Ts && ... args)
	{
		using clock = std::chrono::high_resolution_clock;
		auto time = clock::now();
		auto result = try_catch_invoke(std::forward<Ts>(args)...);
		auto span = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - time);
		return std::make_tuple(std::move(result), std::move(span));
	}

	namespace compile_tests
	{
#ifdef _DEBUG
		inline void compile_tests_try_catch_invoke()
		{
			if(auto test_void = try_catch_invoke([] {}); test_void.has_error())
			{ }

			if (auto test_t = try_catch_invoke([] { return 0.3f; }); test_t.has_error())
			{ }

			if (auto [test_t, chrono] = try_catch_invoke_chrono([] { return 0.3f; }); test_t.has_error())
			{ }

			if (auto test_t = try_catch_invoke([] { return 0.3f; }); !test_t)
			{ }
		}
#endif
	}

}