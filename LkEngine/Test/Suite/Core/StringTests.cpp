#include "StringTests.h"

#include <LkEngine/Core/Utility/StringUtils.h>


using namespace LkEngine;

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

