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
	constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
	constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

	/* Min and max values for mathematical operations that might cause overflows otherwise. */
    const float SAFE_FLOAT_MAX = std::sqrt(FLOAT_MAX / 3.0f);
	const float SAFE_FLOAT_MIN = -SAFE_FLOAT_MAX;

	constexpr float Epsilon = 1e-5f;
}

static float GenerateRandomFloat(const float Min, const float Max)
{
    static std::random_device RandomDevice;
    static std::mt19937 Generator(RandomDevice());

    std::uniform_real_distribution<float> Distribution(Min, Max);

    return Distribution(Generator);
}

template<typename T>
static T GenerateRandomVector(const float Min = SAFE_FLOAT_MIN, const float Max = SAFE_FLOAT_MAX)
{
	return T();
}

template<>
static LVector2 GenerateRandomVector(const float Min, const float Max)
{
	return LVector2(GenerateRandomFloat(Min, Max), GenerateRandomFloat(Min, Max));
}

template<>
static LVector GenerateRandomVector(const float Min, const float Max)
{
    return LVector(GenerateRandomFloat(Min, Max), 
				   GenerateRandomFloat(Min, Max), 
				   GenerateRandomFloat(Min, Max));
}

template<>
static LVector4 GenerateRandomVector(const float Min, const float Max)
{
    return LVector4(GenerateRandomFloat(Min, Max), 
					GenerateRandomFloat(Min, Max), 
					GenerateRandomFloat(Min, Max), 
					GenerateRandomFloat(Min, Max));
}

template<>
static glm::vec2 GenerateRandomVector(const float Min, const float Max)
{
	return glm::vec2(GenerateRandomFloat(Min, Max), GenerateRandomFloat(Min, Max));
}

template<>
static glm::vec3 GenerateRandomVector(const float Min, const float Max)
{
    return glm::vec3(GenerateRandomFloat(Min, Max), 
					 GenerateRandomFloat(Min, Max), 
					 GenerateRandomFloat(Min, Max));
}

template<>
static glm::vec4 GenerateRandomVector(const float Min, const float Max)
{
    return glm::vec4(GenerateRandomFloat(Min, Max), 
					 GenerateRandomFloat(Min, Max), 
					 GenerateRandomFloat(Min, Max),
					 GenerateRandomFloat(Min, Max));
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

static bool IsApproxEqual(const LVector& A, const glm::vec3& B)
{
	return ((std::abs(A.X - B.x) < Epsilon) && (std::abs(A.Y - B.y) < Epsilon) && (std::abs(A.Z - B.z) < Epsilon));
}

static bool IsApproxEqual(const glm::vec3& A, const LVector3& B)
{
	return ((std::abs(A.x - B.X) < Epsilon) && (std::abs(A.y - B.Y) < Epsilon) && (std::abs(A.z - B.Z) < Epsilon));
}

static bool IsApproxEqual(const LVector& A, const LVector3& B)
{
	return ((std::abs(A.X - B.X) < Epsilon) && (std::abs(A.Y - B.Y) < Epsilon) && (std::abs(A.Z - B.Z) < Epsilon));
}

/**************************************************************************
	Operations
***************************************************************************/

constexpr int TestIterations = 5;

/*-------------------------------------------------------------------
  Operation: Add
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Add)
{
	/**
	 * TestVector1: glm::vec3
	 * TestVector2: LVector
	 */
	for (int TestIteration = 0; TestIteration < TestIterations; TestIteration++)
	{
		const auto TestVector1 = GenerateRandomVector<glm::vec3>();
		const auto TestVector2 = GenerateRandomVector<LVector>();

	#if LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR
		LVector ResultVector1 = LVector(TestVector1);
	#else
		LVector ResultVector1 = TestVector1;
	#endif
		glm::vec3 ResultVector2 = TestVector1;

		ResultVector1 = (ResultVector1 + TestVector2);
		ResultVector2 = (ResultVector2 + TestVector2.As<glm::vec3>());

		if (ResultVector1 != ResultVector2)
		{
			LK_TEST_ERROR_TAG(TestName, "Test failed (iteration: {}), TestVector1: glm::vec3  TestVector2: LVector\n'{}' != '{}'", 
							  TestIteration, ResultVector1.ToString(), ToString(ResultVector2));
			return false;
		}
	}

	/**
	 * TestVector1: LVector
	 * TestVector2: glm::vec3
	 */
	for (int TestIteration = 0; TestIteration < TestIterations; TestIteration++)
	{
		const auto TestVector1 = GenerateRandomVector<LVector>();
		const auto TestVector2 = GenerateRandomVector<glm::vec3>();

		LVector ResultVector1 = TestVector1;
		glm::vec3 ResultVector2 = TestVector1.As<glm::vec3>();

		ResultVector1 = (ResultVector1 + TestVector2);
		ResultVector2 = (ResultVector2 + TestVector2);

		if (ResultVector1 != ResultVector2)
		{
			LK_TEST_ERROR_TAG(TestName, "Test failed (iteration: {}), TestVector1: LVector  TestVector2: glm::vec3\n'{}' != '{}'", 
							  TestIteration, ResultVector1.ToString(), ToString(ResultVector2));
			return false;
		}
	}

	return true;
}

