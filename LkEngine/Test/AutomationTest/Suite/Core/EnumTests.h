#pragma once

#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>
#include <LkEngine/Core/LObject/Class.h>

#include "AutomationTest/TestMacros.h"
#include "AutomationTest/AutomationTest.h"

/* Required because of the static test registration in LK_DECLARE_AUTOMATION_TEST. */
#include "AutomationTest/TestManager.h"


/*=================================================================
                           Enum Tests
 =================================================================*/ 
LK_DECLARE_AUTOMATION_TEST(Enum_EnumClass_Operator_Bitwise_AND,                     LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Enum_EnumClass_Operator_Bitwise_OR,                      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_LessThan,             LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_LessThanOrEqualTo,    LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_GreaterThan,          LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_GreaterThanOrEqualTo, LK_TEST_SUITE_CORE);
