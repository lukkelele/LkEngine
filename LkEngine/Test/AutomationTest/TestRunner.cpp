#include "TestRunner.h"
#include "TestLayer.h"

#include <LkEngine/Core/Application.h>
#include <LkEngine/Core/IO/FileSystem.h>

#include "AutomationTest/TestManager.h"


namespace LkEngine {

	LTestRunner::LTestRunner()
	{
		/* Create 'Results' directory to store test results in if it does not already exist. */
		if (!LFileSystem::Exists(std::filesystem::current_path() / "Results"))
		{
			LFileSystem::CreateDirectory(std::filesystem::current_path() / "Results");
		}

		/* Register test logger. */
		std::vector<LogLevelColorConfig> LevelConfigs = {
			{ ELogLevel::Debug, Color::Cyan  },
			{ ELogLevel::Info,  Color::White },
			{ ELogLevel::Error,	Color::Red   }
		};
	
		LLog& Logger = LLog::Get();
		Logger.RegisterLogger(ELoggerType::TestRunner, "TEST", ELogLevel::Trace, LevelConfigs);
	}

	void LTestRunner::Run()
	{
		LK_TEST_TRACE_TAG("TestRunner", "Running tests");
		LTestManager::Get().RunTests(Test::ETestSuite::Core);
	}

}


int main(int Argc, char* Argv[])
{
	using namespace LkEngine;
	//LkEngine::Core::Setup(Argc, Argv); /* Does not work with the Github Action Runner for some reason. */

    LTestRunner TestRunner;
	TestRunner.Run();
	LK_PRINTLN("Test results are placed in: {}", (std::filesystem::current_path() / "Results").string());

    return 0;
}
