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

		/**
		 * Register a test.
		 * Invoked in the LAutomationTestBase constructor.
		 */
		CORE_API bool RegisterAutomationTest(const std::string& TestName, Test::LAutomationTestBase* InTestInstance);

		/**
		 * Unregister a test.
		 * Invoked in the LAutomationTestBase destructor.
		 */
		CORE_API bool UnregisterAutomationTest(const std::string& TestName, Test::LAutomationTestBase* InTestInstance);

		CORE_API void RunTests(const Test::ETestSuite Suite = Test::ETestSuite::All) const;

		/** 
		 * Start a test. 
		 */
		CORE_API bool StartTest(const std::string& TestName);

		/** 
		 * Stop a test if it is running. 
		 */
		CORE_API bool StopTest(const std::string& TestName);

		/**
		 * Get the currently running test.
		 */
		Test::LAutomationTestBase* GetCurrentTest() const
		{
			return CurrentTest;
		}

		FORCEINLINE CORE_API void SetCaptureStack(const bool Enabled)
		{
			bCaptureStack = Enabled;
		}

		/**
		 * Function to create a test instance.
		 * Used in the test macros to register tests statically.
		 */
		using FTestCreator = std::function<std::shared_ptr<Test::LAutomationTestBase>()>;

		/**
		 * Add a test creator to a suite.
		 * The test creator function is what creates the test instance that is evaluated.
		 */
		void AddToSuite(const Test::ETestSuite Suite, FTestCreator CreatorFunc)
        {
			/* TODO: Cannot use log macros here because the registration takes place before the logger is instantiated.
			 *       Needs to be fixed. */
			//LK_PRINTLN("[TestManager] Registering test {} to suite '{}'", TestCreatorMap[Suite].size() + 1, Enum::ToString(Suite));
            TestCreatorMap[Suite].push_back(std::move(CreatorFunc));
        }

        const std::vector<FTestCreator>& GetTests(const Test::ETestSuite Suite) const
        {
            static const std::vector<FTestCreator> EmptySuite;
            auto Iter = TestCreatorMap.find(Suite);
            return (Iter != TestCreatorMap.end()) ? Iter->second : EmptySuite;
        }

	public:
		/**
		 * @brief Test instances.
		 */
		std::unordered_map<std::string, Test::LAutomationTestBase*> TestInstanceMap{};

		/* TODO: REMOVE */
		std::unordered_map<Test::ETestSuite, std::vector<Test::LAutomationTestBase*>> TestSuiteMap{};

		/**
		 * Test creator functions mapped to their corresponding test suites.
		 */
		std::unordered_map<Test::ETestSuite, std::vector<FTestCreator>> TestCreatorMap;

	private:
		CORE_API static bool bCaptureStack;

		Test::LAutomationTestBase* CurrentTest = nullptr;
	};

}
