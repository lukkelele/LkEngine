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

#include "LkEngine/Renderer/Color.h"


namespace LkEngine {

    extern std::filesystem::path LogDirectory;

    /** ELogLevel */
    enum class ELogLevel : int
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    enum class ELoggerType : int
    {
        Core = 0,
        Client,
        UI,
        Asset,
		TestRunner,
    };

    struct FLoggerInitArguments
    {
        std::string LogFilename = "LkEngine.log";
        std::string CoreLoggerName = "CORE";
        std::string ClientLoggerName = "CLIENT";
    };

	/**
	 * @brief Color configuration for a log level.
	 */
	using LogLevelColorConfig = std::pair<ELogLevel, uint16_t>;

    class LLog
    {
    public:
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

        static void Initialize(std::string_view LogfileName = "LkEngine.log",
                               std::string_view CoreLoggerName = "CORE",
                               std::string_view ClientLoggerName = "CLIENT");

        static void Initialize(const FLoggerInitArguments& InitArgs);

        static void RegisterLogger(const ELoggerType Type,
								   const std::string& Name,
								   const ELogLevel LogLevel = ELogLevel::Info,
								   const std::vector<LogLevelColorConfig>& LevelConfigs = {},
								   const Color::EColorCode MainColor = Color::EColorCode::Reset);

        FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger_Core() { return Logger_Core; }
        FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger_Client() { return Logger_Client; }
        FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger_UI() { return Logger_UI; }
        FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger_Asset() { return Logger_Asset; }
        FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger_TestRunner() { return Logger_TestRunner; }

        FORCEINLINE static std::shared_ptr<spdlog::logger>& GetLogger(const ELoggerType LoggerType)
        {
            switch (LoggerType)
            {
				case ELoggerType::Core:			return GetLogger_Core();
                case ELoggerType::Client:		return GetLogger_Client();
                case ELoggerType::UI:			return GetLogger_UI();
                case ELoggerType::Asset:		return GetLogger_Asset();
				case ELoggerType::TestRunner:	return GetLogger_TestRunner(); 
            }

            LK_CORE_ASSERT(false, "Unknown logger type");
            return GetLogger_Core();
        }

#if defined(LK_PLATFORM_WINDOWS)
        template<typename... TArgs>
        static void PrintMessage(const ELoggerType LoggerType, const ELogLevel Level,
                                 std::format_string<TArgs...> Format, TArgs&&... Args);
#elif defined(LK_PLATFORM_LINUX)
        template<typename... TArgs>
        static void PrintMessage(const ELoggerType LoggerType, const ELogLevel Level,
                                 fmt::format_string<TArgs...> Format, TArgs&&... Args);
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
                case ELogLevel::Trace:   return "Trace";
                case ELogLevel::Info:	 return "Info";
                case ELogLevel::Warning: return "Warn";
                case ELogLevel::Error:   return "Error";
                case ELogLevel::Fatal:   return "Fatal";
            }

            LK_CORE_ASSERT(false, "Unknown log level: {}", static_cast<int>(Level));
            return "";
        }

        /* TODO: Convert InString to lowercase before check takes place. */
        FORCEINLINE static ELogLevel LevelFromString(std::string_view InString)
        {
            if (InString == "Trace")      return ELogLevel::Trace;
            else if (InString == "Debug") return ELogLevel::Debug;
            else if (InString == "Info")  return ELogLevel::Info;
            else if (InString == "Warn")  return ELogLevel::Warning;
            else if (InString == "Error") return ELogLevel::Error;
            else if (InString == "Fatal") return ELogLevel::Fatal;

            LK_CORE_ASSERT(false, "LevelFromString failed for \"{}\"", InString);
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
                case ELogLevel::Debug:   return spdlog::level::debug;
                case ELogLevel::Info:	 return spdlog::level::info;
                case ELogLevel::Warning: return spdlog::level::warn;
                case ELogLevel::Error:	 return spdlog::level::err;
                case ELogLevel::Fatal:	 return spdlog::level::critical;
            }

			LK_CORE_ASSERT(false, "ToSpdlogLevel error");
            return spdlog::level::info;
        }

        /**
         * @brief Get the name of a logger by using the logger type.
         */
        FORCEINLINE static std::string_view GetLoggerName(const ELoggerType LoggerType)
        {
            switch (LoggerType)
            {
                case ELoggerType::Core:        return GetLogger_Core()->name();
                case ELoggerType::Client:	   return GetLogger_Client()->name();
                case ELoggerType::UI:		   return GetLogger_UI()->name();
                case ELoggerType::Asset:	   return GetLogger_Asset()->name();
                case ELoggerType::TestRunner:  return GetLogger_TestRunner()->name();
            }

            LK_CORE_ASSERT(false, "Invalid logger type");
            return "";
        }


    private:
        inline static std::shared_ptr<spdlog::logger> Logger_Core = nullptr;
        inline static std::shared_ptr<spdlog::logger> Logger_Client = nullptr;
        inline static std::shared_ptr<spdlog::logger> Logger_UI= nullptr;
        inline static std::shared_ptr<spdlog::logger> Logger_Asset = nullptr;
        inline static std::shared_ptr<spdlog::logger> Logger_TestRunner = nullptr;

        /** @brief Tag details for every logger type. */
        inline static std::map<std::string, FTagDetails> EnabledTags;
    };

}

