#pragma once
#include <stdint.h>
#include <cstring>

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/fmt.h>
#pragma warning(pop)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>


namespace LkEngine {

	class Logger
	{
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	public:

		enum class LK_ASSERT_LEVEL
		{
			TRACE, 
			DEBUG,
			INFO, 
			WARN, 
			ERR, 
			CRITICAL
		};

		static void Init(std::string logfileName = "Logger.log",
						 std::string coreLoggerName = "CORE",
						 std::string clientLoggerName = "CLIENT");

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		template<typename... ARGS>
		static void PrintMessage(LK_ASSERT_LEVEL level, ARGS&&... args);

	};

	// FIXME: fmt::v9::format error when trying to parse passed __VA_ARGS__
	template<typename... ARGS>
	void Logger::PrintMessage(LK_ASSERT_LEVEL level, ARGS&&... args)
	{
		auto logger = GetCoreLogger();
		// auto str = fmt::format(std::forward<ARGS>(args)...);
		switch (level)
		{
			case LK_ASSERT_LEVEL::INFO:
				// logger->info("{0}", fmt::format(std::forward<ARGS>(args)...));
				break;
			case LK_ASSERT_LEVEL::CRITICAL:
				// logger->critical("{0}", fmt::format(std::forward<ARGS>(args)...));
				logger->critical("{0}", "Assertion failed");
				break;
		}
	}

}

// Core log macros 
#define LOG_TRACE(...)     			::LkEngine::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)     			::LkEngine::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)    			::LkEngine::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     			::LkEngine::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    			::LkEngine::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) 			::LkEngine::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros 
#define LOG_CLIENT_TRACE(...)		::LkEngine::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_CLIENT_DEBUG(...)       ::LkEngine::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define LOG_CLIENT_INFO(...)		::LkEngine::Logger::GetClientLogger()->info(__VA_ARGS__)
#define LOG_CLIENT_WARN(...)        ::LkEngine::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_CLIENT_ERROR(...)       ::LkEngine::Logger::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CLIENT_CRITICAL(...)    ::LkEngine::Logger::GetClientLogger()->critical(__VA_ARGS__)