/*-------------------------------------------------------------------
  Operation: Subtract
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Subtract)
{
	/**
	 * TestVector1: glm::vec3
	 * TestVector2: LVector
	 */
	for (int TestIteration = 0; TestIteration < TestIterations; TestIteration++)
	{
		const auto TestVector1 = GenerateRandomVector<glm::vec3>();
		const auto TestVector2 = GenerateRandomVector<LVector>();

	#if LK_MATH_VECTOR3_EXPLICIT_GLM_CONSTRUCTOR
		LVector ResultVector1 = LVector(TestVector1);
	#else
		LVector ResultVector1 = TestVector1;
	#endif
		glm::vec3 ResultVector2 = TestVector1;

		ResultVector1 = (ResultVector1 - TestVector2);
		ResultVector2 = (ResultVector2 - TestVector2.As<glm::vec3>());

		if (ResultVector1 != ResultVector2)
		{
			LK_TEST_ERROR_TAG(TestName, "Test failed (iteration: {}), TestVector1: glm::vec3  TestVector2: LVector\n'{}' != '{}'", 
							  TestIteration, ResultVector1.ToString(), ToString(ResultVector2));
			return false;
		}
	}

	/**
	 * TestVector1: LVector
	 * TestVector2: glm::vec3
	 */
	for (int TestIteration = 0; TestIteration < TestIterations; TestIteration++)
	{
		const auto TestVector1 = GenerateRandomVector<LVector>();
		const auto TestVector2 = GenerateRandomVector<glm::vec3>();

		LVector ResultVector1 = TestVector1;
		glm::vec3 ResultVector2 = TestVector1.As<glm::vec3>();

		ResultVector1 = (ResultVector1 - TestVector2);
		ResultVector2 = (ResultVector2 - TestVector2);

		if (ResultVector1 != ResultVector2)
		{
			LK_TEST_ERROR_TAG(TestName, "Test failed (iteration: {}), TestVector1: LVector  TestVector2: glm::vec3\n'{}' != '{}'", 
							  TestIteration, ResultVector1.ToString(), ToString(ResultVector2));
			return false;
		}
	}

	return true;
}

