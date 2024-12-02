#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/LObject/Class.h"

#include "LkEngine/Test/TestMacros.h"
#include "LkEngine/Test/AutomationTest.h"


/*------------------------------------------------------------------
	 Dummy Tests.

 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne,         LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo,      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree, LK_TEST_SUITE_CORE);


/*------------------------------------------------------------------
	 Object Pointer Tests.

	 Reference counting.
 -----------------------------------------------------------------*/
#if defined(LK_AUTOMATION_TEST_CORE)

LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation,  LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce,  LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyTwice, LK_TEST_SUITE_CORE);

#endif

