/******************************************************************
 * LLog
 *
 * Logger class and logging implementation for the entire engine. 
 *******************************************************************/
#pragma once

#include "LkEngine/Core/CoreMacros.h"

#include <stdint.h>
#include <cstring>
#include <map>
#include <filesystem>

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

	static std::filesystem::path LogDirectory{ "Logs/" };

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
		UI,
		Asset,
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
		/** 
		 * FTagDetails 
		 */
		struct FTagDetails
		{
			bool Enabled = true;
			ELogLevel Filter = ELogLevel::Debug;

			FTagDetails() = default;
			FTagDetails(const ELogLevel InFilter)
				: Filter(InFilter) 
			{
			}
		};

		LLog();
		~LLog();

		static LLog& Instance();

		static void Initialize(std::string_view LogfileName = "Logger.log", 
							   std::string_view CoreLoggerName = "CORE", 
							   std::string_view ClientLoggerName = "CLIENT");

		FORCEINLINE static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return CoreLogger; }
		FORCEINLINE static std::shared_ptr<spdlog::logger>& GetClientLogger() { return ClientLogger; }
		FORCEINLINE static std::shared_ptr<spdlog::logger>& GetUILogger() { return UILogger; }
		FORCEINLINE static std::shared_ptr<spdlog::logger>& GetAssetLogger() { return AssetLogger; }

		FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger(const ELoggerType LoggerType)
		{
			switch (LoggerType)
			{
				case ELoggerType::Core:   return GetCoreLogger();
				case ELoggerType::Client: return GetClientLogger();
				case ELoggerType::UI:     return GetUILogger();
				case ELoggerType::Asset:  return GetAssetLogger();
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

		FORCEINLINE static const char* LevelToString(ELogLevel Level)
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

		/* TODO: Convert InString to lowercase before check takes place. */
		FORCEINLINE static ELogLevel LevelFromString(std::string_view InString)
		{
			if (InString == "Trace")   return ELogLevel::Trace;
			if (InString == "Info")    return ELogLevel::Info;
			if (InString == "Warn")    return ELogLevel::Warn;
			if (InString == "Error")   return ELogLevel::Error;
			if (InString == "Fatal")   return ELogLevel::Fatal;

			LK_CORE_ASSERT(false, "Failed log level conversion for \"{}\"", InString);
			return ELogLevel::Info;
		}

		/**
		 * @brief Convert a ELogLevel to spdlog::level.
		 */
		FORCEINLINE static constexpr spdlog::level::level_enum ToSpdlogLevel(const ELogLevel Level)
		{
			switch (Level)
			{
				case ELogLevel::Trace:   return spdlog::level::trace;
				case ELogLevel::Info:    return spdlog::level::info;
				case ELogLevel::Warn:    return spdlog::level::warn;
				case ELogLevel::Error:   return spdlog::level::err;
				case ELogLevel::Fatal:   return spdlog::level::critical;
			}

			throw "Invalid log level";
			return spdlog::level::info;
		}

		/**
		 * @brief Get the name of a logger by using the logger type.
		 */
		FORCEINLINE static std::string_view GetLoggerName(const ELoggerType LoggerType)
		{
			switch (LoggerType)
			{
				case ELoggerType::Core:   return GetCoreLogger()->name();
				case ELoggerType::Client: return GetClientLogger()->name();
				case ELoggerType::UI:     return GetUILogger()->name();
				case ELoggerType::Asset:  return GetAssetLogger()->name();
				default: break;
			}

			LK_CORE_ASSERT(false, "Invalid logger type");
			return "";
		}


	private:
		inline static std::shared_ptr<spdlog::logger> CoreLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> ClientLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> UILogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> AssetLogger = nullptr;

		/* Tag details for every logger type. */
		inline static std::map<std::string, FTagDetails> EnabledTags;
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
#define LK_CLIENT_TRACE(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, "", __VA_ARGS__)
#define LK_CLIENT_DEBUG(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, "", __VA_ARGS__)
#define LK_CLIENT_INFO(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, "", __VA_ARGS__)
#define LK_CLIENT_WARN(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warn, "", __VA_ARGS__)
#define LK_CLIENT_ERROR(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, "", __VA_ARGS__)
#define LK_CLIENT_FATAL(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, "", __VA_ARGS__)

/* UI Logging. */
#define LK_UI_TRACE(...)                ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, "", __VA_ARGS__)
#define LK_UI_DEBUG(...)                ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, "", __VA_ARGS__)
#define LK_UI_INFO(...)                 ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info, "", __VA_ARGS__)
#define LK_UI_WARN(...)                 ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warn, "", __VA_ARGS__)
#define LK_UI_ERROR(...)                ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, "", __VA_ARGS__)
#define LK_UI_FATAL(...)                ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, "", __VA_ARGS__)

#define LK_UI_TRACE_TAG(_TAG, ...)      ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, _TAG, __VA_ARGS__)
#define LK_UI_DEBUG_TAG(_TAG, ...)      ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, _TAG, __VA_ARGS__)
#define LK_UI_INFO_TAG(_TAG, ...)       ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info, _TAG, __VA_ARGS__)
#define LK_UI_WARN_TAG(_TAG, ...)       ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warn, _TAG, __VA_ARGS__)
#define LK_UI_ERROR_TAG(_TAG, ...)      ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, _TAG, __VA_ARGS__)
#define LK_UI_FATAL_TAG(_TAG, ...)      ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, _TAG, __VA_ARGS__)

/* FIXME: In the future */
/* Asset Logging. */
#define LOG_ASSET(...) 			        ::LkEngine::LLog::GetAssetLogger()->debug(__VA_ARGS__)
#define LK_ASSET_LOG(...) 			    ::LkEngine::LLog::GetAssetLogger()->debug(__VA_ARGS__)


namespace LkEngine {

	template<typename... TArgs>
	void LLog::PrintMessage(ELoggerType LoggerType, ELogLevel Level, 
							std::string_view Tag, TArgs&&... Args)
	{
		FTagDetails& TagDetails = EnabledTags[GetLoggerName(LoggerType).data()];
		if (TagDetails.Enabled && TagDetails.Filter <= Level)
		{
			std::shared_ptr<spdlog::logger>& Logger = LLog::GetLogger(LoggerType);
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
