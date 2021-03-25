#pragma once

#include <string>
#include <type_traits>
#include <optional>
#include <utility>
#include <mutex>
#include <chrono>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <cassert>
#include <variant>
#include <functional>

#define FWD(a) std::forward<decltype(a)>(a)

namespace mzd	
{
	/// <summary>
	/// Useful trait template
	/// </summary>

	namespace type_traits
	{

		template <typename T, typename U>
		struct decay_equiv :
			std::is_same<typename std::decay<T>::type, U>::type
		{};

		template <typename T>
		struct is_wstring_like {
			static const bool value = decay_equiv<T, const wchar_t*>::value || decay_equiv<T, std::wstring>::value;
		};

		template <typename T>
		struct is_string_like {
			static const bool value = decay_equiv<T, const char*>::value || decay_equiv<T, std::string>::value;
		};

		template <typename T>
		struct is_path :
			decay_equiv<T, std::filesystem::path>::type
		{};

	}

	template<typename T, typename... N>
	auto make_array(N&&... args) -> std::array<T, sizeof...(args)>
	{
		return { std::forward<N>(args)... };
	}
	
	template <typename T>
	struct object_wrapper
	{
	private:
		using m_func_wrapper_t = std::function<T*(void)>;	//Save the way how to wrap a object instance
		m_func_wrapper_t m_func;
	public:
		object_wrapper(const object_wrapper&) = default;
		object_wrapper(object_wrapper&&) = default;
		object_wrapper& operator=(object_wrapper&&) = default;
		object_wrapper& operator=(const object_wrapper&) = default;

		//! Contain the object itself using the default constructor
		object_wrapper() { m_func = [o = T{}]() mutable ->auto*  { return &o; }; }

		//! Keeps the address of an already instantiated object.
		object_wrapper(const std::shared_ptr<T> & p) { set(p); }

		object_wrapper(T & p) { set(p); }
		object_wrapper(T && p) { set(std::move(p)); }
		object_wrapper(T * p) { set(*p); }

		//! Constructs an object of type T and wraps it in a object_wrapper<T>
		template<typename ...Args> static auto make(Args&&... args) {
			return object_wrapper(std::move(T(std::forward<Args>(args)... )));
		}

		void set(T& r) { 
			m_func = [ptr = &r]() mutable ->auto*  { return ptr; };
		}

		void set(T&& p) {
			m_func = [o = std::move(std::forward<T>(p))]() mutable ->auto*  { return &o; };
		}

		void set(const std::shared_ptr<T> & p) {
			m_func = [=]() mutable ->auto*  { return p.get();	};
		}

		template<typename ...Args>
		void create(Args&&... args) {
			m_func = [o = T(std::forward<Args>(args)...)]() mutable ->auto*  { return &o; };
		}

		void copy(const T & obj) {
			m_func = [o = T(obj)]() mutable ->auto*  { return &o; };
		}

		void reset() { 
			m_func = {}; 
		}

		template<typename Obj>
		object_wrapper& operator=(Obj && obj) {
			set(std::forward<Obj>(obj));
		}

		T& value() { return *m_func(); }
		const T& value() const { return *m_func(); }
		operator T& () { return value(); }
		operator const T& () const { return value(); }
		T* operator->() { return &value(); }
		const T* operator->() const { return &value(); }
		T& operator*() { return value(); }
		const T& operator*() const { return value(); }

	};


	/// <summary>
	/// Concatenate an argument list into a single string.
	/// </summary>
	/// <param name="...args">argument list</param>
	/// <returns>Concatenated string</returns>
	template<typename ...Args>
	std::string concat_to_string(Args&&... args)
	{
		std::stringstream ss;
		(ss << ... << std::forward<Args>(args));
		return ss.str();
	}

	template<typename T>
	std::enable_if_t<type_traits::is_string_like<T>::value, T> 
	concat_to_string(T && arg)  {
		return std::forward<T>(arg);
	}

	/// <summary>
	/// Synchronization of a std::ostream object
	/// </summary>
	class sync_stream : public std::ostream
	{
		class sync_streambuf : public std::stringbuf
		{
			std::ostream& m_output;
			std::mutex& m_mutex;

		public:
			sync_streambuf(std::ostream& str, std::mutex& mutex)
				: m_output(str), m_mutex(mutex)
			{}

			sync_streambuf(const sync_streambuf& buffer) :
				sync_streambuf(buffer.m_output, buffer.m_mutex)
			{}

			sync_streambuf(const sync_stream& stream) :
				sync_streambuf(stream.m_buffer)
			{}

			sync_streambuf& operator= (const sync_streambuf& buffer) {
				*this = sync_streambuf{ buffer.m_output, buffer.m_mutex };
				return *this;
			}
	
			int sync() override
			{
				auto lckgd = std::lock_guard(m_mutex);
				m_output << (this->str());					// send the data of the stream buff
				str("");
				m_output.flush();
				return 0;
			}

		};

		sync_streambuf m_buffer;
	public:

		template<class...Ts>
		sync_stream(Ts&&... args)
			: m_buffer(std::forward<Ts>(args)...), std::ostream(&m_buffer)
		{}

		sync_stream(const sync_stream& ss) :
			sync_stream(ss.m_buffer) {}

		~sync_stream() {
			this->flush();
		}

	};

}