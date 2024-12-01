#include "TestRunner.h"
#include "TestLayer.h"

#include "LkEngine/Test/TestManager.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include "AutomationTest/Suite/Core/ObjectPtrTests.h"


namespace LkEngine {

	LTestRunner::LTestRunner()
	{
		LLog& Logger = LLog::Instance();

		/* Create 'Results' directory to store test results in if it does not already exist. */
		if (!LFileSystem::Exists("Results"))
		{
			LFileSystem::CreateDirectory("Results");
		}

		/* Register test logger. */
		std::vector<LogLevelColorConfig> LevelConfigs = {
			{ ELogLevel::Debug, Color::Cyan  },
			{ ELogLevel::Info,  Color::White },
			{ ELogLevel::Error,	Color::Red   }
		};
	
		Logger.RegisterLogger(ELoggerType::TestRunner, "TEST", ELogLevel::Debug, LevelConfigs);
	}

	void LTestRunner::RegisterTests()
	{
		/* Dummy tests. */
		Tests.push_back(std::make_shared<Dummy_Arithmetic_OneEqualOne>());
		Tests.push_back(std::make_shared<Dummy_Arithmetic_OneLessThanTwo>());
		Tests.push_back(std::make_shared<Dummy_Arithmetic_TwoGreaterThanOne>());
	}

	void LTestRunner::Run()
	{
		LTestManager::Get().RunTests();
	}

}


int main(int Argc, char* Argv[])
{
	std::printf("Argc=%d\n", Argc);
	for (int i = 0; i < Argc; i++)
	{
		std::printf("Argv %d: %s\n", i, Argv[i]);
	}
	std::printf("\n");

	using namespace LkEngine;
	Global::SetRuntimeArguments(Argc, Argv);

    LTestRunner TestRunner;
	TestRunner.RegisterTests();
	TestRunner.Run();

    return 0;
}
