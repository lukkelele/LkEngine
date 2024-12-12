/******************************************************************
 * TestMacros
 *
/*****************************************************************/
#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log/Log.h"

#define LK_TEST_SUITE_ALL     ::LkEngine::Test::ETestSuite::All
#define LK_TEST_SUITE_CORE    ::LkEngine::Test::ETestSuite::Core

namespace LkEngine::Test 
{
	enum class ETestSuite
	{
		Unknown,
		All,
		Core,
		COUNT
	};
}

/**
 * Declare an Automation Test.
 */
#define LK_DECLARE_AUTOMATION_TEST(TestClass, ...)                  \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase  \
    {                                                               \
    public:                                                         \
        TestClass();                                                \
        virtual bool RunTest() override;                            \
		::LkEngine::Test::ETestSuite GetTestSuite() const           \
		{                                                           \
			using namespace ::LkEngine::Test;                       \
			return __VA_OPT__(static_cast<ETestSuite>(__VA_ARGS__)) \
				   __VA_OPT__(;) ETestSuite::Unknown;               \
		}                                                           \
	private:                                                        \
		LCLASS(TestClass);                                          \
    };


/**
 * TODO: Should be able to wrap a try-catch around the test definition and
 *       report an error and return false for tests that fail miserably.
 *       So that the test code can fail internally and still run and report errors.
 */
/**
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass, ...)                 \
    TestClass::TestClass()                                        \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass)     \
	{ \
		LCLASS_REGISTER(); \
		::LkEngine::LTestManager::Get().DetectAutomationTest<TestClass>(this); \
	} \
    bool TestClass::RunTest()



namespace LkEngine::Enum 
{
	static constexpr const char* ToString(const ::LkEngine::Test::ETestSuite TestSuite)
	{
		using namespace LkEngine::Test;
		switch (TestSuite)
		{
			case ETestSuite::Unknown:  return "Unknown";
			case ETestSuite::All:      return "All";
			case ETestSuite::Core:     return "Core";
			case ETestSuite::COUNT:    return "COUNT";
		}

		LK_CORE_ASSERT(false, "Enum::ToString failed");
		return nullptr;
	}

}
