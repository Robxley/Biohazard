#pragma once

#include "BHM_Utils.h"
#include "BHM_Terminal.h"

#include <type_traits>
#include <mutex>
#include <functional>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <opencv2\opencv.hpp>

namespace bhd::logging
{

	/// <summary>
	/// Some affix/particles add to the message log
	/// </summary>
	enum class LOG_FLAG
	{
		INFO = 0,
		WARNING,
		ERROR,
		N_COUNT
	};

	/// <summary>
	/// Generic logger function;
	/// </summary>

	using TLogFlag = int;
	using TLogMsg = const std::string&;
	using TLogFunc = std::function<void(TLogFlag, TLogMsg)>;

	namespace core
	{

		struct CLogUnit
		{
			CLogUnit() = default;

			template <typename Func>
			CLogUnit(Func&& func) :
				m_lambdaLogFunc(std::forward<Func>(func))
			{	}

			CLogUnit(const CLogUnit& unit) :
				m_lambdaLogFunc(unit.m_lambdaLogFunc)
			{	}
			CLogUnit(CLogUnit&& unit)
			{
				auto lg = std::lock_guard(unit.m_mutexLog);
				m_lambdaLogFunc = std::move(unit.m_lambdaLogFunc);
			}


			/// <summary>
			/// Generic logger function. Each time a log message is recorded, this function is called with the message as input.
			/// </summary>
			TLogFunc m_lambdaLogFunc;  /*= [](std::string&& msg) { std::cout << msg; }*/
			
			/// <summary>
			/// Logger mutex
			/// </summary>
			mutable std::mutex m_mutexLog;

			template <typename T>
			void SetLogFunc(T&& func) {
				auto lg = std::lock_guard(m_mutexLog);
				m_lambdaLogFunc = std::forward<T>(func);
			}

			template <typename ...Args>
			void SetLogFuncList(Args&&... args)
			{
				SetLogFunc(
					[args = std::make_tuple(std::forward<Args>(args) ...)](TLogMsg msg) mutable
				{
					std::apply([&](auto&& ... args) {
						(std::invoke(std::forward<decltype(args)>(args), msg), ...);
					}, std::move(args));
				});
			}

			/// <summary>
			/// Main log function used by all other log functions
			/// Log a sequence of arguments. All arguments have to be supported by the std::stringstream insertion operator (<<)
			/// </summary>
			/// <param name="flag">Logger flag/level</param>
			/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator (<<) </param>
			template<typename ...Args> void Log(TLogFlag&& flag, Args&&... args) const
			{
				if constexpr (sizeof...(args) > 0)
				{
					assert(m_lambdaLogFunc && "Invalid lambda Logger");
					auto msg = concat_to_string(std::forward<Args>(args)...);
					auto lg = std::lock_guard(m_mutexLog);
					m_lambdaLogFunc(std::forward<decltype(flag)>(flag), msg);
				}
			}

			/// <summary>
			/// Log a sequence of arguments with the info flag
			/// </summary>
			/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
			template<typename ...Args>
			void LogInfo(Args&&... args) const {
				Log(static_cast<TLogFlag>(LOG_FLAG::INFO), std::forward<Args>(args)...);
			}


			/// <summary>
			/// Log a sequence of arguments with the error prefix
			/// </summary>
			/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
			template<typename ...Args>
			void LogError(Args&&... args) const {
				Log(static_cast<TLogFlag>(LOG_FLAG::ERROR), std::forward<Args>(args)...);
			}

			/// <summary>
			/// Log a sequence of arguments with the error prefix
			/// </summary>
			/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
			template<typename ...Args>
			void LogWarning(Args&&... args) const {
				Log(static_cast<TLogFlag>(LOG_FLAG::WARNING), std::forward<Args>(args)...);
			}


		};

		namespace log_unit_stdout
		{
			constexpr auto ANSI_AFFIX = std::array{
				color::TEXT,
				color::WARNING,
				color::ERROR,
				color::ENDL
			};

			/// <summary>
			/// Standard output logger (using std::cout)
			/// </summary>
			/// <returns>Lambda function</returns>
			inline auto standard_output_func() {
				return TLogFunc([&](auto&& flag, auto&& msg) {
					if (static_cast<std::size_t>(flag) < ANSI_AFFIX.size())
						terminal(ANSI_AFFIX[static_cast<int>(FWD(flag))], FWD(msg));
					else
						terminal(FWD(msg));
				});
			}

