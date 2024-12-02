/******************************************************************
 * TestMacros
 *
/*****************************************************************/
#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log.h"

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


#if 0

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
    ::LkEngine::Test::ETestSuite TestClass::GetTestSuite() const  \
	{                                                             \
		using namespace ::LkEngine::Test;                         \
		return __VA_OPT__(static_cast<ETestSuite>(__VA_ARGS__))   \
			   __VA_OPT__(;) ETestSuite::Unknown;                 \
	}                                                             \
    bool TestClass::RunTest()


//---------------------------------------------------------------

/**
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass, ...)                 \
    TestClass::TestClass()                                        \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass) {}  \
    TestNS::ETestSuite GetTestSuite() const             \
	{                                                             \
		return __VA_OPTS__(static_cast<TestNS::ETestSuite>(__VA_ARGS__))  \
			   __VA_OPTS__(;) TestNS::ETestSuite::Unknown;                \
	} \
    bool TestClass::RunTest()

//---------------------------------------

/**
 * Declare an Automation Test.
 */
#define LK_DECLARE_AUTOMATION_TEST(TestClass, ...)                 \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase \
    {                                                              \
    public:                                                        \
        TestClass();                                               \
        virtual bool RunTest() override;                           \
        virtual ETestSuite GetTestSuite() const override;          \
    };


#endif


namespace LkEngine::Enum 
{
	static constexpr const char* ToString(const ::LkEngine::Test::ETestSuite TestSuite)
	{
		using namespace LkEngine::Test;
		switch (TestSuite)
		{
			case ETestSuite::Core:     return "Core";
			case ETestSuite::All:      return "All";
			case ETestSuite::Unknown:  return "Unknown";
			case ETestSuite::COUNT:    return "COUNT";
		}

		LK_CORE_ASSERT(false, "Enum::ToString failed");
		return nullptr;
	}

}
