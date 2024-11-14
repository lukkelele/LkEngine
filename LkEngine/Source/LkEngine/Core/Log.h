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

/** 
 * Set loglevel names to UPPERCASE.
 * Must be defined BEFORE including spdlog.
 */
#define SPDLOG_LEVEL_NAMES \
	{ "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF" }

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
		struct FTagDetails
		{
			bool Enabled = true;
			ELogLevel Filter = ELogLevel::Debug;

			FTagDetails() = default;
			FTagDetails(const ELogLevel InFilter) : Filter(InFilter) {}
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

	#if defined(LK_PLATFORM_WINDOWS)
		template<typename... TArgs>
		static void PrintMessage(const ELoggerType LoggerType, const ELogLevel Level, std::format_string<TArgs...> Tag, TArgs&&... Args);
	#elif defined(LK_PLATFORM_LINUX)
		template<typename... TArgs>
		static void PrintMessage(const ELoggerType LoggerType, const ELogLevel level, std::string_view Tag, TArgs&&... Args);
	#endif

		template<typename... TArgs>
		static void PrintMessageWithTag(const ELoggerType LoggerType, const ELogLevel Level, std::string_view Tag, 
										std::format_string<TArgs...> Format, TArgs&&... Args);

		static void PrintMessageWithTag(const ELoggerType LoggerType, const ELogLevel Level, 
										std::string_view Tag, std::string_view Message);

		template<typename... TArgs>
		static void PrintAssertMessage(const ELoggerType LoggerType, std::string_view Prefix, 
									   std::format_string<TArgs...> Message, TArgs&&... Args);
		static void PrintAssertMessage(const ELoggerType LoggerType, std::string_view Prefix);

		FORCEINLINE static const char* LevelToString(const ELogLevel Level)
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
			if (InString == "Trace")  return ELogLevel::Trace;
			if (InString == "Info")   return ELogLevel::Info;
			if (InString == "Warn")   return ELogLevel::Warn;
			if (InString == "Error")  return ELogLevel::Error;
			if (InString == "Fatal")  return ELogLevel::Fatal;

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

		/** @brief Tag details for every logger type. */
		inline static std::map<std::string, FTagDetails> EnabledTags;
	};

}

/* Core Logging. */
#define LK_CORE_TRACE(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_CORE_DEBUG(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_CORE_INFO(...)              ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_CORE_WARN(...)              ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warn,  __VA_ARGS__)
#define LK_CORE_ERROR(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_CORE_FATAL(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_CORE_TRACE_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_CORE_DEBUG_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_CORE_INFO_TAG(Tag, ...)     ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info,  Tag, __VA_ARGS__)
#define LK_CORE_WARN_TAG(Tag, ...)     ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warn,  Tag, __VA_ARGS__)
#define LK_CORE_ERROR_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_CORE_FATAL_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* Client Logging. */
#define LK_CLIENT_TRACE(...)           ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_CLIENT_DEBUG(...)           ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_CLIENT_INFO(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_CLIENT_WARN(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warn,  __VA_ARGS__)
#define LK_CLIENT_ERROR(...)           ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_CLIENT_FATAL(...)           ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_CLIENT_TRACE_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_CLIENT_DEBUG_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_CLIENT_INFO_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info,  Tag, __VA_ARGS__)
#define LK_CLIENT_WARN_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warn,  Tag, __VA_ARGS__)
#define LK_CLIENT_ERROR_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_CLIENT_FATAL_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* UI Logging. */
#define LK_UI_TRACE(...)               ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_UI_DEBUG(...)               ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_UI_INFO(...)                ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_UI_WARN(...)                ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warn,  __VA_ARGS__)
#define LK_UI_ERROR(...)               ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_UI_FATAL(...)               ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_UI_TRACE_TAG(Tag, ...)      ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_UI_DEBUG_TAG(Tag, ...)      ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_UI_INFO_TAG(Tag, ...)       ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info,  Tag, __VA_ARGS__)
#define LK_UI_WARN_TAG(Tag, ...)       ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warn,  Tag, __VA_ARGS__)
#define LK_UI_ERROR_TAG(Tag, ...)      ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_UI_FATAL_TAG(Tag, ...)      ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* Asset Logging. */
#define LK_ASSET_TRACE(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_ASSET_DEBUG(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_ASSET_INFO(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_ASSET_WARN(...)             ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Warn,  __VA_ARGS__)
#define LK_ASSET_ERROR(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_ASSET_FATAL(...)            ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_ASSET_TRACE_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_ASSET_DEBUG_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_ASSET_INFO_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Info,  Tag, __VA_ARGS__)
#define LK_ASSET_WARN_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Warn,  Tag, __VA_ARGS__)
#define LK_ASSET_ERROR_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_ASSET_FATAL_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)