			/// <summary>
			/// Get the log unit instance (singleton) of standard output
			/// </summary>
			/// <returns></returns>
			inline auto& instance() {
				static CLogUnit unit = { standard_output_func() };
				return unit;
			}

		}

		namespace log_unit_file
		{

			constexpr auto FILE_AFFIX = std::array{
				R"(    - INFO   : )",
				R"(/!\ - WARNING: )",
				R"(/X\ - ERROR  : )",
				"\n"
			};

			/// <summary>
			/// File output logger
			/// </summary>
			/// <param name="path">The file path</param>
			/// <returns>Lambda function</returns>
			inline auto file_output_func(const std::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::out) {
				assert(!path.empty() && path.has_filename() && "Invalid file name");
				auto parent_path = path.parent_path();
				if (!parent_path.empty())
				{
					std::filesystem::create_directories(parent_path);
					assert(std::filesystem::exists(path.parent_path()));
				}
				return TLogFunc([file = std::make_shared<std::ofstream>(path, mode)](auto && flag, auto&& msg) mutable {
					assert(file->is_open());
					if (static_cast<std::size_t>(flag) < FILE_AFFIX.size())
						*file << FILE_AFFIX[static_cast<int>(FWD(flag))] <<  FWD(msg);
					else
						*file << FWD(msg) << std::flush;
				});
			}

			template<typename ...Args>
			inline auto make(Args&&... args) {
				return CLogUnit{ file_output_func(std::forward<Args>(args)...) };
			}
		}

	} //End namespace core

	/// <summary>
	/// Recorder object to record what is needed.
	/// </summary>
	class CLogger
	{
	private:
		using WUnit = object_wrapper<core::CLogUnit>;
		mutable std::vector<WUnit> m_vLogUnits;
		int m_levelVerbose =static_cast<int>(LOG_FLAG::INFO);

	public:
		CLogger() :
			m_vLogUnits{ core::log_unit_stdout::instance() }
		{	};

		CLogger(const CLogger&) = default;
		CLogger(CLogger&&) = default;

		CLogger(bool stdoutput, const std::filesystem::path& logfile, std::ios_base::openmode mode = std::ios_base::out) {
			SetUnits(stdoutput, logfile, mode);
		}

		static CLogger& Singleton(bool stdoutput = true, const std::filesystem::path& logfile = {}, std::ios_base::openmode mode = std::ios_base::out)
		{
			static CLogger instance(stdoutput, logfile, mode);
			return instance;
		}

		virtual ~CLogger() = default;

		/// <summary>
		/// Set the logger unit. Not thread safe.
		/// </summary>
		/// <param name="stdoutput">Enable/Disable the standard output (std::cout) as logger</param>
		/// <param name="logfile">File path where to write the logger message. Can be empty.</param>
		/// <param name="mode">File mode</param>
		void SetUnits(bool stdoutput, const std::filesystem::path& logfile, std::ios_base::openmode mode = std::ios_base::out)
		{
			if (stdoutput && !logfile.empty())
				m_vLogUnits = { core::log_unit_stdout::instance() , std::move(core::log_unit_file::make(logfile, mode)) };
			else if (!logfile.empty())
				m_vLogUnits = { std::move(core::log_unit_file::make(logfile, mode)) };
			else if(stdoutput)
				m_vLogUnits = { core::log_unit_stdout::instance() };
		}

		/// <summary>
		/// Set the logger unit. Not thread safe.
		/// </summary>
		/// <param name="stdoutput">Enable/Disable the standard output (std::cout) as logger</param>
		/// <param name="logfile">File path where to write the logger message. Can be empty.</param>
		/// <param name="mode">File mode</param>
		void AddUnits(bool stdoutput, const std::filesystem::path& logfile, std::ios_base::openmode mode = std::ios_base::out)
		{
			if (!logfile.empty())
				m_vLogUnits.emplace_back(std::move(core::log_unit_file::make(logfile, mode)));
			if (stdoutput)
				m_vLogUnits.emplace_back(core::log_unit_stdout::instance());
		}

		/// <summary>
		/// Log a sequence of arguments with the error prefix
		/// </summary>
		/// <param name="affixArray">Affix used with logged message. </param>
		/// <param name="func">Custom logging function call by the Logger</param>
		void AddUnit(const TLogFunc& func)
		{
			m_vLogUnits.emplace_back(std::move(core::CLogUnit{ func }));
		}

