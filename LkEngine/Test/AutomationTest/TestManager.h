#pragma once

#include <LkEngine/Core/CoreTypes.h>
#include <LkEngine/Core/CoreMacros.h>
#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>

#include "AutomationTest.h"


namespace LkEngine {

	class LTestManager
	{
	public:
		LTestManager() = default;
		~LTestManager() = default;

		static LTestManager& Get();

		template<typename T>
		CORE_API bool DetectAutomationTest(LObject* InTestInstance)
		{
			using namespace Test;
			LK_CORE_ASSERT(InTestInstance);
			const LClass* Class = InTestInstance->GetClass();
			LK_CORE_ASSERT(Class);
			const std::string ClassName = Class->GetName();
			LK_PRINT("DetectAutomationTest: {}", ClassName.c_str());

			LAutomationTestBase* TestInstance = static_cast<LAutomationTestBase*>(InTestInstance);
			const ETestSuite TestSuite = TestInstance->GetTestSuite();
			LK_PRINT("  Suite: ({}, ", Enum::ToString(TestSuite));

			if (TestSuiteMap.contains(TestInstance->GetTestSuite()))
			{
				auto& TestCollection = TestSuiteMap[TestSuite];
				TestCollection.push_back(TestInstance);
			}
			else
			{
				TestSuiteMap[TestSuite] = { TestInstance };
			}

			LK_PRINTLN("{}) ", TestSuiteMap[TestSuite].size());

			return true;
		}

		CORE_API bool RegisterAutomationTest(const std::string& TestName, Test::LAutomationTestBase* InTestInstance);
		CORE_API bool UnregisterAutomationTest(const std::string& TestName, Test::LAutomationTestBase* InTestInstance);

		CORE_API void RunTests(const std::string& TestSuite = "All");

		/**
		 * @brief Start a test.
		 */
		CORE_API bool StartTest(const std::string& TestName);

		/**
		 * @brief Stop a test if it is running.
		 */
		CORE_API bool StopTest(const std::string& TestName);

		/**
		 * @brief Get the currently running test.
		 */
		Test::LAutomationTestBase* GetCurrentTest() const
		{
			return CurrentTest;
		}

		FORCEINLINE CORE_API void SetCaptureStack(const bool Enabled)
		{
			bCaptureStack = Enabled;
		}

	public:
		/**
		 * @brief Test instances.
		 */
		std::unordered_map<std::string, Test::LAutomationTestBase*> TestInstanceMap{};

		/**
		 * @brief 
		 */
		std::unordered_map<Test::ETestSuite, std::vector<Test::LAutomationTestBase*>> TestSuiteMap{};

	private:
		CORE_API static bool bCaptureStack;

		Test::LAutomationTestBase* CurrentTest = nullptr;
	};

}
