#pragma once

#include <optional>
#include <cstdlib>
#include <typeindex>
#include <opencv2\opencv.hpp>
#include "BHM_Serialization.h"

namespace bhd
{

	//Configurable default types (for quick recompilation needs)
	namespace configurable_types
	{
		using key_t = std::string;
		using string_t = std::string;
	}

	/// <summary>
	/// Configurable class interface
	/// A configurable is describe with :
	///		- a key 
	///		- a description
	///		- a blurb
	/// </summary>
	class IConfigurable
	{

	public:
		using key_t = typename configurable_types::key_t;
		using string_t = typename configurable_types::string_t;

		const key_t m_sKey;				//! Configurable Key
		const string_t m_sInfo;			//! Info / description
		const string_t m_sBlurb;		//! short description

		IConfigurable(const IConfigurable&) = delete;
		
	public:

		/// <summary>
		/// Configurable class
		/// </summary>
		/// <param name="key">KEY for identification</param>
		/// <param name="info">Name of the configurable (short description)</param>
		/// <param name="blurb">Blurb of the configurable (long description)</param>
		IConfigurable(const key_t & key, const string_t & info, const string_t & blurb) :
			m_sKey(key), m_sInfo(info), m_sBlurb(blurb) {}

		//! Return the configurable key
		auto & GetKey() const	{ return m_sKey;	}

		//! Return the configurable description
		auto & GetInfo() const	{ return m_sInfo;	}

		//! Return the configurable blurb
		auto & GetBlurb() const { return m_sBlurb;	}

		/// <summary>
		/// Export the configurable into a file
		/// </summary>
		/// <param name="file_path">File path</param>
		virtual void ExportFile(const std::filesystem::path & file_path) const { 
			cv::FileStorage fs(file_path.generic_string(), cv::FileStorage::WRITE);
			assert(fs.isOpened());
			write(fs);
		}

		/// <summary>
		/// Import a configurable from a file
		/// </summary>
		/// <param name="file_path">File path</param>
		virtual void ImportFile(const std::filesystem::path & file_path) { 
			cv::FileStorage fs(file_path.generic_string(), cv::FileStorage::READ);
			assert(fs.isOpened());
			read(fs.root());
		}

		/// <summary>
		/// String serialization function. Virtual function.
		/// </summary>
		/// <returns> the string value of the serialization process</returns>
		virtual std::string GetStringValue()	const = 0;

		/// <summary>
		/// String De-serialization function. Virtual function.
		/// </summary>
		/// <returns></returns>
		virtual std::wstring GetWStringValue()	const = 0;

		/// <summary>
		/// String De-serialization function. Virtual function.
		/// </summary>
		/// <param name="svalue">string serialized value</param>
		virtual void SetStringValue(const std::string & svalue) = 0;

		/// <summary>
		/// WString De-serialization function. Virtual function.
		/// </summary>
		virtual void SetWStringValue(const std::wstring & svalue) = 0;

		/// <summary>
		/// Write the configurable using cv::FileStorage format
		/// </summary>
		/// <param name="fs">file</param>
		virtual void write(cv::FileStorage& fs) const {
			assert(0 && "not implemented (virtual function)");
		}

		/// <summary>
		/// Write the configurable using cv::FileStorage format
		/// </summary>
		/// <param name="fs">file</param>
		virtual void read(const cv::FileNode& fs) {
			assert(0 && "not implemented (virtual function)");
		}

		/// <summary>
		/// Copy this configurable state into another configurable 
		/// </summary>
		/// <param name="IConfigurable">Configurable destination</param>
		virtual void CopyTo(IConfigurable& configurable) const {
			assert(0 && "not implemented (virtual function)");
		}

		/// <summary>
		/// Copy the state of configurable into this configurable
		/// </summary>
		/// <param name="configurable">Configurable source</param>
		void CopyFrom(const IConfigurable& configurable) {
			configurable.CopyTo(*this);
		}

		/// <summary>
		/// Affectation operator. Call CopyTo
		/// </summary>
		/// <param name="configurable">configurable source</param>
		/// <returns>Configurable reference</returns>
		virtual IConfigurable& operator=(const IConfigurable& configurable) {
			configurable.CopyTo(*this);
			return *this;
		}

		/// <summary>
		/// Get the type index of the configurable
		/// </summary>
		/// <returns></returns>
		std::type_index TypeIndex() const { return std::type_index(typeid(*this)); }

		/// <summary>
		/// Reset the configurable. Virtual function
		/// </summary>
		virtual void Reset() = 0;
	};

	//Configurable including a data
	template<typename TData>
	class TDataConfigurable : public IConfigurable
	{
	public:

		using typename IConfigurable::key_t;
		using typename IConfigurable::string_t;
		using data_t = typename TData;

		data_t m_data = {};
		std::optional<data_t> m_default = {};