/*-------------------------------------------------------------------
  Operation: Min
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Min)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;

	uint32_t FailedTests = 0;

	for (int TestIteration = 0; TestIteration < TestIterations; TestIteration++)
	{
		/* TestVector type: glm::vec3 */
		{
			const glm::vec3 TestVector1 = GenerateRandomVector<glm::vec3>();
			const glm::vec3 TestVector2 = GenerateRandomVector<glm::vec3>();

			{
				const LVector Vector = LVector::Min(TestVector1, TestVector2);
				const glm::vec3 Glm_Min = glm::min(TestVector1, TestVector2);
				Test1 = ((Vector.X == Glm_Min.x) && (Vector.Y == Glm_Min.y) && (Vector.Z == Glm_Min.z));
				if (!Test1)
				{
					LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
									  Vector.ToString(), ToString(Glm_Min), ToString(TestVector1), ToString(TestVector2));
					FailedTests++;
				}
			}

			/* Same vectors but using LVector boolean operator for validating results. */
			{
				const LVector Vector = LVector::Min(TestVector1, TestVector2);
				const glm::vec3 Glm_Min = glm::min(TestVector1, TestVector2);
				Test2 = (Vector == Glm_Min);
				if (!Test2)
				{
					LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
									  Vector.ToString(), ToString(Glm_Min), ToString(TestVector1), ToString(TestVector2));
					FailedTests++;
				}
			}
		}

		/* TestVector type: LVector */
		{
			const LVector TestVector1 = GenerateRandomVector<LVector>();
			const LVector TestVector2 = GenerateRandomVector<LVector>();

			{
				const LVector Vector = LVector::Min(TestVector1, TestVector2);
				const glm::vec3 Glm_Min = glm::min(TestVector1.As<glm::vec3>(), TestVector2.As<glm::vec3>());
				Test3 = ((Vector.X == Glm_Min.x) && (Vector.Y == Glm_Min.y) && (Vector.Z == Glm_Min.z));
				if (!Test3)
				{
					LK_TEST_ERROR_TAG(TestName, "Test 3 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
									  Vector.ToString(), ToString(Glm_Min), TestVector1.ToString(), TestVector2.ToString());
					FailedTests++;
				}
			}

			/* Same vectors but using LVector boolean operator for validating results. */
			{
				const LVector Vector = LVector::Min(TestVector1, TestVector2);
				const glm::vec3 Glm_Min = glm::min(TestVector1.As<glm::vec3>(), TestVector2.As<glm::vec3>());
				Test4 = (Vector == Glm_Min);
				if (!Test4)
				{
					LK_TEST_ERROR_TAG(TestName, "Test 4 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
									  Vector.ToString(), ToString(Glm_Min), TestVector1.ToString(), TestVector2.ToString());
					FailedTests++;
				}
			}
		}
	}

	//return (Test1 && Test2 && Test3 && Test4);
	return (FailedTests == 0);
}

/*-------------------------------------------------------------------
  Operation: Max
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Max)
{
	const glm::vec3 TestVector1 = GenerateRandomVector<glm::vec3>();
	const glm::vec3 TestVector2 = GenerateRandomVector<glm::vec3>();

	bool Test1 = false;
	{
		const LVector Vector = LVector::Max(TestVector1, TestVector2);
		const glm::vec3 Glm_Max = glm::max(TestVector1, TestVector2);
		Test1 = ((Vector.X == Glm_Max.x) && (Vector.Y == Glm_Max.y) && (Vector.Z == Glm_Max.z));
		if (!Test1)
		{
			LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
							  Vector.ToString(), ToString(Glm_Max), ToString(TestVector1), ToString(TestVector2));
		}
	}

	/* Same vectors but using LVector boolean operator for validating results. */
	bool Test2 = false;
	{
		const LVector Vector = LVector::Max(TestVector1, TestVector2);
		const glm::vec3 Glm_Max = glm::max(TestVector1, TestVector2);
		Test2 = (Vector == Glm_Max);
		if (!Test2)
		{
			LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
							  Vector.ToString(), ToString(Glm_Max), ToString(TestVector1), ToString(TestVector2));
		}
	}

	return (Test1 && Test2);
}

