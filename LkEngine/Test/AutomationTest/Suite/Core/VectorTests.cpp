#include "VectorTests.h"

#include "AutomationTest/TestManager.h"
#include "AutomationTest/DummyTestClass.h"

#include <LkEngine/Core/Log/Log.h>
#include <LkEngine/Core/Utility/StringUtils.h>
#include <LkEngine/Core/Math/MathLibrary.h>

#include <random>

using namespace LkEngine;

static_assert(std::is_same_v<LVector2, TVector2<float>>);
static_assert(std::is_same_v<LVector, TVector3<float>>);

namespace 
{
	constexpr float FLOAT_MIN = std::numeric_limits<float>::min();
	constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
}

static float GenerateRandomFloat(const float Min, const float Max)
{
    static std::random_device RandomDevice;
    static std::mt19937 Generator(RandomDevice());
    std::uniform_real_distribution<float> Distribution(Min, Max);
    return Distribution(Generator);
}

template<typename T>
static T GenerateRandomVector(const float Min = FLOAT_MIN, const float Max = FLOAT_MAX)
{
	static_assert(false, "Vector type not specified");
	return T();
}

template<>
static glm::vec2 GenerateRandomVector(const float Min, const float Max)
{
	return glm::vec2(GenerateRandomFloat(Min, Max), GenerateRandomFloat(Min, Max));
}

template<>
static glm::vec3 GenerateRandomVector(const float Min, const float Max)
{
    return glm::vec3(GenerateRandomFloat(Min, Max), GenerateRandomFloat(Min, Max), GenerateRandomFloat(Min, Max));
}

template<>
static ImVec2 GenerateRandomVector(const float Min, const float Max)
{
	return ImVec2(GenerateRandomFloat(Min, Max), GenerateRandomFloat(Min, Max));
}

template<>
static ImVec4 GenerateRandomVector(const float Min, const float Max)
{
	return ImVec4(GenerateRandomFloat(Min, Max), 
				  GenerateRandomFloat(Min, Max), 
				  GenerateRandomFloat(Min, Max), 
				  GenerateRandomFloat(Min, Max));
}


/*-------------------------------------------------------------------
  [glm] [LVector2 | TVector2<float>]
						       Copy 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Compability_Glm_Copy)
{
	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<glm::vec2>();
		LVector2 Vec2 = TestVec2;

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<glm::vec2>();
		LVector2 Vec2 = TestVec2;

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<glm::vec2>();
		LVector2 Vec2 = TestVec2;

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] [LVector2 | TVector2<float>]
						  Copy Constructor
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Compability_Glm_CopyConstructor)
{
	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<glm::vec2>();
		LVector2 Vec2(TestVec2);

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<glm::vec2>();
		LVector2 Vec2(TestVec2);

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<glm::vec2>();
		LVector2 Vec2(TestVec2);

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] [LVector3 | TVector3<float>]
						       Copy 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Compability_Glm_Copy)
{
	bool Test1 = false;
	{
		const glm::vec3 TestVec3 = GenerateRandomVector<glm::vec3>();
		LVector Vec3 = TestVec3;

		Test1 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const glm::vec3 TestVec3 = GenerateRandomVector<glm::vec3>();
		LVector Vec3 = TestVec3;

		Test2 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const glm::vec3 TestVec3 = GenerateRandomVector<glm::vec3>();
		LVector Vec3 = TestVec3;

		Test3 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] [LVector3 | TVector3<float>]
						  Copy Constructor
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Compability_Glm_CopyConstructor)
{
	bool Test1 = false;
	{
		const auto TestVec3 = GenerateRandomVector<glm::vec3>();
		LVector Vec3(TestVec3);

		Test1 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const auto TestVec3 = GenerateRandomVector<glm::vec3>();
		LVector Vec3(TestVec3);

		Test2 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const auto TestVec3 = GenerateRandomVector<glm::vec3>();
		LVector Vec3(TestVec3);

		Test3 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3);
}


/***************************************************************************************/


/*-------------------------------------------------------------------
  [ImGui] [LVector2 | TVector2<float>]
						       Copy
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Compability_ImGui_Copy)
{
	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<ImVec2>();
		LVector2 Vec2 = TestVec2;

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<ImVec2>();
		LVector2 Vec2 = TestVec2;

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<ImVec2>();
		LVector2 Vec2 = TestVec2;

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [ImGui] [LVector2 | TVector2<float>]
						   Copy Constructor
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Compability_ImGui_CopyConstructor)
{
	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<ImVec2>();
		LVector2 Vec2(TestVec2);

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<ImVec2>();
		LVector2 Vec2(TestVec2);

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<ImVec2>();
		LVector2 Vec2(TestVec2);

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
		if (!Test3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3);
}
