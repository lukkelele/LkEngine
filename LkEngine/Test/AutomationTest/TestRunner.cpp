#include "TestRunner.h"
#include "TestLayer.h"

#include "LkEngine/Test/TestManager.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include "AutomationTest/Suite/Core/ObjectPtrTests.h"


namespace LkEngine {

	LTestRunner::LTestRunner()
	{
		LLog& Logger = LLog::Get();
		Logger.Initialize();

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
		Tests.push_back(std::make_shared<Dummy_Arithmetic_TwoGreaterThanThree>());

	#if defined(LK_AUTOMATION_TEST_CORE)
		/* Object Pointer tests. */
		Tests.push_back(std::make_shared<ObjectPtr_ReferenceCount_Creation>());
		Tests.push_back(std::make_shared<ObjectPtr_ReferenceCount_CopyOnce>());
		Tests.push_back(std::make_shared<ObjectPtr_ReferenceCount_CopyTwice>());
	#endif
	}

	void LTestRunner::Run()
	{
		LTestManager::Get().RunTests("CoreTests");
	}

}


int main(int Argc, char* Argv[])
{
	using namespace LkEngine;
	Global::SetRuntimeArguments(Argc, Argv);

    LTestRunner TestRunner;
	TestRunner.RegisterTests();
	TestRunner.Run();

    return 0;
}
