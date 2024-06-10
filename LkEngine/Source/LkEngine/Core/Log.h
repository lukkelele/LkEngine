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
		enum class ELevel : int
		{
			Trace, 
			Debug,
			Info, 
			Warn, 
			Error, 
			Fatal	
		};

		enum class EType : int
		{ 
			Core = 0, 
			Client,
			Asset
		};

		struct FTagDetails
		{
			bool Enabled = true;
			ELevel LevelFilter = ELevel::Trace;
		};

		static void Init(std::string LogfileName = "Logger.log",
						 std::string CoreLoggerName = "CORE",
						 std::string ClientLoggerName = "CLIENT");

		static std::shared_ptr<spdlog::logger>& GetCoreLogger()   { return m_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		static std::shared_ptr<spdlog::logger>& GetAssetLogger()  { return m_AssetLogger; }

		static std::shared_ptr<spdlog::logger>& GetLogger(Log::EType type)
		{
			switch (type)
			{
				case EType::Core:   return GetCoreLogger();
				case EType::Client: return GetClientLogger();
				case EType::Asset:  return GetClientLogger();
				default: throw std::runtime_error("PrintAssertMessage: Passed Logger type could not be determined");
			}
		}

		template<typename... Args>
		static void PrintMessage(Log::EType type, Log::ELevel level, std::string_view Tag, Args&&... args);

		template<typename... Args>
		static void PrintAssertMessage(Log::EType type, std::string_view prefix, Args&&... args);

		static const char* LevelToString(ELevel Level)
		{
			switch (Level)
			{
				case ELevel::Trace: return "Trace";
				case ELevel::Info:  return "Info";
				case ELevel::Warn:  return "Warn";
				case ELevel::Error: return "Error";
				case ELevel::Fatal: return "Fatal";
			}
			return "";
		}

		static ELevel LevelFromString(std::string_view string)
		{
			if (string == "Trace") return ELevel::Trace;
			if (string == "Info")  return ELevel::Info;
			if (string == "Warn")  return ELevel::Warn;
			if (string == "Error") return ELevel::Error;
			if (string == "Fatal") return ELevel::Fatal;
			return ELevel::Trace;
		}

	private:
		inline static std::shared_ptr<spdlog::logger> m_CoreLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> m_ClientLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> m_AssetLogger = nullptr;

		inline static std::map<std::string, FTagDetails> m_EnabledTags;
	};

}

// Core log macros 
#define LK_CORE_TRACE(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Trace, "", __VA_ARGS__)
#define LK_CORE_DEBUG(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Debug, "", __VA_ARGS__)
#define LK_CORE_INFO(...)               ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Info, "", __VA_ARGS__)
#define LK_CORE_WARN(...)               ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Warn, "", __VA_ARGS__)
#define LK_CORE_ERROR(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Error, "", __VA_ARGS__)
#define LK_CORE_FATAL(...)              ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Fatal, "", __VA_ARGS__)

#define LK_CORE_TRACE_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Trace, _TAG, __VA_ARGS__)
#define LK_CORE_DEBUG_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Debug, _TAG, __VA_ARGS__)
#define LK_CORE_INFO_TAG(_TAG, ...)     ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Info, _TAG, __VA_ARGS__)
#define LK_CORE_WARN_TAG(_TAG, ...)     ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Warn, _TAG, __VA_ARGS__)
#define LK_CORE_ERROR_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Error, _TAG, __VA_ARGS__)
#define LK_CORE_FATAL_TAG(_TAG, ...)    ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Core, ::LkEngine::Log::ELevel::Fatal, _TAG, __VA_ARGS__)

// Client log macros 
#define LK_TRACE(...)                   ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Client, ::LkEngine::Log::ELevel::Trace, "", __VA_ARGS__)
#define LK_INFO(...)                    ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Client, ::LkEngine::Log::ELevel::Info, "", __VA_ARGS__)
#define LK_WARN(...)                    ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Client, ::LkEngine::Log::ELevel::Warn, "", __VA_ARGS__)
#define LK_ERROR(...)                   ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Client, ::LkEngine::Log::ELevel::Warn, "", __VA_ARGS__)
#define LK_FATAL(...)                   ::LkEngine::Log::PrintMessage(::LkEngine::Log::EType::Client, ::LkEngine::Log::ELevel::Fatal, "", __VA_ARGS__)

#define LOG_ASSET(...) 			        ::LkEngine::Log::GetAssetLogger()->debug(__VA_ARGS__)
#define LK_ASSET_LOG(...) 			    ::LkEngine::Log::GetAssetLogger()->debug(__VA_ARGS__)


namespace LkEngine {

	template<typename... Args>
	void Log::PrintMessage(Log::EType Type, Log::ELevel Level, std::string_view Tag, Args&&... args)
	{
		FTagDetails& TagDetails = m_EnabledTags[std::string(Tag)];
		if (TagDetails.Enabled && TagDetails.LevelFilter <= Level)
		{
			std::shared_ptr<spdlog::logger>& Logger = (Type == EType::Core) ? GetCoreLogger() : GetClientLogger();
			std::string LogString = Tag.empty() ? "{0}{1}" : "[{0}] {1}";
			switch (Level)
			{
				case ELevel::Trace:
					Logger->trace(LogString, Tag, fmt::format(std::forward<Args>(args)...));
					break;
				case ELevel::Debug:
					Logger->debug(LogString, Tag, fmt::format(std::forward<Args>(args)...));
					break;
				case ELevel::Info:
					Logger->info(LogString, Tag, fmt::format(std::forward<Args>(args)...));
					break;
				case ELevel::Warn:
					Logger->warn(LogString, Tag, fmt::format(std::forward<Args>(args)...));
					break;
				case ELevel::Error:
					Logger->error(LogString, Tag, fmt::format(std::forward<Args>(args)...));
					break;
				case ELevel::Fatal:
					Logger->critical(LogString, Tag, fmt::format(std::forward<Args>(args)...));
					break;
			}
		}
	}

	template<typename ...ARGS>
	void Log::PrintAssertMessage(Log::EType Type, std::string_view Prefix, ARGS&&... Args)
	{
		std::shared_ptr<spdlog::logger>& Logger = Log::GetLogger(Type);
		Logger->error("{0}: {1}", Prefix, fmt::format(std::forward<ARGS>(Args)...));
	}

	template<>
	inline void Log::PrintAssertMessage(Log::EType Type, std::string_view Prefix)
	{
		std::shared_ptr<spdlog::logger>& Logger = Log::GetLogger(Type);
		Logger->error("{0}", Prefix);
	}

}
