#pragma once


#include "Module.h"
#include "Logger.h"
#include "LoggerImg.h"
#include "Configurable.h"





namespace mzd
{
	/// <summary>
	/// Embryo logger as instance of a logger and a image logger
	/// </summary>
	struct ILoggerProc
	{

		using CLogger = logging::CLogger;
		std::shared_ptr<CLogger> m_pLogger;

		using CLoggerImg = logging::CLoggerImg;
		std::shared_ptr<CLoggerImg> m_pLoggerImg;

		ILoggerProc(const ILoggerProc &) = default;
		ILoggerProc(ILoggerProc &&) = default;

		ILoggerProc& operator=(const ILoggerProc &) = default;
		ILoggerProc& operator=(ILoggerProc &&) = default;

		template<typename TLogger, typename TLoggerImg>
		ILoggerProc(TLogger&& logger, TLoggerImg&& loggerimg) :
			m_pLogger(std::forward<TLogger>(logger)),
			m_pLoggerImg(std::forward<TLoggerImg>(loggerimg))
		{	}

		template<typename ...Args>
		static auto MakeLogger(Args&&... args) {
			return std::make_shared<CLogger>(std::forward<Args>(args)...);
		}

		template<typename ...Args>
		static auto MakeLoggerImg(Args&&... args) {
			return std::make_shared<CLoggerImg>(std::forward<Args>(args)...);
		}

		/// <summary>
		/// Log a sequence of arguments with the info prefix
		/// </summary>
		/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
		template<typename ...Args>
		void LogInfo(Args&&... args) const
		{
			if constexpr (sizeof...(Args) > 0) {
				assert(m_pLogger != nullptr);
				m_pLogger->LogInfo(std::forward<Args>(args)...);
			}
		};

		/// <summary>
		/// Log a sequence of arguments with the warning prefix
		/// </summary>
		/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
		template<typename ...Args>
		void LogWarning(Args&&... args) const
		{
			if constexpr (sizeof...(Args) > 0) {
				assert(m_pLogger != nullptr);
				m_pLogger->LogWarning(std::forward<Args>(args)...);
			}
		};

		/// <summary>
		/// Log a sequence of arguments with the error prefix
		/// </summary>
		/// <param name="...args">Sequence of arguments supported by the std::ostream insertion operator &lt;&lt; </param>
		template<typename ...Args>
		void LogError(Args&&... args) const
		{
			if constexpr (sizeof...(Args) > 0) {
				assert(m_pLogger != nullptr);
				m_pLogger->LogError(std::forward<Args>(args)...);
			}
		};


		void SetVerboseImage(bool flag) {
			assert(m_pLoggerImg != nullptr && "Init a image logger first");
			m_pLoggerImg->m_flagVerbose = flag;
		}

		void SetLogDirectory(const std::filesystem::path &directory) {
			assert(m_pLoggerImg != nullptr && "Init a image logger first");
			m_pLoggerImg->m_directory = directory;
		}

		void SetLevelVerbose(int level) {
			assert(m_pLogger != nullptr && "Init a logger first");
			m_pLogger->SetLevelVerbose(level);
		}

#ifdef _DEBUG
		auto GetLoggerPtr() const { 
			return reinterpret_cast<std::uintptr_t>(m_pLogger.get());
		}
#endif

	};


	/// <summary>
	/// Module with logger as instance of a logger and a image logger
	/// </summary>
	class CImgProcModule : public IModule, public ILoggerProc
	{
		using typename IModule::key_t;
		using typename IModule::string_t;

	public:

		template<typename ...Args>
		CImgProcModule(ILoggerProc && logger, Args&&... args) :
			IModule(std::forward<Args>(args)...),
			ILoggerProc(std::move(logger))
		{	}

		template<typename ...Args>
		CImgProcModule(const ILoggerProc & logger, Args&&... args) :
			IModule(std::forward<Args>(args)...),
			ILoggerProc(logger)
		{	}

		CImgProcModule(const ILoggerProc & logger) :
			ILoggerProc(logger)
		{	}

		CImgProcModule(const CImgProcModule & imgProcModule) :
			CImgProcModule(static_cast<const ILoggerProc&>(imgProcModule))
		{	}
			
		virtual ~CImgProcModule() 
		{	}

	};

}