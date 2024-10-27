#include "LKpch.h"
#include "LkEngine/Core/Log.h"

#include "LkEngine/Core/Time/Time.h"
#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/IO/File.h"


namespace LkEngine {

	/** Assuming the log files are formatted with a timestamp. */
	static bool CompareLogFiles(const std::filesystem::directory_entry& A, 
								const std::filesystem::directory_entry& B)
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

	LLog::LLog()
	{
		// TODO: Read configuration and application args for setting log levels on initialization.
		Initialize(
			LString::Format("LkEditor-{}.log", Time::CurrentTimestamp()).CStr(),
			"CORE", 
			"CLIENT"
		);

		namespace fs = std::filesystem;
		LogDirectory = fs::path(LString::Format("{}/Logs", fs::current_path().string()).CStr());
		LK_CORE_TRACE_TAG("Log", "Log Directory: {}", LogDirectory.string());

		CleanLogDirectory(LogDirectory, 10);
	}

	LLog::~LLog()
	{
		// TODO: Release resources.
	}

	LLog& LLog::Instance()
	{
		static LLog Instance;
		return Instance;
	}

	void LLog::Initialize(std::string_view InLogFilename,
						  std::string_view InCoreLoggerName, 
						  std::string_view InClientLoggerName)
	{
		/* 
		 * TODO: Should parse from config and/or args from commandline to set the log levels 
		 *       for all the loggers. 
		 */
		using ColorSinkType = spdlog::sinks::stdout_color_sink_mt;

		/* Terminal sink. */
		static constexpr const char* ColorSinkPattern  = "%^[%H:%M:%S] [%n] %v%$";
		static constexpr const char* UISinkPattern     = "%^[%H:%M:%S] [%n] %v%$";
		/* Logfile sink. */
		static constexpr const char* FileSinkPattern   = "[%H:%M:%S] [%l] [%n] %v";

		/* Set log level names to UPPERCASE. */
		spdlog::level::level_string_views[spdlog::level::trace]    = "TRACE";
		spdlog::level::level_string_views[spdlog::level::debug]    = "DEBUG";
		spdlog::level::level_string_views[spdlog::level::info]     = "INFO";
		spdlog::level::level_string_views[spdlog::level::warn]     = "WARN";
		spdlog::level::level_string_views[spdlog::level::err]      = "ERROR";
		spdlog::level::level_string_views[spdlog::level::critical] = "FATAL";

		/* Create the color and file sink. */
		std::vector<spdlog::sink_ptr> LogSinks;
		LogSinks.reserve(2);
		LogSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		const std::string Logfile = LogDirectory.string() + std::string(InLogFilename);
		LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(Logfile, true));

		LogSinks[0]->set_pattern(ColorSinkPattern);
		LogSinks[1]->set_pattern(FileSinkPattern);

		/* Core Logger. */
		{
			CoreLogger = MakeShared<spdlog::logger>(InCoreLoggerName.data(), LogSinks.begin(), LogSinks.end());
			CoreLogger->set_level(spdlog::level::trace);
			CoreLogger->flush_on(spdlog::level::trace);
			spdlog::register_logger(CoreLogger);
		}

		/* Client Logger. */
		{
			ClientLogger = MakeShared<spdlog::logger>(InClientLoggerName.data(), LogSinks.begin(), LogSinks.end());
			ClientLogger->set_level(spdlog::level::trace);
			ClientLogger->flush_on(spdlog::level::trace);
			spdlog::register_logger(ClientLogger);
		}

		/* UI Logger. */
		{
			ELogLevel LogLevel = ELogLevel::Trace;

			std::shared_ptr<ColorSinkType> UIColorSink = std::make_shared<ColorSinkType>();
			UIColorSink->set_pattern(UISinkPattern);

			std::vector<spdlog::sink_ptr> UILogSinks { UIColorSink, LogSinks[1] };

			std::string Color_Emerald = "\033[38;5;34m"; /* Bright Green. */

			/* Colors. */
			//UIColorSink->set_color(spdlog::level::trace, Color_Emerald.c_str());
			UIColorSink->set_color(spdlog::level::trace, (FOREGROUND_BLUE | FOREGROUND_RED));
			UIColorSink->set_color(spdlog::level::debug, (FOREGROUND_GREEN | FOREGROUND_BLUE));
			UIColorSink->set_color(spdlog::level::info,  (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED));

			/* Register the logger. */
			UILogger = MakeShared<spdlog::logger>("UI", UILogSinks.begin(), UILogSinks.end());
			UILogger->set_level(ToSpdlogLevel(LogLevel));
			UILogger->flush_on(ToSpdlogLevel(LogLevel));
			spdlog::register_logger(UILogger);

			EnabledTags[UILogger->name()] = LogLevel;
		}

		/* Asset Logger. */
		{
			ELogLevel LogLevel = ELogLevel::Info;

			AssetLogger = std::make_shared<spdlog::logger>("ASSET", LogSinks.begin(), LogSinks.end());
			AssetLogger ->set_level(spdlog::level::debug);
			AssetLogger ->flush_on(spdlog::level::debug);
			spdlog::register_logger(AssetLogger);

			EnabledTags[AssetLogger->name()] = LogLevel;
		}
	}

}