namespace LkEngine {

#if defined(LK_PLATFORM_WINDOWS)
	template<typename... TArgs>
	void LLog::PrintMessage(const ELoggerType LoggerType, const ELogLevel Level, std::format_string<TArgs...> Format, TArgs&&... Args)
#elif defined(LK_PLATFORM_LINUX)
	template<typename... TArgs>
	void LLog::PrintMessage(const ELoggerType LoggerType, const ELogLevel Level, std::string_view Tag, TArgs&&... Args)
#endif
	{
		FTagDetails& TagDetails = EnabledTags[GetLoggerName(LoggerType).data()];
		if (TagDetails.Enabled && TagDetails.Filter <= Level)
		{
			auto& Logger = LLog::GetLogger(LoggerType);
			switch (Level)
			{
				case ELogLevel::Trace:
					Logger->trace(Format, std::forward<TArgs>(Args)...);
					break;

				case ELogLevel::Debug:
					Logger->debug(Format, std::forward<TArgs>(Args)...);
					break;

				case ELogLevel::Info:
					Logger->info(Format, std::forward<TArgs>(Args)...);
					break;

				case ELogLevel::Warn:
					Logger->warn(Format, std::forward<TArgs>(Args)...);
					break;

				case ELogLevel::Error:
					Logger->error(Format, std::forward<TArgs>(Args)...);
					break;

				case ELogLevel::Fatal:
					Logger->critical(Format, std::forward<TArgs>(Args)...);
					break;
			}
		}
	}

	template<typename... TArgs>
	void LLog::PrintMessageWithTag(const ELoggerType LoggerType, const ELogLevel Level, std::string_view Tag, 
								   std::format_string<TArgs...> Format, TArgs&&... Args)
	{
		const FTagDetails& TagDetails = EnabledTags[GetLoggerName(LoggerType).data()];
		if (TagDetails.Enabled && (TagDetails.Filter <= Level))
		{
			const std::string FormattedString = std::format(Format, std::forward<TArgs>(Args)...);
			auto& Logger = LLog::GetLogger(LoggerType);
			switch (Level)
			{
				case ELogLevel::Trace:
					Logger->trace("[{0}] {1}", Tag, FormattedString);
					break;

				case ELogLevel::Debug:
					Logger->debug("[{0}] {1}", Tag, FormattedString);
					break;

				case ELogLevel::Info:
					Logger->info("[{0}] {1}", Tag, FormattedString);
					break;

				case ELogLevel::Warn:
					Logger->warn("[{0}] {1}", Tag, FormattedString);
					break;

				case ELogLevel::Error:
					Logger->error("[{0}] {1}", Tag, FormattedString);
					break;

				case ELogLevel::Fatal:
					Logger->critical("[{0}] {1}", Tag, FormattedString);
					break;
			}
		}
	}

	FORCEINLINE void LLog::PrintMessageWithTag(const ELoggerType LoggerType, const ELogLevel Level, 
											   std::string_view Tag, std::string_view Message)
	{
		FTagDetails& TagDetails = EnabledTags[GetLoggerName(LoggerType).data()];
		if (TagDetails.Enabled && TagDetails.Filter <= Level)
		{
			auto& Logger = LLog::GetLogger(LoggerType);
			switch (Level)
			{
				case ELogLevel::Trace:
					Logger->trace("[{0}] {1}", Tag, Message);
					break;

				case ELogLevel::Debug:
					Logger->debug("[{0}] {1}", Tag, Message);
					break;

				case ELogLevel::Info:
					Logger->info("[{0}] {1}", Tag, Message);
					break;

				case ELogLevel::Warn:
					Logger->warn("[{0}] {1}", Tag, Message);
					break;

				case ELogLevel::Error:
					Logger->error("[{0}] {1}", Tag, Message);
					break;

				case ELogLevel::Fatal:
					Logger->critical("[{0}] {1}", Tag, Message);
					break;
			}
		}

	}

	template<typename ...TArgs>
	void LLog::PrintAssertMessage(const ELoggerType LoggerType, std::string_view Prefix, 
								  std::format_string<TArgs...> Message, TArgs&&... Args)
	{
		const std::string FormattedString = std::format(Message, std::forward<TArgs>(Args)...);
		GetLogger(LoggerType)->error("{0}: {1}", Prefix, FormattedString);
	}

	FORCEINLINE void LLog::PrintAssertMessage(const ELoggerType LoggerType, std::string_view Prefix)
	{
		LLog::GetLogger(LoggerType)->error("{0}", Prefix);
	}

}
