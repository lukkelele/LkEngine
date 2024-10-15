#include "LKpch.h"
#include "LkEngine/Core/Log.h"


namespace LkEngine {

	LLog::LLog()
	{
		// TODO: Read config
		Initialize("LkEditor.log", "CORE", "CLIENT");
	}

	LLog::~LLog()
	{
		// TODO
	}

	LLog& LLog::Instance()
	{
		static LLog Instance;
		return Instance;
	}

	void LLog::Initialize(std::string_view logfilename,
						  std::string_view coreLoggerName, 
						  std::string_view clientLoggerName)
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfilename.data(), true));

		logSinks[0]->set_pattern("%^%H:%M:%S | %n: %v%$");
		logSinks[1]->set_pattern("%H:%M:%S | %n: %v");

		/* Core Logger. */
		m_CoreLogger = MakeShared<spdlog::logger>(coreLoggerName.data(), begin(logSinks), end(logSinks));
		m_CoreLogger->set_level(spdlog::level::trace);
		m_CoreLogger->flush_on(spdlog::level::trace);
		spdlog::register_logger(m_CoreLogger);

		/* Client Logger. */
		m_ClientLogger = std::make_shared<spdlog::logger>(clientLoggerName.data(), begin(logSinks), end(logSinks));
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ClientLogger->flush_on(spdlog::level::trace);
		spdlog::register_logger(m_ClientLogger);

		/* Asset Logger. */
		m_AssetLogger = std::make_shared<spdlog::logger>("ASSET", begin(logSinks), end(logSinks));
		m_AssetLogger ->set_level(spdlog::level::debug);
		m_AssetLogger ->flush_on(spdlog::level::debug);
		spdlog::register_logger(m_AssetLogger);
	}

}
