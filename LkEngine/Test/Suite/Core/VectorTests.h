#pragma once

#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>
#include <LkEngine/Core/LObject/Class.h>

#include "LTesto/Core/TestMacros.h"
#include "LTesto/Core/AutomationTest.h"

/* Required because of the static test registration in LK_DECLARE_AUTOMATION_TEST. */
#include "LTesto/Runtime/TestManager.h"

/*=================================================================
                         Vector Tests
 =================================================================*/ 

/*-------------------------------------------------------------
   Operations: TVector3<float>
 -------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Add,                LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Subtract,           LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Min,                LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Max,                LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Dot,                LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Cross,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Normalize,          LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_NormalizeSafe,      LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_Distance,           LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Operation_DistanceSquared,    LK_TEST_SUITE_CORE);


/*-------------------------------------------------------------
   Compability: glm
 -------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Float_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector3_Float_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_Glm_Copy,              LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_Glm_CopyConstructor,   LK_TEST_SUITE_CORE);


/*-------------------------------------------------------------
   Compability: ImGui
 -------------------------------------------------------------*/
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_ImGui_Copy,            LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector2_Float_Compability_ImGui_CopyConstructor, LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_ImGui_Copy,            LK_TEST_SUITE_CORE);
LK_DECLARE_AUTOMATION_TEST(Math_Vector4_Float_Compability_ImGui_CopyConstructor, LK_TEST_SUITE_CORE);

