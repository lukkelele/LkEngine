#include "LKpch.h"
#include "LkEngine/Core/Logger.h"

namespace LkEngine {

	void Logger::Init(std::string logfilename,
		           std::string coreLoggerName,
			       std::string clientLoggerName)
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfilename, true));

		logSinks[0]->set_pattern("%^%H:%M:%S | %n: %v%$");
		logSinks[1]->set_pattern("%H:%M:%S | %n: %v");

		// Core Logger
		m_CoreLogger = std::make_shared<spdlog::logger>(coreLoggerName, begin(logSinks), end(logSinks));
		m_CoreLogger->set_level(spdlog::level::trace);
		m_CoreLogger->flush_on(spdlog::level::trace);
		spdlog::register_logger(m_CoreLogger);

		// Client Logger
		m_ClientLogger = std::make_shared<spdlog::logger>(clientLoggerName, begin(logSinks), end(logSinks));
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ClientLogger->flush_on(spdlog::level::trace);
		spdlog::register_logger(m_ClientLogger);

		// Asset Logger
		m_AssetLogger = std::make_shared<spdlog::logger>("ASSET", begin(logSinks), end(logSinks));
		m_AssetLogger ->set_level(spdlog::level::debug);
		m_AssetLogger ->flush_on(spdlog::level::debug);
		spdlog::register_logger(m_AssetLogger);
	}

}
