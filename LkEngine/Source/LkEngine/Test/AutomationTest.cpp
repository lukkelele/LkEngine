#include "LKpch.h"
#include "AutomationTest.h"

#include "TestManager.h"


namespace LkEngine::Test {

	int LAutomationTestBase::SuppressLogs = static_cast<int>(ELogLevel::Info);
	bool LAutomationTestBase::bElevateWarningsToErrors = false;

	LAutomationTestBase::LAutomationTestBase(const std::string& InName)
		: TestName(InName)
	{
		LK_CORE_VERIFY(!TestName.empty());
		LK_TEST_DEBUG_TAG("AutomationTest", "Registering: {}", TestName);
		const bool bTestRegistration = LTestManager::Get().RegisterAutomationTest(TestName, this);
		if (!bTestRegistration)
		{
			LK_CORE_ERROR("Failed to register '{}'", TestName);
		}
	}

	LAutomationTestBase::~LAutomationTestBase()
	{
		LK_TEST_TRACE_TAG("AutomationTest", "Destroying: {}", TestName);
		const bool bTestUnregistered = LTestManager::Get().UnregisterAutomationTest(TestName, this);
		if (!bTestUnregistered)
		{
			LK_CORE_ERROR("Test '{}' failed to unregister", TestName);
		}
	}

	void LAutomationTestBase::AddError(const std::string& ErrorMessage)
	{
		/* TODO: Add error event. */
		LK_TEST_TRACE_TAG(LK_FORMAT_STRING("AutomationTest::{}", GetName()), "Failed");
	}

	bool LAutomationTestBase::TestEqual(const int32_t Actual, const int32_t Expected, const char* Message)
	{
		if (Actual != Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestEqual(const int64_t Actual, const int64_t Expected, const char* Message)
	{
		if (Actual != Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestEqual(const std::size_t Actual, const std::size_t Expected, const char* Message)
	{
		if (Actual != Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestEqual(const float Actual, const float Expected, const char* Message)
	{
		if (Actual != Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterEqual(const int32_t Actual, const int32_t Expected, const char* Message)
	{
		if (Actual < Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterEqual(const int64_t Actual, const int64_t Expected, const char* Message)
	{
		if (Actual < Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterEqual(const std::size_t Actual, const std::size_t Expected, const char* Message)
	{
		if (Actual < Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterEqual(const float Actual, const float Expected, const char* Message)
	{
		if (Actual < Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterThan(const int32_t Actual, const int32_t Expected, const char* Message)
	{
		if (Actual <= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterThan(const int64_t Actual, const int64_t Expected, const char* Message)
	{
		if (Actual <= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterThan(const float Actual, const float Expected, const char* Message)
	{
		if (Actual <= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestGreaterThan(const std::size_t Actual, const std::size_t Expected, const char* Message)
	{
		if (Actual <= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestLessEqual(const int32_t Actual, const int32_t Expected, const char* Message)
	{
		if (Actual > Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestLessEqual(const int64_t Actual, const int64_t Expected, const char* Message)
	{
		if (Actual > Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestLessEqual(const float Actual, const float Expected, const char* Message)
	{
		if (Actual > Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestLessThan(const int32_t Actual, const int32_t Expected, const char* Message)
	{
		if (Actual >= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestLessThan(const int64_t Actual, const int64_t Expected, const char* Message)
	{
		if (Actual >= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}

	bool LAutomationTestBase::TestLessThan(const float Actual, const float Expected, const char* Message)
	{
		if (Actual >= Expected)
		{
			AddError(LK_FORMAT_STRING("Expected '{}' to be {}, but it was {}", Message, Expected, Actual));
			return false;
		}

		return true;
	}


}
