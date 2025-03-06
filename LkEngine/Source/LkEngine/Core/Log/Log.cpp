#include "LKpch.h"
#include "Log.h"

#include "LkEngine/Core/Time/Time.h"
#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/IO/File.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#if LK_ENGINE_CONSOLE_ENABLED
#	include "LkEngine/Editor/EditorConsole/ConsoleSink.h"
#endif


namespace LkEngine {

	namespace fs = std::filesystem;

	namespace 
	{
	#if defined(LK_ENGINE_EDITOR)
		constexpr const char* FileName = "LkEditor";
	#elif defined(LK_ENGINE_AUTOMATION_TEST)
		constexpr const char* FileName = "LkTestRunner";
	#else
		constexpr const char* FileName = "LkEngine";
	#endif

		/* Color and file sink. */
		std::string Logfile;

		std::filesystem::path LogDirectory;
	}

	LLog::LLog()
	{
		/* Use binary workdir as directory for logs. */
		LogDirectory = std::filesystem::path(std::format("{}/Logs/", LFileSystem::GetRuntimeDir()));
	#if defined(LK_ENGINE_AUTOMATION_TEST)
		LK_PRINTLN("Log directory: {}", LogDirectory);
	#endif

		/* Keep a maximum of 10 logfiles present. */
		LogUtility::CleanLogDirectory(LogDirectory, 10);
	}

	LLog::~LLog()
	{
		/* FIXME: The call to spdlog::shutdown causes errors in the test runner for some odd reason.
		 *        Seems to be something with the release of the registry. */
		//spdlog::shutdown();
	}

	LLog& LLog::Get()
	{
		static LLog Instance;
		return Instance;
	}

	void LLog::Initialize(std::string_view InLogFilename)
	{
		if (!InLogFilename.empty())
		{
			Logfile = InLogFilename;	
		}
		else
		{
			Logfile = std::format("{}-{}.log", FileName, Time::CurrentTimestamp());
		}

		const std::string AppLogFilename = std::format("App-{}.log", Time::CurrentTimestamp());

	#if defined(LK_ENGINE_STDOUT_FLUSH_ALWAYS)
		setvbuf(stdout, nullptr, _IONBF, 0);
	#endif

		std::vector<spdlog::sink_ptr> CoreSinks = {
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogDirectory.string() + Logfile, true),
		#if LK_ENGINE_CONSOLE_ENABLED
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		#endif
		};

		const std::string AppLogFile = LogDirectory.string() + AppLogFilename;
		std::vector<spdlog::sink_ptr> AppSinks = {
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(AppLogFile, true),
		#if LK_ENGINE_CONSOLE_ENABLED
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		#endif
		};

		std::vector<spdlog::sink_ptr> EditorConsoleSinks = {
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(AppLogFile, true),
		#if LK_ENGINE_CONSOLE_ENABLED
			std::make_shared<LEditorConsoleSink>(1)
		#endif
		};

		/**
		 * Index 0: File
		 * Index 1: Stdout
		 */
		CoreSinks[0]->set_pattern("[%T] [%l] [%n] %v");
		AppSinks[0]->set_pattern("[%T] [%l] [%n] %v");

	#if LK_ENGINE_CONSOLE_ENABLED
		CoreSinks[1]->set_pattern("%^[%T] [%l] [%n] %v%$");
		if (auto ColorSink = std::static_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(CoreSinks[1]))
		{
			ColorSink->set_color(spdlog::level::trace, Color::Log::Green);
			ColorSink->set_color(spdlog::level::debug, Color::Log::Cyan);
			ColorSink->set_color(spdlog::level::info, Color::Log::White);
		}

		AppSinks[1]->set_pattern("%^[%T] [%n] %v%$");
		if (auto ColorSink = std::static_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(AppSinks[1]))
		{
			ColorSink->set_color(spdlog::level::trace, Color::Log::Green);
			ColorSink->set_color(spdlog::level::debug, Color::Log::Cyan);
			ColorSink->set_color(spdlog::level::info, Color::Log::White);
		}

		EditorConsoleSinks[0]->set_pattern("[%T] [%l] [%n] %v");
		EditorConsoleSinks[1]->set_pattern("%^%v%$");
	#endif

		/* Logger: Core/Engine */
		{
			Logger_Core = std::make_shared<spdlog::logger>("CORE", CoreSinks.begin(), CoreSinks.end());
			Logger_Core->set_level(spdlog::level::trace);
			Logger_Core->flush_on(spdlog::level::trace);
		}

		/* Logger: Client/App */
		{
			Logger_Client = std::make_shared<spdlog::logger>("APP", AppSinks.begin(), AppSinks.end());
			Logger_Client->set_level(spdlog::level::trace);
			Logger_Client->flush_on(spdlog::level::trace);
		}

	#if LK_ENGINE_CONSOLE_ENABLED
		/* Logger: EditorConsole */
		{
			Logger_EditorConsole = std::make_shared<spdlog::logger>("Console", EditorConsoleSinks.begin(), EditorConsoleSinks.end());
			Logger_EditorConsole->set_level(spdlog::level::trace);
		}
	#endif

	}

    void LLog::RegisterLogger(const ELoggerType Type, 
							  const std::string& Name, 
							  const ELogLevel LogLevel,
							  const std::vector<LogLevelColorConfig>& LevelConfigs, 
							  const Color::EColorCode MainColor)
	{
		/* Create the color and file sink. */
		std::vector<spdlog::sink_ptr> Sinks;
		Sinks.reserve(2);

		/* Color sink. */
		auto ColorSinkLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

		/* Default level color configuration. */
		ColorSinkLogger->set_color(spdlog::level::trace, Color::Log::Green);
		ColorSinkLogger->set_color(spdlog::level::debug, Color::Log::Cyan);
		ColorSinkLogger->set_color(spdlog::level::info, Color::Log::White);

		for (const auto& [Level, Color] : LevelConfigs)
		{
			ColorSinkLogger->set_color(ToSpdlogLevel(Level), Color);
		}

		const std::string AnsiColorReset = "\033[0m";
		constexpr const char* SinkPattern = "%^[%H:%M:%S] [%t] [%n] %v%$";
		Sinks.push_back(ColorSinkLogger);
		Sinks[0]->set_pattern(Color::GetEscapeCode(MainColor) + std::string(SinkPattern) + AnsiColorReset);

		auto& Logger = GetLogger(Type);
		Logger = std::make_shared<spdlog::logger>(Name, Sinks.begin(), Sinks.end());
		Logger->set_level(spdlog::level::trace);
		Logger->flush_on(ToSpdlogLevel(LogLevel));
		spdlog::register_logger(Logger);
	}

}
