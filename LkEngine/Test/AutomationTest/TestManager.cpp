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
		const bool bTestRegistered = TestInstanceMap.contains(TestName);
		if (!bTestRegistered)
		{
			TestInstanceMap.insert({ TestName, InTestInstance });
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

	void LTestManager::RunTests(const std::string& TestSuite)
	{
		LK_CORE_ASSERT(!TestSuite.empty(), "TestSuite is empty");
		const FRuntimeArguments& RuntimeArgs = Global::GetRuntimeArguments();

		const std::string ResultFile = LK_FORMAT_STRING("TestResult-{}.yaml", TestSuite); 

		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << TestSuite << YAML::Value << YAML::BeginSeq;

		for (const auto& [TestName, TestInstance] : TestInstanceMap)
		{
			LK_CORE_ASSERT(TestInstance, "Invalid test instance");
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
		LK_TEST_DEBUG("Results dir: {}", ResultDir);
		if (!LFileSystem::Exists(ResultDir))
		{
			LFileSystem::CreateDirectory(ResultDir);
		}
		const std::string FileOutPath = LK_FORMAT_STRING("{}/{}", ResultDir, ResultFile);
		LK_TEST_INFO("Saving test results ({})", FileOutPath);
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
			LK_CORE_ERROR_TAG("TestManager", "Failed to start {}, could not find any registered tests with that name", TestName);
		}

		return false;
	}

	bool LTestManager::StopTest(const std::string& TestName)
	{
		return true;
	}

}
