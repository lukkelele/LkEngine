#pragma once

#include "LkEngine/Core/CoreMacros.h"

#include <stdint.h>
#include <cstring>
#include <map>

/* Ignore warnings raised inside external headers. */
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/fmt.h>
#pragma warning(pop)

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "LkEngine/Core/Assert.h"


namespace LkEngine {

	/** ELogLevel */
	enum class ELogLevel : int
	{
		Trace, 
		Debug,
		Info, 
		Warn, 
		Error, 
		Fatal
	};

	enum class ELoggerType : int
	{ 
		Core = 0, 
		Client,
		Asset
	};

	struct FLoggerInitArguments
	{
		std::string LogFilename = "LkEngine_Core.log";
		std::string CoreLoggerName = "CORE";
		std::string ClientLoggerName = "CLIENT";
	};

	class LLog 
	{
	public:
		/** FTagDetails */
		struct FTagDetails
		{
			bool Enabled = true;
			ELogLevel Filter = ELogLevel::Debug;
		};

		static void Initialize(std::string_view LogfileName = "Logger.log", 
							   std::string_view CoreLoggerName = "CORE", 
							   std::string_view ClientLoggerName = "CLIENT");

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		static std::shared_ptr<spdlog::logger>& GetAssetLogger() { return m_AssetLogger; }

		FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger(const ELoggerType LoggerType)
		{
			switch (LoggerType)
			{
				case ELoggerType::Core:   return GetCoreLogger();
				case ELoggerType::Client: return GetClientLogger();
				case ELoggerType::Asset:  return GetClientLogger();
				default: break;
			}

			LK_CORE_ASSERT(false, "Invalid logger type");
			return GetCoreLogger();
		}

		template<typename... TArgs>
		static void PrintMessage(ELoggerType LoggerType, ELogLevel level, 
								 std::string_view Tag, TArgs&&... Args);

		template<typename... TArgs>
		static void PrintAssertMessage(ELoggerType LoggerType, std::string_view Prefix , TArgs&&... Args);

		static const char* LevelToString(ELogLevel Level)
		{
			switch (Level)
			{
				case ELogLevel::Trace: return "Trace";
				case ELogLevel::Info:  return "Info";
				case ELogLevel::Warn:  return "Warn";
				case ELogLevel::Error: return "Error";
				case ELogLevel::Fatal: return "Fatal";
			}

			LK_CORE_ASSERT(false, "Unknown log level: {}", static_cast<int>(Level));
			return "";
		}

		static ELogLevel LevelFromString(std::string_view InString)
		{
			if (InString == "Trace")   return ELogLevel::Trace;
			if (InString == "Info")    return ELogLevel::Info;
			if (InString == "Warn")    return ELogLevel::Warn;
			if (InString == "Error")   return ELogLevel::Error;
			if (InString == "Fatal")   return ELogLevel::Fatal;

			LK_CORE_ASSERT(false, "Failed log level conversion for \"{}\"", InString);
			return ELogLevel::Info;
		}

	private:
		inline static std::shared_ptr<spdlog::logger> m_CoreLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> m_ClientLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> m_AssetLogger = nullptr;

		inline static std::map<std::string, FTagDetails> m_EnabledTags;
	};

}

/* Core Logging. */
#define LK_CORE_TRACE(...)              ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, "", __VA_ARGS__)
#define LK_CORE_DEBUG(...)              ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, "", __VA_ARGS__)
#define LK_CORE_INFO(...)               ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info, "", __VA_ARGS__)
#define LK_CORE_WARN(...)               ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warn, "", __VA_ARGS__)
#define LK_CORE_ERROR(...)              ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, "", __VA_ARGS__)
#define LK_CORE_FATAL(...)              ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, "", __VA_ARGS__)

#define LK_CORE_TRACE_TAG(_TAG, ...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, _TAG, __VA_ARGS__)
#define LK_CORE_DEBUG_TAG(_TAG, ...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, _TAG, __VA_ARGS__)
#define LK_CORE_INFO_TAG(_TAG, ...)     ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info, _TAG, __VA_ARGS__)
#define LK_CORE_WARN_TAG(_TAG, ...)     ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warn, _TAG, __VA_ARGS__)
#define LK_CORE_ERROR_TAG(_TAG, ...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, _TAG, __VA_ARGS__)
#define LK_CORE_FATAL_TAG(_TAG, ...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, _TAG, __VA_ARGS__)

/* Client Logging. */
#define LK_TRACE(...)                   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, "", __VA_ARGS__)
#define LK_INFO(...)                    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, "", __VA_ARGS__)
#define LK_WARN(...)                    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warn, "", __VA_ARGS__)
#define LK_ERROR(...)                   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warn, "", __VA_ARGS__)
#define LK_FATAL(...)                   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, "", __VA_ARGS__)

/* FIXME: In the future */
/* Asset Logging. */
#define LOG_ASSET(...) 			        ::LkEngine::LLog::GetAssetLogger()->debug(__VA_ARGS__)
#define LK_ASSET_LOG(...) 			    ::LkEngine::LLog::GetAssetLogger()->debug(__VA_ARGS__)


namespace LkEngine {

	template<typename... TArgs>
	void LLog::PrintMessage(ELoggerType LoggerType, ELogLevel Level, 
							std::string_view Tag, TArgs&&... Args)
	{
		FTagDetails& TagDetails = m_EnabledTags[std::string(Tag)];
		if (TagDetails.Enabled && TagDetails.Filter <= Level)
		{
			std::shared_ptr<spdlog::logger>& Logger = (LoggerType == ELoggerType::Core) ? GetCoreLogger() : GetClientLogger();
			std::string LogString = Tag.empty() ? "{0}{1}" : "[{0}] {1}";
			switch (Level)
			{
				case ELogLevel::Trace:
					Logger->trace(LogString, Tag, fmt::format(std::forward<TArgs>(Args)...));
					break;

				case ELogLevel::Debug:
					Logger->debug(LogString, Tag, fmt::format(std::forward<TArgs>(Args)...));
					break;

				case ELogLevel::Info:
					Logger->info(LogString, Tag, fmt::format(std::forward<TArgs>(Args)...));
					break;

				case ELogLevel::Warn:
					Logger->warn(LogString, Tag, fmt::format(std::forward<TArgs>(Args)...));
					break;

				case ELogLevel::Error:
					Logger->error(LogString, Tag, fmt::format(std::forward<TArgs>(Args)...));
					break;

				case ELogLevel::Fatal:
					Logger->critical(LogString, Tag, fmt::format(std::forward<TArgs>(Args)...));
					break;
			}
		}
	}

	template<typename ...TArgs>
	void LLog::PrintAssertMessage(ELoggerType LoggerType, std::string_view Prefix, TArgs&&... Args)
	{
		LLog::GetLogger(LoggerType)->error("{0}: {1}", Prefix, fmt::format(std::forward<TArgs>(Args)...));
	}

	template<>
	inline void LLog::PrintAssertMessage(ELoggerType LoggerType, std::string_view Prefix)
	{
		LLog::GetLogger(LoggerType)->error("{0}", Prefix);
	}

}
