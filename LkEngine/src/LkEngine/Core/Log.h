#pragma once

#include <stdint.h>
#include <cstring>
#include <map>

// Ignore all warnings raised inside external headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/fmt.h>
#pragma warning(pop)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>


namespace LkEngine {

	class Log 
	{
	public:
		enum class Level 
		{
			Trace, 
			Debug,
			Info, 
			Warn, 
			Error, 
			Fatal	
		};

		enum Type { Core = 0, Client = 1, Asset = 2 };

		struct TagDetails
		{
			bool Enabled = true;
			Level LevelFilter = Level::Trace;
		};

		static void Init(std::string logfileName = "Logger.log",
						 std::string coreLoggerName = "CORE",
						 std::string clientLoggerName = "CLIENT");

		static std::shared_ptr<spdlog::logger>& GetCoreLogger()   { return m_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		static std::shared_ptr<spdlog::logger>& GetAssetLogger()  { return m_AssetLogger; }

		static std::shared_ptr<spdlog::logger>& GetLogger(Log::Type type)
		{
			switch (type)
			{
				case Type::Core:   return GetCoreLogger();
				case Type::Client: return GetClientLogger();
				case Type::Asset:  return GetClientLogger();
				default: throw std::runtime_error("PrintAssertMessage: Passed logger type could not be determined");
			}
		}

		template<typename... Args>
		static void PrintMessage(Log::Type type, Log::Level level, std::string_view tag, Args&&... args);

		template<typename... Args>
		static void PrintAssertMessage(Log::Type type, std::string_view prefix, Args&&... args);

		static const char* LevelToString(Level level)
		{
			switch (level)
			{
				case Level::Trace: return "Trace";
				case Level::Info:  return "Info";
				case Level::Warn:  return "Warn";
				case Level::Error: return "Error";
				case Level::Fatal: return "Fatal";
			}
			return "";
		}

		static Level LevelFromString(std::string_view string)
		{
			if (string == "Trace") return Level::Trace;
			if (string == "Info")  return Level::Info;
			if (string == "Warn")  return Level::Warn;
			if (string == "Error") return Level::Error;
			if (string == "Fatal") return Level::Fatal;

			return Level::Trace;
		}

	private:
		inline static std::shared_ptr<spdlog::logger> m_CoreLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> m_ClientLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> m_AssetLogger = nullptr;

		inline static std::map<std::string, TagDetails> m_EnabledTags;
	};

}

// Core log macros 
#define LK_CORE_TRACE(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Trace, "", __VA_ARGS__)
#define LK_CORE_DEBUG(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Debug, "", __VA_ARGS__)
#define LK_CORE_INFO(...)               ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Info, "", __VA_ARGS__)
#define LK_CORE_WARN(...)               ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Warn, "", __VA_ARGS__)
#define LK_CORE_ERROR(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Error, "", __VA_ARGS__)
#define LK_CORE_FATAL(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Fatal, "", __VA_ARGS__)

#define LK_CORE_TRACE_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Trace, _TAG, __VA_ARGS__)
#define LK_CORE_DEBUG_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Debug, _TAG, __VA_ARGS__)
#define LK_CORE_INFO_TAG(_TAG, ...)     ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Info, _TAG, __VA_ARGS__)
#define LK_CORE_WARN_TAG(_TAG, ...)     ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Warn, _TAG, __VA_ARGS__)
#define LK_CORE_ERROR_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Error, _TAG, __VA_ARGS__)
#define LK_CORE_FATAL_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Core, ::LkEngine::Log::Level::Fatal, _TAG, __VA_ARGS__)

// Client log macros 
#define LK_TRACE(...)                   ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Client, ::LkEngine::Log::Level::Trace, "", __VA_ARGS__)
#define LK_INFO(...)                    ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Client, ::LkEngine::Log::Level::Info, "", __VA_ARGS__)
#define LK_WARN(...)                    ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Client, ::LkEngine::Log::Level::Warn, "", __VA_ARGS__)
#define LK_ERROR(...)                   ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Client, ::LkEngine::Log::Level::Warn, "", __VA_ARGS__)
#define LK_FATAL(...)                   ::LkEngine::Log::PrintMessage(::LkEngine::Log::Type::Client, ::LkEngine::Log::Level::Fatal, "", __VA_ARGS__)

#define LOG_ASSET(...) 			        ::LkEngine::Log::GetAssetLogger()->debug(__VA_ARGS__)
#define LK_ASSET_LOG(...) 			    ::LkEngine::Log::GetAssetLogger()->debug(__VA_ARGS__)


namespace LkEngine {

	template<typename... Args>
	void Log::PrintMessage(Log::Type type, Log::Level level, std::string_view tag, Args&&... args)
	{
		auto detail = m_EnabledTags[std::string(tag)];
		if (detail.Enabled && detail.LevelFilter <= level)
		{
			auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
			std::string logString = tag.empty() ? "{0}{1}" : "[{0}] {1}";
			switch (level)
			{
				case Level::Trace:
					logger->trace(logString, tag, fmt::format(std::forward<Args>(args)...));
					break;
				case Level::Debug:
					logger->debug(logString, tag, fmt::format(std::forward<Args>(args)...));
					break;
				case Level::Info:
					logger->info(logString, tag, fmt::format(std::forward<Args>(args)...));
					break;
				case Level::Warn:
					logger->warn(logString, tag, fmt::format(std::forward<Args>(args)...));
					break;
				case Level::Error:
					logger->error(logString, tag, fmt::format(std::forward<Args>(args)...));
					break;
				case Level::Fatal:
					logger->critical(logString, tag, fmt::format(std::forward<Args>(args)...));
					break;
			}
		}
	}

	template<typename... Args>
	void Log::PrintAssertMessage(Log::Type type, std::string_view prefix, Args&&... args)
	{
		auto logger = Log::GetLogger(type);
		logger->error("{0}: {1}", prefix, fmt::format(std::forward<Args>(args)...));
	}

	template<>
	inline void Log::PrintAssertMessage(Log::Type type, std::string_view prefix)
	{
		auto logger = Log::GetLogger(type);
		logger->error("{0}", prefix);
	}

}