#include "TestManager.h"

#include <LkEngine/Core/Core.h>
#include <LkEngine/Core/Globals.h>
#include <LkEngine/Core/IO/FileSystem.h>
#include <LkEngine/Serialization/FileStream.h>
#include <LkEngine/Serialization/YAMLSerialization.h>


namespace LkEngine {

	using namespace Test;

	bool LTestManager::bCaptureStack = true;

	LTestManager& LTestManager::Get()
	{
		static LTestManager TestManager;
		return TestManager;
	}

	bool LTestManager::RegisterAutomationTest(const std::string& TestName, LAutomationTestBase* InTestInstance)
	{
		LK_CORE_VERIFY(!TestName.empty(), "Test name is empty");
		LK_CORE_VERIFY(InTestInstance);

		LK_TEST_TRACE_TAG("TestManager", "Registering test: {}", TestName);
		/* FIXME: The call to 'GetTestSuite' causes crashes but the instance is OK, need to investigate further. */

		const bool bTestRegistered = TestInstanceMap.contains(TestName);
		if (!bTestRegistered)
		{
			TestInstanceMap.insert({ TestName, InTestInstance });
		}
		else
		{
			LK_TEST_WARN_TAG("TestManager", "Test '{}' already registered", TestName);
		}

		return !bTestRegistered;
	}

	bool LTestManager::UnregisterAutomationTest(const std::string& TestName, LAutomationTestBase* InTestInstance)
	{
		LK_CORE_VERIFY(!TestName.empty(), "Test name is empty");
		auto Iter = TestInstanceMap.find(TestName); 
		const bool bTestRegistered = (Iter != TestInstanceMap.end());
		if (bTestRegistered)
		{
			LK_TEST_TRACE_TAG("TestRunner", "Unregistering: {}", TestName);
			TestInstanceMap.erase(Iter);
		}

		return bTestRegistered;
	}

	void LTestManager::RunTests(const Test::ETestSuite Suite) const
	{
		const std::vector<FTestCreator>& Tests = GetTests(Suite);
		if (Tests.empty())
		{
			LK_TEST_ERROR_TAG("TestManager", "No tests registered for '{}'", Enum::ToString(Suite));
			return;
		}

		LK_TEST_INFO_TAG("TestManager", "Running {} tests for: {}", Tests.size(), Enum::ToString(Suite));
		const FRuntimeArguments& RuntimeArgs = Global::GetRuntimeArguments();
		const std::string ResultFile = std::format("TestResult-{}.yaml", Enum::ToString(Suite)); 

		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << Enum::ToString(Suite) << YAML::Value << YAML::BeginSeq;

		for (const auto& TestCreator : Tests)
		{
			std::shared_ptr<Test::LAutomationTestBase> TestInstance = TestCreator();
			LK_CORE_ASSERT(TestInstance);
			const std::string TestName = TestInstance->GetName();

			/* Run the test. */
			LK_TEST_TRACE_TAG("TestManager", "Running: {} ({})", TestName, Enum::ToString(TestInstance->GetTestSuite()));
			const bool bTestResult = TestInstance->RunTest();
			if (bTestResult)
			{
				LK_TEST_SUCCESS(LK_ANSI_COLOR_BRIGHT_GREEN "{}: OK", TestName);
			}
			else
			{
				LK_TEST_FAIL("{}: Failed", TestName);
			}

			/* Store test result in format: [ TestName: Result ] */
			Out << YAML::BeginMap;
			Out << YAML::Key << TestName << YAML::Value << (bTestResult ? 1 : 0);
			Out << YAML::EndMap;
		}

		Out << YAML::EndSeq;
		Out << YAML::EndMap;

		const std::string ResultDir = LFileSystem::GetBinaryDir().string() + "Results";
		LK_TEST_TRACE_TAG("TestManager", "Results directory: {}", ResultDir);
		if (!LFileSystem::Exists(ResultDir))
		{
			LFileSystem::CreateDirectory(ResultDir);
		}

		const std::string FileOutPath = std::format("{}/{}", ResultDir, ResultFile);
		LK_TEST_INFO("Saving test results: {}", FileOutPath);
		std::ofstream FileOut(FileOutPath);
		if (FileOut.is_open() && FileOut.good())
		{
			FileOut << Out.c_str();
		}
	}

	bool LTestManager::StartTest(const std::string& TestName)
	{
		LK_CORE_VERIFY(!TestName.empty(), "Test name is empty");
		auto Iter = TestInstanceMap.find(TestName); 
		if (Iter != TestInstanceMap.end())
		{
			CurrentTest = Iter->second;
			LK_CORE_ASSERT(CurrentTest);
			const bool bTestResult = CurrentTest->RunTest();
			if (bTestResult)
			{
				LK_TEST_SUCCESS_TAG("Test", "Result: OK");
			}
			else
			{
				LK_TEST_FAIL("Result: Failed");
			}
		}
		else
		{
			/* Test does not exist. */
			LK_TEST_ERROR_TAG("TestManager", "Failed to start {}, could not find any registered tests with that name", TestName);
		}

		return false;
	}

	bool LTestManager::StopTest(const std::string& TestName)
	{
		return true;
	}

}