/*-------------------------------------------------------------------
  Operation: Dot
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Dot)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;

	uint32_t FailedTests = 0;

	for (int i = 0; i < TestIterations; i++)
	{
		/**
		 * TestVector1: glm::vec3
		 * TestVector2: glm::vec3
		 */
		{
			const glm::vec3 TestVector1 = GenerateRandomVector<glm::vec3>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);
			const glm::vec3 TestVector2 = GenerateRandomVector<glm::vec3>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);

			const float Vector_Dot = LVector::Dot(TestVector1, TestVector2);
			const float Glm_Dot = glm::dot(TestVector1, TestVector2);
			Test1 = (Vector_Dot == Glm_Dot);
			if (!Test1)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dot, Glm_Dot, ToString(TestVector1), ToString(TestVector2));
				FailedTests++;
			}
		}

		/**
		 * TestVector1: LVector
		 * TestVector2: LVector
		 */
		{
			const auto TestVector1 = GenerateRandomVector<LVector>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);
			const auto TestVector2 = GenerateRandomVector<LVector>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);

			const float Vector_Dot = LVector::Dot(TestVector1, TestVector2);
			const float Glm_Dot = glm::dot(TestVector1.As<glm::vec3>(), TestVector2.As<glm::vec3>());
			Test2 = (Vector_Dot == Glm_Dot);
			if (!Test2)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dot, Glm_Dot, TestVector1.ToString(), TestVector2.ToString());
				FailedTests++;
			}
		}

		/**
		 * TestVector1: glm::vec3
		 * TestVector2: LVector
		 */
		{
			const auto TestVector1 = GenerateRandomVector<glm::vec3>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);
			const auto TestVector2 = GenerateRandomVector<LVector>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);

			const float Vector_Dot = LVector::Dot(TestVector1, TestVector2);
			const float Glm_Dot = glm::dot(TestVector1, TestVector2.As<glm::vec3>());
			Test3 = (Vector_Dot == Glm_Dot);
			if (!Test3)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 3 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dot, Glm_Dot, ToString(TestVector1), TestVector2.ToString());
				FailedTests++;
			}
		}

		/**
		 * TestVector1: LVector
		 * TestVector2: glm::vec3
		 */
		{
			const auto TestVector1 = GenerateRandomVector<LVector>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);
			const auto TestVector2 = GenerateRandomVector<glm::vec3>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);

			const float Vector_Dot = LVector::Dot(TestVector1, TestVector2);
			const float Glm_Dot = glm::dot(TestVector1.As<glm::vec3>(), TestVector2);
			Test4 = (Vector_Dot == Glm_Dot);
			if (!Test4)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 4 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dot, Glm_Dot, ToString(TestVector1), ToString(TestVector2));
				FailedTests++;
			}
		}
	}

	return (FailedTests == 0);
}

/*-------------------------------------------------------------------
  Operation: Cross
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Cross)
{
	bool Test1 = false;
	bool Test2 = false;

	const glm::vec3 TestVector1 = GenerateRandomVector<glm::vec3>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);
	const glm::vec3 TestVector2 = GenerateRandomVector<glm::vec3>(SAFE_FLOAT_MIN, SAFE_FLOAT_MAX);

	uint32_t FailedTests = 0;

	for (int i = 0; i < TestIterations; i++)
	{
		{
			const LVector Vector = LVector::Cross(TestVector1, TestVector2);
			const glm::vec3 Glm_Cross = glm::cross(TestVector1, TestVector2);
			Test1 = ((Vector.X == Glm_Cross.x) && (Vector.Y == Glm_Cross.y) && (Vector.Z == Glm_Cross.z));
			if (!Test1)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector.ToString(), ToString(Glm_Cross), ToString(TestVector1), ToString(TestVector2));
				FailedTests++;
			}
		}

		/* Same vectors but using LVector boolean operator for validating results. */
		{
			const LVector Vector = LVector::Cross(TestVector1, TestVector2);
			const glm::vec3 Glm_Cross = glm::cross(TestVector1, TestVector2);
			Test2 = (Vector == Glm_Cross);
			if (!Test2)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector.ToString(), ToString(Glm_Cross), ToString(TestVector1), ToString(TestVector2));
				FailedTests++;
			}
		}
	}

	return (FailedTests == 0);
}

