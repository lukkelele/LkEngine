#include "LKpch.h"
#include "LkEngine/Core/Log.h"

#include "LkEngine/Core/Time/Time.h"
#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/IO/File.h"

#include <spdlog/sinks/ansicolor_sink.h>


namespace LkEngine {

	namespace Color
	{
		constexpr uint16_t Black   = 0x0000;
		constexpr uint16_t Blue    = 0x0001;
		constexpr uint16_t Green   = 0x0002;
		constexpr uint16_t Cyan    = 0x0003;
		constexpr uint16_t Red     = 0x0004;
		constexpr uint16_t Magenta = 0x0005;
		constexpr uint16_t Yellow  = 0x0006;
		constexpr uint16_t White   = 0x0007;

		namespace Formatter 
		{
			constexpr uint16_t Reset     = 0x0000;
			constexpr uint16_t Bold      = 0x0100; 
			constexpr uint16_t Dark      = 0x0200;
			constexpr uint16_t Underline = 0x0400;
			constexpr uint16_t Blink     = 0x0800;
			constexpr uint16_t Reverse   = 0x1000;
			constexpr uint16_t Concealed = 0x2000;
			constexpr uint16_t ClearLine = 0x4000;
		}

		namespace Background 
		{
			constexpr uint16_t Black   = 0x0000;
			constexpr uint16_t Red     = 0x0010;
			constexpr uint16_t Green   = 0x0020;
			constexpr uint16_t Yellow  = 0x0030;
			constexpr uint16_t Blue    = 0x0040;
			constexpr uint16_t Magenta = 0x0050;
			constexpr uint16_t Cyan    = 0x0060;
			constexpr uint16_t White   = 0x0070;
		}

		namespace Bold 
		{
			constexpr uint16_t Black   = 0xFF00;
			constexpr uint16_t Blue    = 0xFF01;
			constexpr uint16_t Green   = 0xFF02;
			constexpr uint16_t Cyan    = 0xFF03;
			constexpr uint16_t Red     = 0xFF04;
			constexpr uint16_t Magenta = 0xFF05;
			constexpr uint16_t Yellow  = 0xFF06;
			constexpr uint16_t White   = 0xFF07;
		}
	}

	namespace fs = std::filesystem;

	/** Assuming the log files are formatted with a timestamp. */
	static bool CompareLogFiles(const fs::directory_entry& A, const fs::directory_entry& B);

	/** Get count of logfiles present in directory. */
	static int CountLogFilesInDir(const fs::path& InDirectory);

	static void CleanLogDirectory(const std::filesystem::path& InDirectory, const int MaxLogFiles);

	LLog::LLog()
	{
		// TODO: Read configuration and application args for setting log levels on initialization.
		Initialize(
			LK_FORMAT_STRING("LkEditor-{}.log", Time::CurrentTimestamp()).c_str(),
			"CORE", 
			"CLIENT"
		);

		LogDirectory = fs::path(LK_FORMAT_STRING("{}/Logs", fs::current_path().string()).c_str());
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
		static constexpr const char* ColorSinkPattern  = "%^[%H:%M:%S] [%t] [%n] %v%$";
		static constexpr const char* UISinkPattern     = "%^[%H:%M:%S] [%t] [%n] %v%$";
		/* Logfile sink. */
		static constexpr const char* FileSinkPattern   = "[%H:%M:%S] [%l] [%t] [%n] %v";

		const std::string Logfile = LogDirectory.string() + std::string(InLogFilename);

		/* Create the color and file sink. */
		std::vector<spdlog::sink_ptr> LogSinks;
		LogSinks.reserve(2);

		/* Color Sink. */
		auto ColorSinkLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		ColorSinkLogger->set_color(spdlog::level::trace, Color::Green);
		ColorSinkLogger->set_color(spdlog::level::debug, Color::Cyan);
		ColorSinkLogger->set_color(spdlog::level::info, Color::White);
		LogSinks.push_back(ColorSinkLogger);

		/* Logfile Sink. */
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

			/* Colors. */
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

			std::shared_ptr<ColorSinkType> AssetColorSink = std::make_shared<ColorSinkType>();
			AssetColorSink->set_pattern(UISinkPattern);

			std::vector<spdlog::sink_ptr> UILogSinks { AssetColorSink, LogSinks[1] };

			/* Colors. */
			AssetColorSink->set_color(spdlog::level::trace, (FOREGROUND_BLUE | FOREGROUND_RED));
			AssetColorSink->set_color(spdlog::level::debug, (FOREGROUND_GREEN | FOREGROUND_BLUE));
			AssetColorSink->set_color(spdlog::level::info,  (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED));

			AssetLogger = std::make_shared<spdlog::logger>("ASSET", LogSinks.begin(), LogSinks.end());
			AssetLogger->set_level(ToSpdlogLevel(LogLevel));
			AssetLogger->flush_on(ToSpdlogLevel(LogLevel));
			spdlog::register_logger(AssetLogger);

			EnabledTags[AssetLogger->name()] = LogLevel;
		}
	}

	/** Assuming the log files are formatted with a timestamp. */
	static bool CompareLogFiles(const fs::directory_entry& A, const fs::directory_entry& B)
	{
		return (A.path().filename().string() < B.path().filename().string());
	}

	/** Get count of logfiles present in directory. */
	static int CountLogFilesInDir(const fs::path& InDirectory)
	{
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
