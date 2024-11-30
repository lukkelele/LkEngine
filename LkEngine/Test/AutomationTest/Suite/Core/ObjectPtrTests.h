#pragma once

#include "LkEngine/Test/TestMacros.h"
#include "LkEngine/Test/AutomationTest.h"


/*------------------------------------------------------------------
	 Test Declarations. 
 -----------------------------------------------------------------*/


/* Dummy tests. */
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne);

/* Object Pointer tests. */
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation);

