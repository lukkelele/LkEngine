#pragma once

#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>
#include <LkEngine/Core/LObject/Class.h>

#include "AutomationTest/TestMacros.h"
#include "AutomationTest/AutomationTest.h"


/*=================================================================
                         Vector Tests
 =================================================================*/ 

/*-----------------------------------------------------------------
     Compability: glm
 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Float_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Float_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);

/*-----------------------------------------------------------------
     Compability: ImGui
 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_ImGui_Copy,            LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_ImGui_CopyConstructor, LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_ImGui_Copy,            LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_ImGui_CopyConstructor, LK_TEST_SUITE_CORE);
