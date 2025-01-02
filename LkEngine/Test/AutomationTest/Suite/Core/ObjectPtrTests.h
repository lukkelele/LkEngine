#pragma once

#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>
#include <LkEngine/Core/LObject/Class.h>

#include "AutomationTest/TestMacros.h"
#include "AutomationTest/AutomationTest.h"


/*------------------------------------------------------------------
	 Dummy Tests.
 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne,         LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo,      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree, LK_TEST_SUITE_CORE);


/*------------------------------------------------------------------
   Core Tests
 -----------------------------------------------------------------*/
#if defined(LK_AUTOMATION_TEST_CORE)
/*------------------------------------------------------------------
	 Object Pointer Tests.

	 Reference counting.
 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation,  LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce,  LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyTwice, LK_TEST_SUITE_CORE);

LK_DECLARE_AUTOMATION_TEST(StringUtils_ToLower_ConstChar,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(StringUtils_ToUpper_ConstChar,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(StringUtils_ToLower_String,      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(StringUtils_ToUpper_String,      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(StringUtils_ToLower_WideString,  LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(StringUtils_ToUpper_WideString,  LK_TEST_SUITE_CORE);

#endif

