#include "TestRunner.h"

#include <LkEngine/Core/Application.h>
#include <LkEngine/Core/IO/FileSystem.h>

#include "AutomationTest/TestManager.h"


namespace LkEngine {

	LTestRunner::LTestRunner()
		: ResultsDir(LFileSystem::ConvertToUnixPath(LFileSystem::GetRuntimeDir() / "Results"))
	{
		/* Create 'Results' directory to store test results in if it does not already exist. */
		if (!LFileSystem::Exists(ResultsDir))
		{
			LFileSystem::CreateDirectory(ResultsDir);
		}

		/* Register test logger. */
		std::vector<LogLevelColorConfig> LevelConfigs = {
			{ ELogLevel::Debug, Color::Log::Cyan  },
			{ ELogLevel::Info,  Color::Log::White },
			{ ELogLevel::Error,	Color::Log::Red   }
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
	LkEngine::Core::Setup(Argc, Argv);

    LTestRunner TestRunner;
	LK_TEST_INFO("Test results are placed in: {}", LFileSystem::ConvertToUnixPath(TestRunner.ResultsDir));
	TestRunner.Run();

	LK_TEST_INFO_TAG("TestRunner", "Exiting");
    return 0;
}