/*-------------------------------------------------------------------
  Operation: Normalize
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Normalize)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;

	uint32_t FailedTests = 0;

	for (int i = 0; i < TestIterations; i++)
	{
		/* TestVector type: glm::vec3 */
		{
			const glm::vec3 TestVector = GenerateRandomVector<glm::vec3>();
			{
				const LVector Vector = LVector::Normalize(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector);
				Test1 = ((Vector.X == Glm_Norm.x) && (Vector.Y == Glm_Norm.y) && (Vector.Z == Glm_Norm.z));
				if (!Test1)
				{
					Test1 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test1)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), ToString(TestVector));
						FailedTests++;
					}
				}
			}

			/* Same vectors but using LVector boolean operator for validating results. */
			{
				const LVector Vector = LVector::Normalize(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector);
				Test2 = (Vector == Glm_Norm);
				if (!Test2)
				{
					Test2 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test2)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), ToString(TestVector));
						FailedTests++;
					}
				}
			}
		}

		/* TestVector type: LVector */
		{
			const LVector TestVector = GenerateRandomVector<LVector>();
			{
				const LVector Vector = LVector::Normalize(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector.As<glm::vec3>());
				Test3 = ((Vector.X == Glm_Norm.x) && (Vector.Y == Glm_Norm.y) && (Vector.Z == Glm_Norm.z));
				if (!Test3)
				{
					Test3 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test3)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 3 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), TestVector.ToString());
						FailedTests++;
					}
				}
			}

			{
				const LVector Vector = LVector::Normalize(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector.As<glm::vec3>());
				Test4 = (Vector == Glm_Norm);
				if (!Test4)
				{
					Test4 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test4)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 4 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), TestVector.ToString());
						FailedTests++;
					}
				}
			}
		}
	}

	return (FailedTests == 0);
}

/*-------------------------------------------------------------------
  Operation: NormalizeSafe
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_NormalizeSafe)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;

	uint32_t FailedTests = 0;

	for (int i = 0; i < TestIterations; i++)
	{
		/* TestVector type: glm::vec3 */
		{
			const glm::vec3 TestVector = GenerateRandomVector<glm::vec3>();
			{
				const LVector Vector = LVector::NormalizeSafe(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector);
				Test1 = ((Vector.X == Glm_Norm.x) && (Vector.Y == Glm_Norm.y) && (Vector.Z == Glm_Norm.z));
				if (!Test1)
				{
					Test1 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test1)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), ToString(TestVector));
						FailedTests++;
					}
				}
			}

			/* Same vectors but using LVector boolean operator for validating results. */
			{
				const LVector Vector = LVector::NormalizeSafe(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector);
				Test2 = (Vector == Glm_Norm);
				if (!Test2)
				{
					Test2 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test2)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), ToString(TestVector));
						FailedTests++;
					}
				}
			}
		}

		/* TestVector type: LVector */
		{
			const LVector TestVector = GenerateRandomVector<LVector>();
			{
				const LVector Vector = LVector::NormalizeSafe(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector.As<glm::vec3>());
				Test3 = ((Vector.X == Glm_Norm.x) && (Vector.Y == Glm_Norm.y) && (Vector.Z == Glm_Norm.z));
				if (!Test3)
				{
					Test3 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test3)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 3 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), TestVector.ToString());
						FailedTests++;
					}
				}
			}

			/* Same vectors but using LVector boolean operator for validating results. */
			{
				const LVector Vector = LVector::NormalizeSafe(TestVector);
				const glm::vec3 Glm_Norm = glm::normalize(TestVector.As<glm::vec3>());
				Test4 = (Vector == Glm_Norm);
				if (!Test4)
				{
					Test4 = IsApproxEqual(Vector, Glm_Norm);
					if (!Test4)
					{
						LK_TEST_ERROR_TAG(TestName, "Test 4 failed\nLVector: {}\nGlm: {}\nTestVector: {}", 
										  Vector.ToString(), ToString(Glm_Norm), TestVector.ToString());
						FailedTests++;
					}
				}
			}
		}
	}

	return (FailedTests == 0);
}

