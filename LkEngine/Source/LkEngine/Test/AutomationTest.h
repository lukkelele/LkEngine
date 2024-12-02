#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Log.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "TestMacros.h"


namespace LkEngine::Test {

	struct FTestExecutionEntry
	{
		std::string Filename{};
		int LineNumber = -1;
	};

	class LTestExecutionData
	{
	public:
		LTestExecutionData() = default;
		~LTestExecutionData() = default;

		FORCEINLINE void PushContext(const std::string& Context)
		{
			ContextStack.push_front(Context);
		}

		FORCEINLINE void PopContext()
		{
			ContextStack.pop_front();
		}

	public:
		bool bSuccessful = false;
	private:
		int Warnings = 0;
		int Errors = 0;
		std::deque<std::string> ContextStack{};

		/**
		 * @brief Errors that occured during test execution.
		 */
		std::deque<FTestExecutionEntry> Entries{};
	};

	/**
	 * @class LAutomationTestBase
	 */
	class LAutomationTestBase : public LObject
	{
	public:
		explicit LAutomationTestBase(const std::string& InName);
		virtual ~LAutomationTestBase();

		/**
		 * @brief Run the test.
		 */
		virtual bool RunTest() = 0;

		/**
		 * @brief Get the test suite.
		 */
		virtual ETestSuite GetTestSuite() const = 0;

	public:
		bool TestEqual(const int32_t Actual, const int32_t Expected, const char* Message);
		bool TestEqual(const int64_t Actual, const int64_t Expected, const char* Message);
		bool TestEqual(const std::size_t Actual, const std::size_t Expected, const char* Message);
		bool TestEqual(const float Actual, const float Expected, const char* Message);
		bool TestGreaterEqual(const int32_t Actual, const int32_t Expected, const char* Message);
		bool TestGreaterEqual(const int64_t Actual, const int64_t Expected, const char* Message);
		bool TestGreaterEqual(const std::size_t Actual, const std::size_t Expected, const char* Message);
		bool TestGreaterEqual(const float Actual, const float Expected, const char* Message);
		bool TestGreaterThan(const int32_t Actual, const int32_t Expected, const char* Message);
		bool TestGreaterThan(const int64_t Actual, const int64_t Expected, const char* Message);
		bool TestGreaterThan(const float Actual, const float Expected, const char* Message);
		bool TestGreaterThan(const std::size_t Actual, const std::size_t Expected, const char* Message);
		bool TestLessEqual(const int32_t Actual, const int32_t Expected, const char* Message);
		bool TestLessEqual(const int64_t Actual, const int64_t Expected, const char* Message);
		bool TestLessEqual(const float Actual, const float Expected, const char* Message);
		bool TestLessThan(const int32_t Actual, const int32_t Expected, const char* Message);
		bool TestLessThan(const int64_t Actual, const int64_t Expected, const char* Message);
		bool TestLessThan(const float Actual, const float Expected, const char* Message);

		void AddError(const std::string& ErrorMessage);

		const char* GetName() const { return TestName.c_str(); }

	public:
		/** @variable Suppress logs below a certain log level. */
		static int SuppressLogs;
		static bool bElevateWarningsToErrors;
	protected:
		LTestExecutionData ExecutionData{};
		std::string TestName{};
	private:
		LCLASS(LAutomationTestBase);
	};


}
