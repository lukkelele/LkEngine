#include "ObjectPtrTests.h"

#include "AutomationTest/TestManager.h"
#include "AutomationTest/DummyTestClass.h"

#include <LkEngine/Core/Utility/StringUtils.h>

using namespace LkEngine;
using namespace LkEngine::Test;


/*=================================================================
                           Core Tests

 =================================================================*/ 


LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne) 
{
	return TestEqual(1, 1, GetName());
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo) 
{
	return TestEqual((1 < 2), true, GetName());
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne) 
{
	return TestEqual((2 > 1), true, GetName());
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree) 
{
	return TestLessThan(2, 3, GetName());
};


/*--------------------------------------------------
	 Object Pointer Tests.
 --------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation) 
{
	using namespace LkEngine::Test;
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	return TestEqual(TestObject->GetReferenceCount(), 1, GetName());
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	return TestEqual(TestObject->GetReferenceCount(), 2, GetName());
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyTwice) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	TObjectPtr<LObject> SecondCopiedObject(CopiedObject);
	return TestEqual(TestObject->GetReferenceCount(), 3, GetName());
};

/*--------------------------------------------------
	 StringUtils Tests.
 --------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(StringUtils_ToLower_ConstChar) 
{
	constexpr const char* TestString = "TEST_STRING";
	constexpr const char* Expected = "test_string";
	const std::string Result = StringUtils::ToLower(TestString);
	return (Result == Expected);
};

LK_DEFINE_AUTOMATION_TEST(StringUtils_ToUpper_ConstChar) 
{
	constexpr const char* TestString = "test_string";
	constexpr const char* Expected = "TEST_STRING";
	const std::string Result = StringUtils::ToUpper(TestString);
	return (Result == Expected);
};

LK_DEFINE_AUTOMATION_TEST(StringUtils_ToLower_String) 
{
	const std::string TestString("TEST_STRING");
	const std::string Expected("test_string");
	const std::string Result = StringUtils::ToLower(TestString);
	return (Result == Expected);
};

LK_DEFINE_AUTOMATION_TEST(StringUtils_ToUpper_String) 
{
	const std::string TestString("test_string");
	const std::string Expected("TEST_STRING");
	const std::string Result = StringUtils::ToUpper(TestString);
	return (Result == Expected);
};

LK_DEFINE_AUTOMATION_TEST(StringUtils_ToLower_WideString) 
{
	const std::wstring TestString(L"TEST_STRING");
	const std::wstring Expected(L"test_string");
	const std::wstring Result = StringUtils::ToLower(TestString);
	return (Result == Expected);
};

LK_DEFINE_AUTOMATION_TEST(StringUtils_ToUpper_WideString) 
{
	const std::wstring TestString(L"test_string");
	const std::wstring Expected(L"TEST_STRING");
	const std::wstring Result = StringUtils::ToUpper(TestString);
	return (Result == Expected);
};
