/******************************************************************
 * TestMacros
 *
/*****************************************************************/
#pragma once

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

namespace TestNamespace = ::LkEngine::Test;

/**
 * Declare an Automation Test.
 */
#define LK_DECLARE_AUTOMATION_TEST(TestClass, ...)                 \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase \
    {                                                              \
    public:                                                        \
        TestClass();                                               \
        virtual bool RunTest() override;                           \
        virtual ::LkEngine::Test::ETestSuite GetTestSuite() const override; \
    };


/**
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass, ...)                 \
    TestClass::TestClass()                                        \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass) {}  \
    ::LkEngine::Test::ETestSuite TestClass::GetTestSuite() const  \
	{                                                             \
		using namespace ::LkEngine::Test;                         \
		return __VA_OPT__(static_cast<ETestSuite>(__VA_ARGS__))   \
			   __VA_OPT__(;) ETestSuite::Unknown;                 \
	}                                                             \
    bool TestClass::RunTest()


#if 0

/**
 * Declare an Automation Test.
 */
#define LK_DECLARE_AUTOMATION_TEST(TestClass, ...)                 \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase \
    {                                                              \
		namespace TestNS = ::LkEngine::Test;                       \
    public:                                                        \
        TestClass();                                               \
        virtual bool RunTest() override;                           \
        virtual TestNS::ETestSuite GetTestSuite() const override; \
    };

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
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass, ...)                 \
    TestClass::TestClass()                                        \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass) {}  \
    ::LkEngine::Test::ETestSuite GetTestSuite() const             \
	{                                                             \
		return __VA_OPTS__(static_cast<::LkEngine::Test::ETestSuite>(__VA_ARGS__))  \
			   __VA_OPTS__(;) ::LkEngine::Test::ETestSuite::Unknown;                \
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


/**
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass, ...)                 \
    TestClass::TestClass()                                        \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass) {}  \
    ETestSuite GetTestSuite() const                               \
	{                                                             \
		return __VA_OPTS__(static_cast<ETestSuite>(__VA_ARGS__))  \
			   __VA_OPTS__(;) ETestSuite::Unknown;                \
	} \
    bool TestClass::RunTest()

/**
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass)                       \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase \
    {                                                              \
    public:                                                        \
        TestClass() : ::LkEngine::Test::LAutomationTestBase(#TestClass) {}   \
        virtual bool RunTest() override;                           \
    };                                                             \
    bool TestClass::RunTest()

#endif