		template<typename TLogUnit>
		void AddUnit(TLogUnit && unit)
		{
			m_vLogUnits.emplace_back(FWD(unit));
		}


		/// <summary>
		/// Enable/disable the logging on the standard output (used std::cout)
		/// </summary>
		/// <param name="value">Flag true/false</param>
		void SetLevelVerbose(int value) {
			m_levelVerbose = value;
		}

		/// <summary>
		/// Log a sequence of arguments with the info prefix
		/// </summary>
		/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
		template<typename ...Args>
		void LogInfo(Args&&... args) const
		{
			if constexpr (sizeof...(Args) > 0)
			{
				if (m_levelVerbose > static_cast<int>(LOG_FLAG::INFO))
					return;

				assert(m_vLogUnits.size() > 0);
				auto msg = concat_to_string(std::forward<Args>(args)...);
				std::for_each(m_vLogUnits.begin(), m_vLogUnits.end() - 1, [&](WUnit& unit) {unit->LogInfo(msg); });
				m_vLogUnits.back()->LogInfo(std::move(msg));
			}
		}

		/// <summary>
		/// Log a sequence of arguments with the warning prefix
		/// </summary>
		/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
		template<typename ...Args>
		void LogWarning(Args&&... args) const {
			if constexpr (sizeof...(Args) > 0)
			{
				if (m_levelVerbose > static_cast<int>(LOG_FLAG::WARNING))
					return;

				assert(m_vLogUnits.size() > 0);
				auto msg = concat_to_string(std::forward<Args>(args)...);
				std::for_each(m_vLogUnits.begin(), m_vLogUnits.end() - 1, [&](WUnit& unit) {unit->LogWarning(msg); });
				m_vLogUnits.back()->LogWarning(std::move(msg));
			}
		}


		/// <summary>
		/// Log a sequence of arguments with the error prefix
		/// </summary>
		/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
		template<typename ...Args>
		void LogError(Args&&... args) const {
			if constexpr (sizeof...(Args) > 0)
			{
				if (m_levelVerbose > static_cast<int>(LOG_FLAG::ERROR))
					return;

				assert(m_vLogUnits.size() > 0);
				auto msg = concat_to_string(std::forward<Args>(args)...);
				std::for_each(m_vLogUnits.begin(), m_vLogUnits.end() - 1, [&](WUnit& unit) {unit->LogError(msg); });
				m_vLogUnits.back()->LogError(std::move(msg));
			}
		}

	};



	/// <summary>
	/// Interface functions to access to the CLogger class
	/// </summary>

	/// <summary>
	/// Get a single instance of a logger type. Thread safe (using static variable initialization)
	/// </summary>
	/// <param name="...args">Parameter used with the first call of instance to initialize the logger parameter</param>
	/// <returns>Singleton logger instance</returns>
	template<typename ...Args> auto& Logger(Args&&... args) {
		return CLogger::Singleton(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Configuration the default logger (singleton logger).
	/// Not thread safe.
	/// </summary>
	/// <param name="stdoutput">Enable/Disable the standard output (</param>
	/// <param name="logfile"></param>
	/// <param name="mode"></param>
	inline void Setup(bool stdoutput, const std::filesystem::path& logfile, std::ios_base::openmode mode) {
		Logger().SetUnits(stdoutput, logfile, mode);
	}

	/// <summary>
	/// Add a custom logger unit to the common logger.
	/// Not thread safe.
	/// </summary>
	/// <param name="affixArray">Affix used with logged message. </param>
	/// <param name="func">Custom logging function call by the Logger</param>
	inline void AddLogUnit(const TLogFunc& func){
		Logger().AddUnit(func);
	}

	/// <summary>
	/// Log a sequence of arguments with the error prefix.
	/// Thread safe.
	/// </summary>
	/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
	template<typename ...Args>
	void Error(Args&&... args) {
		Logger().LogError(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Log a sequence of arguments with the error prefix
	/// Thread safe.
	/// </summary>
	/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
	template<typename ...Args>
	void Warning(Args&&... args) {
		Logger().LogWarning(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Log a sequence of arguments with the error prefix.
	/// Thread Safe.
	/// </summary>
	/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
	template<typename ...Args>
	void Info(Args&&... args) {
		Logger().LogInfo(std::forward<Args>(args)...);
	}


}