#include "TestRunner.h"
#include "TestLayer.h"

#include <LkEngine/Core/Application.h>
#include <LkEngine/Core/IO/FileSystem.h>

#include "AutomationTest/TestManager.h"

#if defined(LK_AUTOMATION_TEST_CORE)
#	include "AutomationTest/Suite/Core/ObjectPtrTests.h"
#	include "AutomationTest/Suite/Core/EnumTests.h"
#	include "AutomationTest/Suite/Core/VectorTests.h"
#endif

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
		/* StringUtils tests. */
		Tests.push_back(std::make_shared<StringUtils_ToLower_ConstChar>());
		Tests.push_back(std::make_shared<StringUtils_ToUpper_ConstChar>());
		Tests.push_back(std::make_shared<StringUtils_ToLower_String>());
		Tests.push_back(std::make_shared<StringUtils_ToUpper_String>());
		Tests.push_back(std::make_shared<StringUtils_ToLower_WideString>());
		Tests.push_back(std::make_shared<StringUtils_ToUpper_WideString>());
		/* Enum tests. */
		Tests.push_back(std::make_shared<Enum_EnumClass_Operator_Bitwise_AND>());
		Tests.push_back(std::make_shared<Enum_EnumClass_Operator_Bitwise_OR>());
		Tests.push_back(std::make_shared<Enum_EnumClass_Operator_Comparison_LessThan>());
		Tests.push_back(std::make_shared<Enum_EnumClass_Operator_Comparison_LessThanOrEqualTo>());
		Tests.push_back(std::make_shared<Enum_EnumClass_Operator_Comparison_GreaterThan>());
		Tests.push_back(std::make_shared<Enum_EnumClass_Operator_Comparison_GreaterThanOrEqualTo>());
		/* Math tests. */
		Tests.push_back(std::make_shared<Math_Vector2_Float_Compability_Glm_Copy>());
		Tests.push_back(std::make_shared<Math_Vector2_Float_Compability_Glm_CopyConstructor>());
		Tests.push_back(std::make_shared<Math_Vector3_Float_Compability_Glm_Copy>());
		Tests.push_back(std::make_shared<Math_Vector3_Float_Compability_Glm_CopyConstructor>());
		Tests.push_back(std::make_shared<Math_Vector4_Float_Compability_Glm_Copy>());
		Tests.push_back(std::make_shared<Math_Vector4_Float_Compability_Glm_CopyConstructor>());
		Tests.push_back(std::make_shared<Math_Vector2_Float_Compability_ImGui_Copy>());
		Tests.push_back(std::make_shared<Math_Vector2_Float_Compability_ImGui_CopyConstructor>());
		Tests.push_back(std::make_shared<Math_Vector4_Float_Compability_ImGui_Copy>());
		Tests.push_back(std::make_shared<Math_Vector4_Float_Compability_ImGui_CopyConstructor>());
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
	//LkEngine::Core::Setup(Argc, Argv); /* Does not work with the Github Action Runner for some reason. */

    LTestRunner TestRunner;
	TestRunner.RegisterTests();
	TestRunner.Run();
	LK_PRINTLN("Results are placed in: {}", (std::filesystem::current_path() / "Results").string());

    return 0;
}
