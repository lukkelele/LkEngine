#include "LKpch.h"
#include "LkEngine/Core/Log.h"

#include "LkEngine/Core/Time/Time.h"
#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/IO/File.h"

#include "LkEngine/Renderer/Color.h"

#include <spdlog/sinks/ansicolor_sink.h>


namespace LkEngine {

    std::filesystem::path LogDirectory{ "Logs/" };

	namespace fs = std::filesystem;

	namespace 
	{
	#if defined(LK_EDITOR)
		constexpr const char* FileName = "LkEditor";
	#elif defined(LK_ENGINE_AUTOMATION_TEST)
		constexpr const char* FileName = "LkTest";
	#else
		constexpr const char* FileName = "LkEngine";
	#endif

		const std::string Logfile = LogDirectory.string() + std::string(FileName);

		using ColorSinkType = spdlog::sinks::stdout_color_sink_mt;

		/* Terminal sink. */
		constexpr const char* ColorSinkPattern = "%^[%H:%M:%S] [%t] [%n] %v%$";
		constexpr const char* UISinkPattern    = "%^[%H:%M:%S] [%t] [%n] %v%$";

		/* Logfile sink. */
		constexpr const char* FileSinkPattern = "[%H:%M:%S] [%l] [%t] [%n] %v";

		/* Create the color and file sink. */
		std::vector<spdlog::sink_ptr> LogSinks;
	}

	LLog::LLog()
	{
		LogSinks.reserve(2);
		/* Use binary workdir for log directory. */
		LogDirectory = fs::path(LK_FORMAT_STRING("{}/Logs", fs::current_path().string()).c_str());

		/* Keep a maximum of 10 logfiles present. */
		LogUtility::CleanLogDirectory(LogDirectory, 10);

	#if 0
		// TODO: Read configuration and application args for setting log levels on initialization.
		Initialize(LK_FORMAT_STRING("{}-{}.log", FileName, Time::CurrentTimestamp()).c_str());
		LK_CORE_INFO("Log Directory: {}", LogDirectory.string());
	#endif
	}

	LLog::~LLog()
	{
		// TODO: Release resources.
	}

	LLog& LLog::Get()
	{
		static LLog Instance;
		return Instance;
	}

	void LLog::Initialize(std::string_view InLogFilename)
	{
		std::string Logfile;
		if (!InLogFilename.empty())
		{
			Logfile = InLogFilename;	
		}
		else
		{
			Logfile = LK_FORMAT_STRING("{}-{}.log", FileName, Time::CurrentTimestamp());
		}
		//LK_PRINT("Log Directory: {}", LogDirectory.string());

		//const std::string Logfile = LogDirectory.string() + std::string(InLogFilename);
		spdlog::info("LogFile: {}", Logfile);

		/* Color Sink. */
		auto ColorSinkLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		ColorSinkLogger->set_color(spdlog::level::trace, Color::Green);
		ColorSinkLogger->set_color(spdlog::level::debug, Color::Cyan);
		ColorSinkLogger->set_color(spdlog::level::info, Color::White);
		LogSinks.push_back(ColorSinkLogger);

		/* Logfile Sink. */
		std::printf("Logfile Sink: %s\n", Logfile.c_str());
		LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(Logfile, true));

		LogSinks[0]->set_pattern(ColorSinkPattern);
		LogSinks[1]->set_pattern(FileSinkPattern);

		/* Logger: Core. */
		{
			Logger_Core = std::make_shared<spdlog::logger>("CORE", LogSinks.begin(), LogSinks.end());
			Logger_Core->set_level(spdlog::level::trace);
			Logger_Core->flush_on(spdlog::level::trace);
			spdlog::register_logger(Logger_Core);
		}

		/* Logger: Client. */
		{
			Logger_Client = std::make_shared<spdlog::logger>("CLIENT", LogSinks.begin(), LogSinks.end());
			Logger_Client->set_level(spdlog::level::trace);
			Logger_Client->flush_on(spdlog::level::trace);
			spdlog::register_logger(Logger_Client);
		}

	}

	void LLog::RegisterLoggers()
	{
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
		//const std::string Logfile = LogDirectory.string() + std::string(Logfile);
		LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(Logfile, true));

		LogSinks[0]->set_pattern(ColorSinkPattern);
		LogSinks[1]->set_pattern(FileSinkPattern);

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
			Logger_UI = std::make_shared<spdlog::logger>("UI", UILogSinks.begin(), UILogSinks.end());
			Logger_UI->set_level(ToSpdlogLevel(LogLevel));
			Logger_UI->flush_on(ToSpdlogLevel(LogLevel));
			spdlog::register_logger(Logger_UI);

			EnabledTags[Logger_UI->name()] = LogLevel;
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

			Logger_Asset = std::make_shared<spdlog::logger>("ASSET", LogSinks.begin(), LogSinks.end());
			Logger_Asset->set_level(ToSpdlogLevel(LogLevel));
			Logger_Asset->flush_on(ToSpdlogLevel(LogLevel));
			spdlog::register_logger(Logger_Asset);

			EnabledTags[Logger_Asset->name()] = LogLevel;
		}

	}

    void LLog::RegisterLogger(const ELoggerType Type, 
							  const std::string& Name, 
							  const ELogLevel LogLevel,
							  const std::vector<LogLevelColorConfig>& LevelConfigs, 
							  const Color::EColorCode MainColor)
	{
		using ColorSinkType = spdlog::sinks::stdout_color_sink_mt;

		/* Create the color and file sink. */
		std::vector<spdlog::sink_ptr> Sinks;
		Sinks.reserve(2);

		/* Color sink. */
		auto ColorSinkLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		/* Default level color configuration. */
		ColorSinkLogger->set_color(spdlog::level::trace, Color::Green);
		ColorSinkLogger->set_color(spdlog::level::debug, Color::Cyan);
		ColorSinkLogger->set_color(spdlog::level::info, Color::White);

		for (const auto& [Level, Color]: LevelConfigs)
		{
			ColorSinkLogger->set_color(ToSpdlogLevel(Level), Color);
		}

		static constexpr const char* ColorSinkPattern = "%^[%H:%M:%S] [%t] [%n] %v%$";
		const std::string AnsiColorReset = "\033[0m";

		Sinks.push_back(ColorSinkLogger);
		Sinks[0]->set_pattern(Color::GetEscapeCode(MainColor) + std::string(ColorSinkPattern) + AnsiColorReset);

		auto& Logger = GetLogger(Type);
		Logger = std::make_shared<spdlog::logger>(Name, Sinks.begin(), Sinks.end());
		Logger->set_level(spdlog::level::trace);
		Logger->flush_on(spdlog::level::trace);
		spdlog::register_logger(Logger);
	}

}
