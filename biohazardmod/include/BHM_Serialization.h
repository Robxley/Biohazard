#pragma once

#include <string>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <charconv>
#include <type_traits>
#include <array>

namespace bhd
{
	
	namespace serialization
	{
		template<typename ... Args>
		std::string concat_to_string(Args&& ...args) {
			return (std::stringstream() << ... << std::forward<Args>(args)).str();
		}
		
		namespace details
		{
			template <typename T>
			struct is_path :
				std::is_same<typename std::decay<T>::type, std::filesystem::path>::type
			{};
		}

		// --- to_string ---
		// -----------------
		namespace details
		{
			//to_string checking function
			template <class T>
			using to_string_t = decltype(std::to_string(std::declval<T>()));

			template <class T>
			using to_string_void_t = std::void_t<to_string_t<T>>;

			template<class, class = void >
			struct has_to_string : std::false_type { };

			template<class T>
			struct has_to_string<T, to_string_void_t<T>> : std::is_same<std::string, to_string_t<T>> { };

			//ostringstream checking function
			template <class T>
			using to_ostream_t = decltype((std::ostringstream() << std::declval<T>()).str());

			template <class T>
			using to_ostream_void_t = std::void_t<to_ostream_t<T>>;

			template<class, class = void >
			struct has_ostream : std::false_type { };

			template<class T>
			struct has_ostream<T, to_ostream_void_t<T>> : std::is_same<std::string, to_ostream_t<T>> { };

			//foo checking
			template <class T>
			using is_constructible_to_string = std::is_constructible<std::string, T>;

			template <typename T>
			static constexpr bool is_serializable_v = has_to_string<T>::value || has_ostream<T>::value || is_constructible_to_string<T>::value || is_path<T>::value;

		}


		template <class T>
		std::enable_if_t<details::is_serializable_v<T>, std::string>
		to_string(T&& value)
		{
			using namespace details;
			if constexpr (is_constructible_to_string<T>::value)
				return std::string(std::forward<T>(value));
			else if constexpr (is_path<T>::value)
				return value.generic_string();
			else if constexpr (has_to_string<T>::value)
				return std::to_string(std::forward<T>(value));
			else if constexpr (has_ostream<T>::value)
				return (std::ostringstream() << value).str();
		}

		namespace type_tokens
		{
			inline constexpr const char * cs_default = " ";
			inline thread_local static auto separator = cs_default;
			namespace brackets
			{
				inline thread_local static bool disable = true;
				inline thread_local static auto begin = "[";
				inline thread_local static auto end = "]";
			}
			inline static void reset() { separator = cs_default; }
		};

		namespace details
		{

			template<typename T>
			void buffer_to_ostream_with_separator(std::ostream& os, T* buffer, const std::size_t& count)
			{
				if (count == 0) return;
				os << buffer[0];
				std::for_each(buffer+1, buffer+count, [&](auto& v) {os << type_tokens::separator << v; });
			}

			template<auto N, auto I = 0, typename T>
			void buffer_to_ostream_with_separator(std::ostream& os, T* buffer)
			{
				if constexpr (I < N)
				{
					if constexpr (I == 0)
						os << buffer[0];
					else
						os << type_tokens::separator << buffer[I];
					buffer_to_ostream_with_separator<N, I + 1>(os, buffer);
				}
			}

			template<typename First, typename ... Args>
			void to_ostream_with_separator(std::ostream& os, First&& first, Args&& ...args)
			{
				os << first;
				((os << type_tokens::separator) << ... << std::forward<Args>(args));
			}

			template<typename ... Args>
			std::string to_string_sep(Args&& ...args)
			{
				std::stringstream ss;
				to_ostream_with_separator(ss, std::forward<Args>(args)...);
				return ss.str();
			}

			template <class Func>
			std::string func_to_string_tokens(Func&& func)
			{
				std::stringstream ss;
				if (!type_tokens::brackets::disable)
				{
					ss << type_tokens::brackets::begin;
					std::forward<Func>(func)(ss);
					ss << type_tokens::brackets::end;
				}
				else
					std::forward<Func>(func)(ss);
				return ss.str();
			};
		}

		template<typename ... Args>
		std::string to_string_tokens(Args&& ...args)
		{
			auto func = [&args...](std::ostream& os) {
				details::to_ostream_with_separator(os, std::forward<Args>(args)...);
			};
			return details::func_to_string_tokens(func);
		}

		template<auto N, typename T>
		std::string buffer_to_string_tokens(T * buffer)
		{
			auto func = [&](std::ostream& os) {
				details::buffer_to_ostream_with_separator<N>(os, buffer);
			};
			return details::func_to_string_tokens(func);
		}

		template<typename T>
		std::string buffer_to_string_tokens(T* buffer, const std::size_t& count)
		{
			auto func = [&](std::ostream& os) {
				details::buffer_to_ostream_with_separator(os, buffer, count);
			};
			return details::func_to_string_tokens(func);
		}
	}

	namespace deserialization
	{
		
		//Single object
		template <class T>
		auto to_data(const std::string& str, T& data)
			-> decltype(std::istringstream(str) >> data, void())
		{
			std::istringstream(str) >> data;
		}

		inline auto to_data(const std::string& str, std::filesystem::path& data) {
			data = str;
		}

		//list of object (like array, vector ...)

		template <typename T>
		struct type_tokens {
			static constexpr auto tokens() {
				if constexpr (std::is_integral_v<T>)
					return std::array{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9','-' };
				else if constexpr (std::is_floating_point_v<T>)
					return std::array{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9','-', '.' };
				else
					static_assert(0, "Unsupported type");
			}
		};

		template <typename T>
		void to_data(const std::string& str, T* buffer, std::size_t size)
		{
			std::size_t i = 0;

			const char* ptr_start = str.data();
			const char* ptr_end = str.data() + str.size();

			auto tokens = type_tokens<T>::tokens();

			while (ptr_start < ptr_end && i < size)
			{
				ptr_start = std::find_first_of(ptr_start, ptr_end, tokens.begin(), tokens.end());
				if (ptr_start == ptr_end)
					return;

				if (auto [ptr, ec] = std::from_chars(ptr_start, ptr_end, buffer[i]); ec == std::errc()) {
					ptr_start = ptr;
					i++;
				}
				else {
					return;
				}
			}
		}

		template <typename T, std::size_t N>
		void to_data(const std::string& str, std::array<T, N>& buffer) {
			to_data(str, buffer.data(), N);
		}

		template<typename T, typename V = typename T::value_type>
		using has_emplace_back = decltype(std::declval<T>().emplace_back(std::declval<V>()), void());

		template<typename T>
		auto to_data(const std::string& str, T& buffer) -> has_emplace_back<T>   //Like std  containers (vector, deque, ...)
		{
			const char* ptr_start = str.data();
			const char* ptr_end = str.data() + str.size();

			using value_type_t = typename T::value_type;
			auto tokens = type_tokens<value_type_t>::tokens();

			value_type_t value;
			while (ptr_start < ptr_end)
			{
				ptr_start = std::find_first_of(ptr_start, ptr_end, tokens.begin(), tokens.end());
				if (ptr_start == ptr_end)
					return;

				if (auto [ptr, ec] = std::from_chars(ptr_start, ptr_end, value); ec == std::errc()) {
					buffer.emplace_back(std::move(value));
					ptr_start = ptr;
				}
				else {
					return;
				}
			}
		}

		template<typename T>
		inline void to_data(...)
		{
			static_assert(0, "Conversion from string not yet implemented");
		}

	}

}
