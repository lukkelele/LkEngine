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
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);
/* TODO: Write tests for glm::vec4 when LVector4 is available. */
//LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
//LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);

/*-----------------------------------------------------------------
     Compability: ImGui
 -----------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Compability_ImGui_Copy,            LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Compability_ImGui_CopyConstructor, LK_TEST_SUITE_CORE);
/* TODO: Write tests for ImVec4 when LVector4 is available. */
//LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Compability_ImGui_Copy,            LK_TEST_SUITE_CORE);
//LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Compability_ImGui_CopyConstructor, LK_TEST_SUITE_CORE);
