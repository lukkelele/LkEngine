#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	namespace Test 
	{
		class LAutomationTestBase;
	}

	class LTestManager
	{
	public:
		LTestManager() = default;
		~LTestManager() = default;

		static LTestManager& Get();

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

	private:
		CORE_API static bool bCaptureStack;

		Test::LAutomationTestBase* CurrentTest = nullptr;
	};

}