/*-------------------------------------------------------------------
  Operation: Distance
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_Distance)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;

	/* TestVector type: glm::vec3 */
	{
		const glm::vec3 TestVector1 = GenerateRandomVector<glm::vec3>();
		const glm::vec3 TestVector2 = GenerateRandomVector<glm::vec3>();

		{
			const float Vector_Dist = LVector::Distance(TestVector1, TestVector2);
			const float Glm_Dist = glm::distance(TestVector1, TestVector2);
			Test1 = (Vector_Dist == Glm_Dist);
			if (!Test1)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 1 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dist, Glm_Dist, ToString(TestVector1), ToString(TestVector2));
			}
		}

		/* Same vectors but using LVector boolean operator for validating results. */
		{
			const float Vector_Dist = LVector::Distance(TestVector1, TestVector2);
			const float Glm_Dist = glm::distance(TestVector1, TestVector2);
			Test2 = (Vector_Dist == Glm_Dist);
			if (!Test2)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 2 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dist, Glm_Dist, ToString(TestVector1), ToString(TestVector2));
			}
		}
	}

	/* TestVector type: LVector */
	{
		const LVector TestVector1 = GenerateRandomVector<LVector>();
		const LVector TestVector2 = GenerateRandomVector<LVector>();

		{
			const float Vector_Dist = LVector::Distance(TestVector1, TestVector2);
			const float Glm_Dist = glm::distance(TestVector1.As<glm::vec3>(), TestVector2.As<glm::vec3>());
			Test3 = (Vector_Dist == Glm_Dist);
			if (!Test3)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 3 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dist, Glm_Dist, TestVector1.ToString(), TestVector2.ToString());
			}
		}

		/* Same vectors but using LVector boolean operator for validating results. */
		{
			const float Vector_Dist = LVector::Distance(TestVector1, TestVector2);
			const float Glm_Dist = glm::distance(TestVector1.As<glm::vec3>(), TestVector2.As<glm::vec3>());
			Test4 = (Vector_Dist == Glm_Dist);
			if (!Test4)
			{
				LK_TEST_ERROR_TAG(TestName, "Test 4 failed\nLVector: {}\nGlm: {}\nTestVector1: {}\nTestVector2: {}", 
								  Vector_Dist, Glm_Dist, TestVector1.ToString(), TestVector2.ToString());
			}
		}
	}

	return (Test1 && Test2 && Test3 && Test4);
}

/*-------------------------------------------------------------------
  Operation: DistanceSquared
  [TVector3<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Operation_DistanceSquared)
{
	const glm::vec3 TestVector1 = GenerateRandomVector<glm::vec3>();
	const glm::vec3 TestVector2 = GenerateRandomVector<glm::vec3>();

	bool Test1 = false;
	{
		const float Vector_Dist = LVector::DistanceSquared(TestVector1, TestVector2);
		const float Glm_Dist = glm::distance2(TestVector1, TestVector2);
		Test1 = (Vector_Dist == Glm_Dist);
		if (!Test1)
		{
			LK_TEST_ERROR_TAG(TestName, "Test 1 failed, LVector: {}, glm: {}", Vector_Dist, Glm_Dist);
		}
	}

	/* Same vectors but using LVector boolean operator for validating results. */
	bool Test2 = false;
	{
		const float Vector_Dist = LVector::DistanceSquared(TestVector1, TestVector2);
		const float Glm_Dist = glm::distance2(TestVector1, TestVector2);
		Test2 = (Vector_Dist == Glm_Dist);
		if (!Test2)
		{
			LK_TEST_ERROR_TAG(TestName, "Test 2 failed, LVector: {}, glm: {}", Vector_Dist, Glm_Dist);
		}
	}

	return (Test1 && Test2);
}


/**************************************************************************
	Compability: glm
***************************************************************************/