		TDataConfigurable(const key_t & key, const string_t & info, const string_t & blurb) :
			IConfigurable(key, info, blurb)
		{		}

		template<class Tdata_t>
		TDataConfigurable(const key_t & key, const string_t & info, const string_t & blurb, Tdata_t&& default_data) :
			IConfigurable(key, info, blurb),
			m_data(std::forward<Tdata_t>(default_data)),
			m_default(m_data)
		{		}

		template<class Tdata_t>
		TDataConfigurable(const key_t& key, const string_t& info, Tdata_t&& default_data) :
			TDataConfigurable(key, info, {}, std::forward<Tdata_t>(default_data))
		{		}

		template<class Tdata_t>
		TDataConfigurable(const key_t & key, Tdata_t&& default_data) :
			TDataConfigurable(key, {}, {}, std::forward<Tdata_t>(default_data))
		{		}

		TDataConfigurable(const key_t & key) :
			TDataConfigurable(key, {}, {})
		{		}

		//Getters/setters

		virtual void Set(const data_t& data)		{ m_data = data; }
		virtual void Set(data_t&& data)				{ m_data = std::move(data); }
		auto & Value()			const				{ return m_data; }
		auto & Value()								{ return m_data; }
		void SetDefault()							{ m_data = m_default.value_or(data_t{}); }
		auto DefaultValue()		const				{ return m_default.value_or(data_t{});   }
		const auto & Default()	const				{ return m_default; }
		auto & Default()							{ return m_default; }
		void Reset()			override     		{ SetDefault(); }


		//Operators

		operator data_t&()						{ return m_data; }
		operator const data_t&()	const		{ return m_data; }
		auto& operator()()						{ return m_data; }
		auto& operator()()			const		{ return m_data; }
		auto operator->()			const		{ return &m_data; }
		auto operator->()						{ return &m_data; }
		auto & operator*()			const		{ return m_data; }
		auto & operator*()						{ return m_data; }

		template<typename ...Args>
		TDataConfigurable & operator=(Args&&... args) {
			Set(std::forward<Args>(args)...); return *this; 
		}

		TDataConfigurable & operator=(const TDataConfigurable & data) {
			Set(data.Value()); return *this;
		}

		template<typename ...Args>
		bool operator==(Args&&... args) const { 
			return Value() == data_t(std::forward<Args>(args)...);
		}

		template<typename T>
		bool operator==(T&& data) const { 
			return Value() == static_cast<data_t>(std::forward<T>(data));
		}

		std::string GetStringValue() const override 
		{ 
			if constexpr (std::is_same_v<std::string, data_t>){
				return m_data;
			}
			else{
				return serialization::to_string(m_data);
			}
		}

		std::wstring GetWStringValue() const override 
		{
			if constexpr (std::is_same_v<std::wstring, data_t>){
				return m_data;
			}
			else{
				return wserialization::to_wstring(m_data);
			}
		}

		void SetStringValue(const std::string & svalue) override
		{
			if constexpr (
				std::is_same_v<std::string, data_t> ||
				std::is_same_v<std::filesystem::path, data_t>
				)
			{
				m_data = svalue;
			}
			else if constexpr (std::is_same_v<std::wstring, data_t>)
			{
				assert(0 && "Instead, use SetWStringValue function");
			}
			else
			{
				unserialization::to_data(svalue, m_data);
			}
		}

		void SetWStringValue(const std::wstring & wsvalue) override
		{
			if constexpr (
				std::is_same_v<std::wstring, data_t> ||
				std::is_same_v<std::filesystem::path, data_t>
				)
			{
				m_data = wsvalue;
			}
			else if constexpr (std::is_same_v<std::string, data_t>)
			{
				assert(0 && "Instead, use SetStringValue function");
			}
			else
			{
				unserialization::to_data(wsvalue, m_data);
			}
		}


		void write(cv::FileStorage& fs) const override {
			fs << GetKey() << m_data;
		}

		void read(const cv::FileNode& fs) override {
			auto keyNode = fs[GetKey()];
			if(!keyNode.empty())
				keyNode >> m_data;
		}

	};

	//Configurable including a numerical data with a possible [min, max] range
	template<typename TNumeric>
	class CNumericConfigurable : public TDataConfigurable<TNumeric>
	{
	
	public:
		using numeric_t = typename TNumeric;
		using typename TDataConfigurable<numeric_t>::key_t;
		using typename TDataConfigurable<numeric_t>::string_t;
		using typename TDataConfigurable<numeric_t>::data_t;
		
		static constexpr numeric_t MIN_VALUE = std::numeric_limits<numeric_t>::lowest();
		static constexpr numeric_t MAX_VALUE = std::numeric_limits<numeric_t>::max();

		CNumericConfigurable(const key_t & key, const string_t & info, const string_t & blurb) :
			TDataConfigurable<numeric_t>(key, info, blurb)
		{		}

