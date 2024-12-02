#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/LObject/Class.h"

#include "LkEngine/Test/TestMacros.h"
#include "LkEngine/Test/AutomationTest.h"


#define LK_TEST_SUITE_ALL     ::LkEngine::Test::ETestSuite::All
#define LK_TEST_SUITE_CORE    ::LkEngine::Test::ETestSuite::Core

/*------------------------------------------------------------------
	 Test Declarations. 
 -----------------------------------------------------------------*/

/* Dummy tests. */
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne, LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo, LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne, LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree, LK_TEST_SUITE_CORE);


/* Object Pointer tests. */
LK_DECLARE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation);