/*-------------------------------------------------------------------
  [glm] Copy 
  [TVector2<float>] 
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Float_Compability_Glm_Copy)
{
	using VectorType = glm::vec2;

	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2 = TestVec2;

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2 = TestVec2;

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2 = TestVec2;

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] Copy Constructor
  [TVector2<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Float_Compability_Glm_CopyConstructor)
{
	using VectorType = glm::vec2;

	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2(TestVec2);

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2(TestVec2);

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2(TestVec2);

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] Copy 
  [TVector3<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Float_Compability_Glm_Copy)
{
	using VectorType = glm::vec3;
	
	bool Test1 = false;
	{
		const auto TestVec3 = GenerateRandomVector<VectorType>();
		LVector3 Vec3 = LVector(TestVec3);

		Test1 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
	}

	bool Test2 = false;
	{
		const auto TestVec3 = GenerateRandomVector<VectorType>();
		LVector3 Vec3 = LVector(TestVec3);

		Test2 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
		if (!Test2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const auto TestVec3 = GenerateRandomVector<VectorType>();
		LVector3 Vec3 = LVector(TestVec3);

		Test3 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] Copy Constructor
  [TVector3<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector3_Float_Compability_Glm_CopyConstructor)
{
	using VectorType = glm::vec3;

	bool Test1 = false;
	{
		const auto TestVec3 = GenerateRandomVector<VectorType>();
		LVector3 Vec3(TestVec3);

		Test1 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
	}

	bool Test2 = false;
	{
		const auto TestVec3 = GenerateRandomVector<VectorType>();
		LVector3 Vec3(TestVec3);

		Test2 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
	}

	bool Test3 = false;
	{
		const auto TestVec3 = GenerateRandomVector<VectorType>();
		LVector3 Vec3(TestVec3);

		Test3 = ((Vec3.X == TestVec3.x) && (Vec3.Y == TestVec3.y) && (Vec3.Z == TestVec3.z));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] Copy 
  [TVector4<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector4_Float_Compability_Glm_Copy)
{
	using VectorType = glm::vec4;

	bool Test1 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4 = TestVec4;

		Test1 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test2 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4 = TestVec4;

		Test2 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test3 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4 = TestVec4;

		Test3 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [glm] Copy Constructor
  [LVector4 | TVector4<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector4_Float_Compability_Glm_CopyConstructor)
{
	using VectorType = glm::vec4;

	bool Test1 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4(TestVec4);

		Test1 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test2 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4(TestVec4);

		Test2 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test3 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4(TestVec4);

		Test3 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	return (Test1 && Test2 && Test3);
}


/**************************************************************************
	Compabiltiy: ImGui
***************************************************************************/

/*-------------------------------------------------------------------
  [ImGui] Copy
  [TVector2<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Float_Compability_ImGui_Copy)
{
	using VectorType = ImVec2;

	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2 = TestVec2;

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2 = TestVec2;

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2 = TestVec2;

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [ImGui] Copy Constructor
  [LVector2 | TVector2<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector2_Float_Compability_ImGui_CopyConstructor)
{
	using VectorType = ImVec2;

	bool Test1 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2(TestVec2);

		Test1 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test2 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2(TestVec2);

		Test2 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	bool Test3 = false;
	{
		const auto TestVec2 = GenerateRandomVector<VectorType>();
		LVector2 Vec2(TestVec2);

		Test3 = ((Vec2.X == TestVec2.x) && (Vec2.Y == TestVec2.y));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [ImGui] Copy
  [LVector4 | TVector4<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector4_Float_Compability_ImGui_Copy)
{
	using VectorType = ImVec4;

	bool Test1 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4 = TestVec4;

		Test1 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test2 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4 = TestVec4;

		Test2 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test3 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4 = TestVec4;

		Test3 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	return (Test1 && Test2 && Test3);
}

/*-------------------------------------------------------------------
  [ImGui] Copy Constructor
  [TVector4<float>]
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Math_Vector4_Float_Compability_ImGui_CopyConstructor)
{
	using VectorType = ImVec4;

	bool Test1 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4(TestVec4);

		Test1 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test2 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4(TestVec4);

		Test2 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	bool Test3 = false;
	{
		const auto TestVec4 = GenerateRandomVector<VectorType>();
		LVector4 Vec4(TestVec4);

		Test3 = ((Vec4.X == TestVec4.x) && (Vec4.Y == TestVec4.y) && (Vec4.Z == TestVec4.z) && (Vec4.W == TestVec4.w));
	}

	return (Test1 && Test2 && Test3);
}

