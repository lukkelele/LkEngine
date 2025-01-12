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


/**
 * LK_DECLARE_AUTOMATION_TEST
 * 
 *  Declare an automation test.
 * 
 */
#define LK_DECLARE_AUTOMATION_TEST(TestClass, ...)                         \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase         \
    {                                                                      \
    public:                                                                \
        TestClass();                                                       \
        virtual bool RunTest() override;                                   \
		virtual ::LkEngine::Test::ETestSuite GetTestSuite() const override \
		{                                                                  \
			using namespace ::LkEngine::Test;                              \
			return __VA_OPT__(static_cast<ETestSuite>(__VA_ARGS__)) __VA_OPT__(;) ETestSuite::Unknown; \
		}                                                                  \
	private:                                                               \
		LCLASS(TestClass);                                                 \
    };                                                                     \
    namespace                                                                           \
    {                                                                                   \
        struct TestClass##_Register                                                     \
        {                                                                               \
            TestClass##_Register()                                                      \
            {                                                                           \
				using namespace ::LkEngine::Test;                                       \
                ::LkEngine::LTestManager::Get().AddToSuite(                             \
                    __VA_OPT__(static_cast<::LkEngine::Test::ETestSuite>(__VA_ARGS__))  \
					__VA_OPT__(,)                                                       \
                    []() { return std::make_shared<TestClass>(); });                    \
            }                                                                           \
        };                                                                              \
        static TestClass##_Register TestClass##_Instance;                               \
    }


/**
 * TODO: Should be able to wrap a try-catch around the test definition and
 *       report an error and return false for tests that fail miserably.
 *       So that the test code can fail internally and still run and report errors.
 */
/**
 * LK_DEFINE_AUTOMATION_TEST
 *
 *  Define an automation test by providing the test function.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass, ...)                \
    TestClass::TestClass()                                       \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass)    \
	{                                                            \
		LOBJECT_REGISTER();                                      \
	}                                                            \
    bool TestClass::RunTest()


