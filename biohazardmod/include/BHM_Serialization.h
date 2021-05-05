#pragma once

#include <string>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <charconv>
#include <type_traits>

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
			using to_osstream_t = decltype((std::ostringstream() << std::declval<T>()).str());

			template <class T>
			using to_osstream_void_t = std::void_t<to_osstream_t<T>>;

			template<class, class = void >
			struct has_osstream : std::false_type { };

			template<class T>
			struct has_osstream<T, to_osstream_void_t<T>> : std::is_same<std::string, to_osstream_t<T>> { };

			//foo checking
			template <class T>
			using is_constructible_to_string = std::is_constructible<std::string, T>;

			template <typename T>
			static constexpr bool is_serializable_v = has_to_string<T>::value || has_osstream<T>::value || is_constructible_to_string<T>::value || is_path<T>::value;

		}


		template <class T>
		std::string to_string(T&& value)
		{
			using namespace details;
			if constexpr (is_constructible_to_string<T>::value)
				return std::string(std::forward<T>(value));
			else if constexpr (is_path<T>::value)
				return value.generic_string();
			else if constexpr (has_to_string<T>::value)
				return std::to_string(std::forward<T>(value));
			else if constexpr (has_osstream<T>::value)
				return (std::ostringstream() << value).str();
			else
			{
				static_assert(false, "Not implemented")
			}
		}
	}

	namespace wserialization
	{

		// --- to_wstring ---
		// -----------------
		namespace details
		{
			//to_wstring checking function
			template <class T>
			using to_wstring_t = decltype(std::to_wstring(std::declval<T>()));

			template <class T>
			using to_wstring_void_t = std::void_t<to_wstring_t<T>>;

			template<class, class = void >
			struct has_to_wstring : std::false_type { };

			template<class T>
			struct has_to_wstring<T, to_wstring_void_t<T>> : std::is_same<std::wstring, to_wstring_t<T>> { };

			//owstringstream checking function
			template <class T>
			using to_wosstream_t = decltype((std::wostringstream() << std::declval<T>()).str());

			template <class T>
			using to_wosstream_void_t = std::void_t<to_wosstream_t<T>>;

			template<class, class = void >
			struct has_wosstream : std::false_type { };

			template<class T>
			struct has_wosstream<T, to_wosstream_void_t<T>> : std::is_same<std::wstring, to_wosstream_t<T>> { };

			//foo checking
			template <class T>
			using is_constructible_to_wstring = std::is_constructible<std::wstring, T>;

			template <typename T>
			struct is_path :
				std::is_same<typename std::decay<T>::type, std::filesystem::path>::type
			{};

			template <typename T>
			static constexpr bool is_serializable_v = has_to_wstring<T>::value || has_wosstream<T>::value || is_constructible_to_wstring<T>::value || is_path<T>::value;

		}

		template <class T>
		std::wstring to_wstring(T&& value)
		{
			using namespace details;
			if constexpr (is_constructible_to_wstring<T>::value)
				return std::wstring(std::forward<T>(value));
			else if constexpr (is_path<T>::value)
				return value.generic_wstring();
			else if constexpr (has_to_wstring<T>::value)
				return std::to_wstring(std::forward<T>(value));
			else if constexpr (has_wosstream<T>::value)
				return (std::wostringstream() << value).str();
			else
			{
				static_assert(false, "Not implemented")
			}
		}
	}


	namespace unserialization
	{
		
		//Single object
		template <class T>
		auto to_data(const std::string& str, T& data)
			-> decltype(std::istringstream(str) >> data, void())
		{
			std::istringstream(str) >> data;
		}

		template <class T>
		auto to_data(const std::wstring& wstr, T& data)
			-> decltype(std::wistringstream(wstr) >> data, void())
		{
			std::wistringstream(wstr) >> data;
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
			assert_static(0, "Conversion from string not yet implemented");
		}

	}

}
