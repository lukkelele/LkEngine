/******************************************************************
 * LLog
 *
 * Logger class and logging implementation for the entire engine.
 ******************************************************************/
#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LogMacros.h"
#include "LogFormatters.h"

#include <stdint.h>
#include <cstring>
#include <codecvt>
#include <filesystem>
#include <format>
#include <locale>
#include <map>

#if LK_LOG_USE_IOSTREAM
#	include <iostream>
#	include <iterator>
#endif

#define LK_ASSERT_MESSAGE_BOX 1

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
#include "LkEngine/Core/Utility/StringUtils.h"

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
            FTagDetails(const ELogLevel InFilter) : Filter(InFilter) {}
        };

        LLog();
        ~LLog();

        static LLog& Get();

        static void Initialize(std::string_view LogfileName = "");
		static void RegisterLoggers();

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

            return GetLogger_Core();
        }

        template<typename... TArgs>
        static void PrintMessage(const ELoggerType LoggerType, const ELogLevel Level,
                                 std::format_string<TArgs...> Format, TArgs&&... Args);

        template<typename... TArgs>
        static void PrintMessageWithTag(const ELoggerType LoggerType, const ELogLevel Level, std::string_view Tag,
                                        std::format_string<TArgs...> Format, TArgs&&... Args);

        static void PrintMessageWithTag(const ELoggerType LoggerType, const ELogLevel Level,
                                        std::string_view Tag, std::string_view Message);

        template<typename... TArgs>
        static void PrintAssertMessage(const ELoggerType LoggerType, std::string_view Prefix,
                                       std::format_string<TArgs...> Message, TArgs&&... Args);
        static void PrintAssertMessage(const ELoggerType LoggerType, std::string_view Prefix);

		template<typename... TArgs>
		static void Print(std::format_string<TArgs...> Format, TArgs&&... Args)
		{
		#if LK_LOG_USE_IOSTREAM
			std::ostream_iterator<char> Out(std::cout);
			std::format_to(Out, Format, std::forward<TArgs>(Args)...);
		#else
			const std::string FormattedString = std::format(Format, std::forward<TArgs>(Args)...);
			printf("%s", FormattedString.c_str());
		#endif
		}

		template<typename... TArgs>
		static void PrintLn(std::format_string<TArgs...> Format, TArgs&&... Args)
		{
		#if LK_LOG_USE_IOSTREAM
			std::ostream_iterator<char> Out(std::cout);
			std::format_to(Out, Format, std::forward<TArgs>(Args)...);
			*Out = '\n';
		#else
			const std::string FormattedString = std::format(Format, std::forward<TArgs>(Args)...);
			printf("%s\n", FormattedString.c_str());
		#endif
		}

        FORCEINLINE static const char* LevelToString(const ELogLevel Level)
        {
            switch (Level)
            {
                case ELogLevel::Trace:   return "Trace";
                case ELogLevel::Debug:	 return "Debug";
                case ELogLevel::Info:	 return "Info";
                case ELogLevel::Warning: return "Warning";
                case ELogLevel::Error:   return "Error";
                case ELogLevel::Fatal:   return "Fatal";
            }

            LK_CORE_ASSERT(false, "Unknown log level: {}", static_cast<int>(Level));
            return "";
        }

        FORCEINLINE static ELogLevel LevelFromString(std::string_view InString)
        {
			const std::string StrLower = StringUtils::ToLower(InString);
            if (StrLower == "trace")         return ELogLevel::Trace;
            else if (StrLower == "debug")    return ELogLevel::Debug;
            else if (StrLower == "info")     return ELogLevel::Info;
            else if (StrLower == "warning")  return ELogLevel::Warning;
            else if (StrLower == "error")    return ELogLevel::Error;
            else if (StrLower == "fatal")    return ELogLevel::Fatal;

            LK_CORE_ASSERT(false, "LevelFromString failed for \"{}\" (lower: \"{}\")", InString, StrLower);
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


namespace LkEngine
{

	template<typename... TArgs>
	void LLog::PrintMessage(const ELoggerType LoggerType, const ELogLevel Level,
							std::format_string<TArgs...> Format, TArgs&&... Args)
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
								  std::format_string<TArgs...> Format, TArgs&&... Args)
	{
		const std::string FormattedString = std::format(Format, std::forward<TArgs>(Args)...);
		if (auto Logger = GetLogger(LoggerType); Logger != nullptr)
		{
			Logger->error("{0}: {1}", Prefix, FormattedString);
		}
		else
		{
			PrintLn("{2}{0}: {1}{3}", Prefix, FormattedString, LK_ANSI_COLOR_BG_BRIGHT_RED, LK_ANSI_COLOR_RESET);
		}

	#if LK_ASSERT_MESSAGE_BOX
		MessageBoxA(nullptr, FormattedString.c_str(), "LkEngine Error", (MB_OK | MB_ICONERROR));
	#endif
	}

	FORCEINLINE void LLog::PrintAssertMessage(const ELoggerType LoggerType, std::string_view Message)
	{
		if (auto Logger = GetLogger(LoggerType); Logger != nullptr)
		{
			LLog::GetLogger(LoggerType)->error("{0}", Message);
		}
		else
		{
			PrintLn("{1}{0}{2}", Message, LK_ANSI_COLOR_BG_BRIGHT_RED, LK_ANSI_COLOR_RESET);
		}
		
	#if LK_ASSERT_MESSAGE_BOX
		MessageBoxA(nullptr, Message.data(), "LkEngine Error", (MB_OK | MB_ICONERROR));
	#endif
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
					}
				}
			}
		}
	}
}

