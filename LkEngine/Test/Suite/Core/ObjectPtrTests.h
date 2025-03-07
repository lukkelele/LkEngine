#pragma once

#include "TestBase.h"

/*=================================================================
                        ObjectPtr Tests
 =================================================================*/

/*------------------------------------------------------------------
	 Dummy Tests.
 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne,         LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo,      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree, LK_TEST_SUITE_CORE);


/*=================================================================
                        ObjectPtr Tests
 =================================================================*/
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_Operator_Copy,                     LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation,           LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyConstructor,    LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce,           LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyTwice,          LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_MoveConstructor,    LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_Function_PassedAsCopy,             LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_Function_PassedAsReference,        LK_TEST_SUITE_CORE);

