#include "LKpch.h"
#include "LkEngine/Core/Log.h"
#include "LkEngine/Core/Time.h"
#include "LkEngine/Core/Globals.h"


namespace LkEngine {

	LLog::LLog()
	{
		// TODO: Read configuration and application args.
		Initialize(
			LString::Format("LkEditor-{}.log", Time::CurrentTimestamp()).CStr(),
			"CORE", 
			"CLIENT"
		);

		LogDirectory = std::filesystem::current_path();
		LK_CORE_DEBUG("Default Log Directory: {}", LogDirectory.string());

		/* TODO: Only allow X amount of logfiles to exist. */
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