		CNumericConfigurable(const key_t & key, const string_t & info, const string_t & blurb, data_t min, data_t max) :
			TDataConfigurable<numeric_t>(key, info, blurb),
			m_minRange(min), m_maxRange(max)
		{		}

		CNumericConfigurable(const key_t & key, const string_t & info, const string_t & blurb, data_t default_data) :
			TDataConfigurable<numeric_t>(key, info, blurb, default_data)
		{		}

		CNumericConfigurable(const key_t & key, const string_t & info, const string_t & blurb, data_t min, data_t max, data_t default_data) :
			TDataConfigurable<numeric_t>(key, info, blurb, default_data),
			m_minRange(min), m_maxRange(max)
		{		}

		CNumericConfigurable(const key_t & key, data_t default_data) :
			CNumericConfigurable(key, {}, {}, default_data) 
		{		};

		CNumericConfigurable(const key_t& key, const string_t& info, data_t default_data) :
			CNumericConfigurable(key, info, {}, default_data)
		{		};

		CNumericConfigurable(const key_t & key) :
			CNumericConfigurable(key, {}, {}, {}) 
		{		};

		template<typename ...Args>
		decltype(auto) operator=(Args&&... args) { TDataConfigurable<numeric_t>::operator=(std::forward<Args>(args)...); return *this; }

	private:
		data_t m_minRange = MIN_VALUE;
		data_t	m_maxRange = MAX_VALUE;
	};

	using CBoolConfigurable = TDataConfigurable<bool>;
	using CDoubleConfigurable = CNumericConfigurable<double>;
	using CFloatConfigurable = CNumericConfigurable<float>;
	using CIntConfigurable = CNumericConfigurable<int>;
	using CStringConfigurable = TDataConfigurable<std::string>;
	using CPathConfigurable = TDataConfigurable<std::filesystem::path>;
	using CScalarConfigurable = TDataConfigurable<cv::Scalar>;

}

inline static
cv::FileStorage& operator<<(cv::FileStorage & fs, const std::wstring & wstr)
{
	assert(false && "NOT implemented correctly");

	/*auto buffer_size = sizeof(wchar_t) * wstr.size();
	std::string str(buffer_size, (char)0);
	std::wcstombs(str.data(), wstr.data(), buffer_size);
	fs << str;*/

	return fs;
}

inline static
const cv::FileNode& operator>>(const cv::FileNode & fs, std::wstring & wstr)
{
	std::string str;
	fs >> str;
	wstr.assign(std::begin(str), std::end(str));
	return fs;
}

inline static
cv::FileStorage& operator<<(cv::FileStorage & fs, const std::filesystem::path & path)
{
	fs << path.generic_string();
	return fs;
}

inline static
const cv::FileNode& operator>>(const cv::FileNode & fs, std::filesystem::path & path)
{
	std::string strpath;
	fs >> strpath;
	path = strpath;
	return fs;
}

inline static
std::ostream& operator<<(std::ostream& os, const bhd::IConfigurable & configurable)
{
	os << configurable.GetStringValue();
	return os;
}

inline static
std::wostream& operator<<(std::wostream& wos, const bhd::IConfigurable & configurable)
{
	wos << configurable.GetWStringValue();
	return wos;
}

//IConfigurable

inline static
cv::FileStorage& operator<<(cv::FileStorage & fs, const bhd::IConfigurable & configurable)
{
	configurable.write(fs);
	return fs;
}

inline static
const cv::FileNode& operator>>(const cv::FileNode & fn, bhd::IConfigurable & configurable)
{
	configurable.read(fn);
	return fn;
}

inline static
const cv::FileStorage& operator>>(const cv::FileStorage & fs, bhd::IConfigurable & configurable)
{
	configurable.read(fs.root());
	return fs;
}

//TDataConfigurable

template <class T>
cv::FileStorage& operator<<(cv::FileStorage & fs, const bhd::TDataConfigurable<T> & configurable)
{
	configurable.write(fs);
	return fs;
}

template <class T>
const cv::FileNode& operator>>(const cv::FileNode & fn, bhd::TDataConfigurable<T> & configurable)
{
	configurable.read(fn);
	return fn;
}

template <class T>
const cv::FileStorage& operator>>(const cv::FileStorage & fs, bhd::TDataConfigurable<T> & configurable)
{
	configurable.read(fs.root());
	return fs;
}

//CNumericConfigurable

template <class T>
cv::FileStorage& operator<<(cv::FileStorage & fs, const bhd::CNumericConfigurable<T> & configurable)
{
	configurable.write(fs);
	return fs;
}

template <class T>
const cv::FileNode& operator>>(const cv::FileNode & fn, bhd::CNumericConfigurable<T> & configurable)
{
	configurable.read(fn);
	return fn;
}

template <class T>
const cv::FileStorage& operator>>(const cv::FileStorage & fs, bhd::CNumericConfigurable<T> & configurable)
{
	configurable.read(fs.root());
	return fs;
}