/*----------------------------------------------------------------
	Logging Macros.
-----------------------------------------------------------------*/

/* Core Logging. */
#define LK_CORE_TRACE(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_CORE_DEBUG(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_CORE_INFO(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_CORE_WARN(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_CORE_ERROR(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_CORE_FATAL(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_CORE_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_CORE_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_CORE_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_CORE_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_CORE_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_CORE_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* Client Logging. */
#define LK_TRACE(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_DEBUG(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_INFO(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_WARN(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_ERROR(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_FATAL(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* UI Logging. */
#define LK_UI_TRACE(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_UI_DEBUG(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_UI_INFO(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_UI_WARN(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_UI_ERROR(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_UI_FATAL(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_UI_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_UI_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_UI_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_UI_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_UI_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_UI_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* Asset Logging. */
#define LK_ASSET_TRACE(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_ASSET_DEBUG(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_ASSET_INFO(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_ASSET_WARN(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_ASSET_ERROR(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_ASSET_FATAL(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_ASSET_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_ASSET_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_ASSET_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_ASSET_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_ASSET_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_ASSET_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/* Test Logging. */
#define LK_TEST_TRACE(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_TEST_DEBUG(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_TEST_INFO(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_TEST_WARN(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_TEST_ERROR(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_TEST_FATAL(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_TEST_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_TEST_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_TEST_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_TEST_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_TEST_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_TEST_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

#define LK_TEST_SUCCESS(...)          ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_TEST_FAIL(...)	          ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_TEST_SUCCESS_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info,  Tag, __VA_ARGS__)
#define LK_TEST_FAIL_TAG(Tag, ...)	  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)


namespace LkEngine {

#if defined(LK_PLATFORM_WINDOWS)
	template<typename... TArgs>
	void LLog::PrintMessage(const ELoggerType LoggerType, const ELogLevel Level,
							std::format_string<TArgs...> Format, TArgs&&... Args)
#elif defined(LK_PLATFORM_LINUX)
	template<typename... TArgs>
	void LLog::PrintMessage(const ELoggerType LoggerType, const ELogLevel Level,
							fmt::format_string<TArgs...> Format, TArgs&&... Args)
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
				case ELogLevel::Warning:
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
				case ELogLevel::Warning:
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
				case ELogLevel::Warning:
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

	template<typename... TArgs>
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


/********************************************************
				   Utility functions.
********************************************************/
namespace LkEngine {

	namespace LogUtility 
	{

		/** Assuming the log files are formatted with a timestamp. */
		static bool CompareLogFiles(const std::filesystem::directory_entry& A, const std::filesystem::directory_entry& B)
		{
			return (A.path().filename().string() < B.path().filename().string());
		}

		/** Get count of logfiles present in directory. */
		static int CountLogFilesInDir(const std::filesystem::path& InDirectory)
		{
			namespace fs = std::filesystem;
			int Files = 0;
			if (fs::exists(InDirectory) && fs::is_directory(InDirectory))
			{
				for (const fs::directory_entry& Entry : fs::directory_iterator(InDirectory))
				{
					if (Entry.is_regular_file() && Entry.path().extension() == ".log")
					{
						Files++;
					}
				}
			}

			return Files;
		}

		static void CleanLogDirectory(const std::filesystem::path& InDirectory, const int MaxLogFiles)
		{
			namespace fs = std::filesystem;
			std::vector<fs::directory_entry> LogFiles;
			LogFiles.reserve(MaxLogFiles);

			/* Check if the directory exists. */
			if (fs::exists(InDirectory) && fs::is_directory(InDirectory))
			{
				for (const auto& Entry : fs::directory_iterator(InDirectory))
				{
					if (Entry.is_regular_file() && Entry.path().extension() == ".log")
					{
						LogFiles.push_back(Entry);
					}
				}
			}

			/* If we have more than MaxLogFiles, sort and remove the oldest. */
			if (LogFiles.size() > MaxLogFiles)
			{
				/* Sort log files based on their names (timestamps in filenames). */
				std::sort(LogFiles.begin(), LogFiles.end(), CompareLogFiles);

				/* Remove the oldest files, keeping only the most recent ones. */
				for (std::size_t Index = 0; Index < LogFiles.size() - MaxLogFiles; Index++)
				{
					const fs::directory_entry& LogFile = LogFiles[Index];
					if (LogFile.path().extension() == ".log")
					{
						fs::remove(LogFile.path());
						LK_CORE_TRACE_TAG("Log", "Deleted logfile: {}", LogFile.path().string());
					}
				}
			}
		}
	}
}
