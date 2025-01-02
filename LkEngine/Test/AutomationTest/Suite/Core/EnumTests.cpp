#include "EnumTests.h"

#include "AutomationTest/TestManager.h"
#include "AutomationTest/DummyTestClass.h"

#include <LkEngine/Core/Log/Log.h>
#include <LkEngine/Core/Utility/StringUtils.h>
#include <LkEngine/Core/Math/MathLibrary.h>

using namespace LkEngine;

#define LK_ENUM_VALUE(Value) LK_BIT(Value)

enum class ETestEnum : uint32_t
{
	None   = LK_BIT(0),
	Value1 = LK_BIT(1),
	Value2 = LK_BIT(2),
	Value3 = LK_BIT(3),
	Value4 = LK_BIT(4),
	COUNT  = LK_BIT(5)
};
LK_ENUM_CLASS_FLAGS(ETestEnum);
static_assert(std::is_same_v<LK_Enum_Type_ETestEnum, std::underlying_type_t<ETestEnum>>);

/* TODO: Cannot subtract 1, need to shift value. */
//static_assert((ETestEnum)((int)ETestEnum::COUNT - 1) <= std::numeric_limits<std::underlying_type_t<ETestEnum>>::max());
static_assert(ETestEnum::COUNT <= std::numeric_limits<std::underlying_type_t<ETestEnum>>::max());

namespace LkEngine::Enum 
{
	inline static constexpr const char* ToString(const ETestEnum EnumValue)
	{
		switch (EnumValue)
		{
			case ETestEnum::None:   return "None";
			case ETestEnum::Value1: return "Value1";
			case ETestEnum::Value2: return "Value2";
			case ETestEnum::Value3: return "Value3";
			case ETestEnum::Value4: return "Value4";
			case ETestEnum::COUNT:  return "COUNT";
		}

		LK_CORE_ASSERT(false, "Unknown ETestEnum value: {}", static_cast<int>(EnumValue));
		return nullptr;
	}
}

LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Bitwise_AND)
{
	//using SizeType = std::underlying_type_t<ETestEnum>;
	using SizeType = LK_Enum_Type_ETestEnum;
	bool bTest1 = false;
	{
		constexpr SizeType TestValue = LK_BIT(1);
		bTest1 = ((TestValue & LK_BIT(1)) == (TestValue & ETestEnum::Value1));
		if (!bTest1)
		{
			LK_PRINTLN("[{}] Test 1: {}", GetName(), (bTest1 ? "OK" : "Failed"));
		}
	}

	bool bTest2 = false;
	{
		constexpr SizeType TestValue = LK_BIT(2);
		bTest2 = ((TestValue & LK_BIT(2)) == (TestValue & ETestEnum::Value2));
		if (!bTest2)
		{
			LK_PRINTLN("[{}] Test 2: {}", GetName(), (bTest2 ? "OK" : "Failed"));
		}
	}

	bool bTest3 = false;
	{
		constexpr SizeType TestValue = LK_BIT(3);
		bTest3 = ((TestValue & LK_BIT(3)) == (TestValue & ETestEnum::Value3));
		if (!bTest3)
		{
			LK_PRINTLN("[{}] Test 3: {}", GetName(), (bTest3 ? "OK" : "Failed"));
		}
	}

	bool bTest4 = false;
	{
		constexpr int Iterations = 10;
		int Failed = 0;
		bool bCurrentResult = false;

		for (int i = 0; i < Iterations; i++)
		{
			/* Value 1. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bCurrentResult = ((TestValue & LK_BIT(1)) == (TestValue & ETestEnum::Value1));
			if (!bCurrentResult)
			{
				Failed++;
				LK_PRINTLN("[{}] Test 4: ({}) Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < Iterations; i++)
		{
			/* Value 2. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bCurrentResult = ((TestValue & LK_BIT(2)) == (TestValue & ETestEnum::Value2));
			if (!bCurrentResult)
			{
				Failed++;
				LK_TEST_WARN("[{}] Test 4: ({}) Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < Iterations; i++)
		{
			/* Value 3. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bCurrentResult = ((TestValue & LK_BIT(3)) == (TestValue & ETestEnum::Value3));
			if (!bCurrentResult)
			{
				Failed++;
				LK_TEST_WARN("[{}] Test 4: ({}) Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		bTest4 = (Failed == 0);
	}

	return (bTest1 && bTest2 && bTest3 && bTest4);
};

LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Bitwise_OR)
{
	using SizeType = std::underlying_type_t<ETestEnum>;

	bool bTest1 = false;
	{
		constexpr SizeType TestValue = LK_BIT(1);
		bTest1 = ((TestValue | LK_BIT(1)) == (TestValue | ETestEnum::Value1));
		if (!bTest1)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest1 ? "OK" : "Failed"));
		}
	}

	bool bTest2 = false;
	{
		constexpr SizeType TestValue = LK_BIT(2);
		bTest2 = ((TestValue | LK_BIT(2)) == (TestValue | ETestEnum::Value2));
		if (!bTest2)
		{
			LK_TEST_WARN("[{}] Test 2: {}", GetName(), (bTest2 ? "OK" : "Failed"));
		}
	}

	bool bTest3 = false;
	{
		constexpr SizeType TestValue = LK_BIT(3);
		bTest3 = ((TestValue | LK_BIT(3)) == (TestValue | ETestEnum::Value3));
		if (!bTest3)
		{
			LK_TEST_WARN("[{}] Test 3: {}", GetName(), (bTest3 ? "OK" : "Failed"));
		}
	}

	bool bTest4 = false;
	{
		constexpr int Iterations = 10;
		int Failed = 0;
		bool bCurrentResult = false;

		for (int i = 0; i < Iterations; i++)
		{
			/* Value 1. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bCurrentResult = ((TestValue | LK_BIT(1)) == (TestValue | ETestEnum::Value1));
			if (!bCurrentResult)
			{
				Failed++;
				LK_TEST_WARN("[{}] Test 4: Value1 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < Iterations; i++)
		{
			/* Value 2. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bCurrentResult = ((TestValue | LK_BIT(2)) == (TestValue | ETestEnum::Value2));
			if (!bCurrentResult)
			{
				Failed++;
				LK_TEST_WARN("[{}] Test 4: Value2 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < Iterations; i++)
		{
			/* Value 3. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bCurrentResult = ((TestValue | LK_BIT(3)) == (TestValue | ETestEnum::Value3));
			if (!bCurrentResult)
			{
				Failed++;
				LK_TEST_WARN("[{}] Test 4: Value3 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		bTest4 = (Failed == 0);
	}

	return (bTest1 && bTest2 && bTest3 && bTest4);
};

/*-------------------------------------------------------------------
	Comparison Operator: Less Than
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_LessThan)
{
	using SizeType = std::underlying_type_t<ETestEnum>;
	bool bTest1 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest1 = ((TestValue < LK_BIT(1)) == (TestValue < ETestEnum::Value1));
		if (!bTest1)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest1 ? "OK" : "Failed"));
		}
	}

	bool bTest2 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest2 = ((TestValue < LK_BIT(2)) == (TestValue < ETestEnum::Value2));
		if (!bTest2)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest2 ? "OK" : "Failed"));
		}
	}

	bool bTest3 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest3 = ((TestValue < LK_BIT(3)) == (TestValue < ETestEnum::Value3));
		if (!bTest3)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest3 ? "OK" : "Failed"));
		}
	}

	bool bTest4 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest4 = ((TestValue < LK_BIT(4)) == (TestValue < ETestEnum::Value4));
		if (!bTest4)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest4 ? "OK" : "Failed"));
		}
	}

	return (bTest1 && bTest2 && bTest3 && bTest4);
}

/*-------------------------------------------------------------------
	Comparison Operator: Less Than or Equal To
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_LessThanOrEqualTo)
{
	using SizeType = std::underlying_type_t<ETestEnum>;
	bool bTest1 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest1 = ((TestValue <= LK_BIT(1)) == (TestValue <= ETestEnum::Value1));
		if (!bTest1)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest1 ? "OK" : "Failed"));
		}
	}

	bool bTest2 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest2 = ((TestValue <= LK_BIT(2)) == (TestValue <= ETestEnum::Value2));
		if (!bTest2)
		{
			LK_TEST_WARN("[{}] Test 2: {}", GetName(), (bTest2 ? "OK" : "Failed"));
		}
	}

	bool bTest3 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest3 = ((TestValue <= LK_BIT(3)) == (TestValue <= ETestEnum::Value3));
		if (!bTest3)
		{
			LK_TEST_WARN("[{}] Test 3: {}", GetName(), (bTest3 ? "OK" : "Failed"));
		}
	}

	bool bTest4 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest4 = ((TestValue <= LK_BIT(4)) == (TestValue <= ETestEnum::Value4));
		if (!bTest4)
		{
			LK_TEST_WARN("[{}] Test 4: {}", GetName(), (bTest4 ? "OK" : "Failed"));
		}
	}

	return (bTest1 && bTest2 && bTest3 && bTest4);
}

/*-------------------------------------------------------------------
	Comparison Operator: Greater Than
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_GreaterThan)
{
	using SizeType = std::underlying_type_t<ETestEnum>;
	bool bTest1 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest1 = ((TestValue > LK_BIT(1)) == (TestValue > ETestEnum::Value1));
		if (!bTest1)
		{
			LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest1 ? "OK" : "Failed"));
		}
	}

	bool bTest2 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest2 = ((TestValue > LK_BIT(2)) == (TestValue > ETestEnum::Value2));
		if (!bTest2)
		{
			LK_TEST_WARN("[{}] Test 2: {}", GetName(), (bTest2 ? "OK" : "Failed"));
		}
	}

	bool bTest3 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest3 = ((TestValue > LK_BIT(3)) == (TestValue > ETestEnum::Value3));
		if (!bTest3)
		{
			LK_TEST_WARN("[{}] Test 3: {}", GetName(), (bTest3 ? "OK" : "Failed"));
		}
	}

	bool bTest4 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		bTest4 = ((TestValue > LK_BIT(4)) == (TestValue > ETestEnum::Value4));
		if (!bTest4)
		{
			LK_TEST_WARN("[{}] Test 4: {}", GetName(), (bTest4 ? "OK" : "Failed"));
		}
	}

	return (bTest1 && bTest2 && bTest3 && bTest4);
}

/*-------------------------------------------------------------------
	Comparison Operator: Greater Than or Equal To
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_GreaterThanOrEqualTo)
{
	using SizeType = std::underlying_type_t<ETestEnum>;
	const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
	constexpr int Iterations = 10;

	int Errors = 0; /* TODO: Remove this and replace with internal error counter in the test class. */

	for (int i = 0; i < Iterations; i++)
	{
		bool bTest1 = false;
		{
			bTest1 = ((TestValue >= LK_BIT(1)) == (TestValue >= ETestEnum::Value1));
			if (!bTest1)
			{
				LK_TEST_WARN("[{}] Test 1: {}", GetName(), (bTest1 ? "OK" : "Failed"));
				AddError(std::format("Test 1 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}

		bool bTest2 = false;
		{
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bTest2 = ((TestValue >= LK_BIT(2)) == (TestValue >= ETestEnum::Value2));
			if (!bTest2)
			{
				LK_TEST_WARN("[{}] Test 2: {}", GetName(), (bTest2 ? "OK" : "Failed"));
				AddError(std::format("Test 2 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}

		bool bTest3 = false;
		{
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bTest3 = ((TestValue >= LK_BIT(3)) == (TestValue >= ETestEnum::Value3));
			if (!bTest3)
			{
				LK_TEST_WARN("[{}] Test 3: {}", GetName(), (bTest3 ? "OK" : "Failed"));
				AddError(std::format("Test 3 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}

		bool bTest4 = false;
		{
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			bTest4 = ((TestValue >= LK_BIT(4)) == (TestValue >= ETestEnum::Value4));
			if (!bTest4)
			{
				LK_TEST_WARN("[{}] Test 4: {}", GetName(), (bTest4 ? "OK" : "Failed"));
				AddError(std::format("Test 4 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}
	}

	return (Errors == 0);
}

