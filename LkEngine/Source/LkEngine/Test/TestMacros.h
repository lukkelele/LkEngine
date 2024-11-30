/******************************************************************
 *
 *
/*****************************************************************/
#pragma once


/**
 * Declare an Automation Test.
 */
#define LK_DECLARE_AUTOMATION_TEST(TestClass) \
    class TestClass : public ::LkEngine::Test::LAutomationTestBase \
    {                                                              \
    public:                                                        \
        TestClass();                                               \
        virtual bool RunTest() override;                           \
    };

/**
 * Define an Automation Test.
 */
#define LK_DEFINE_AUTOMATION_TEST(TestClass)                       \
    TestClass::TestClass()                                         \
          : ::LkEngine::Test::LAutomationTestBase(#TestClass) {}   \
    bool TestClass::RunTest()


#if 0

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
