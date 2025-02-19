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
	Value1 = LK_BIT(1),
	Value2 = LK_BIT(2),
	Value3 = LK_BIT(3),
	Value4 = LK_BIT(4),
	COUNT  = LK_BIT(5)
};

LK_ENUM_CLASS_FLAGS(ETestEnum);

LK_ENUM_RANGE_BY_VALUES(ETestEnum, 
	ETestEnum::Value1, 
	ETestEnum::Value2, 
	ETestEnum::Value3, 
	ETestEnum::Value4
);

namespace LkEngine::Enum 
{
	inline static constexpr const char* ToString(const ETestEnum EnumValue)
	{
		switch (EnumValue)
		{
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

static_assert(std::is_same_v<LK_Enum_ETestEnum, std::underlying_type_t<ETestEnum>>);

/* TODO: Cannot subtract 1, need to shift value because bitmask. */
static_assert(ETestEnum::COUNT <= std::numeric_limits<std::underlying_type_t<ETestEnum>>::max());

std::unordered_map<int, ETestEnum> TestPairMap;

static constexpr int TestIterations = 25;


LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Bitwise_AND)
{
	using SizeType = LK_Enum_ETestEnum;

	bool Test1 = false;
	{
		constexpr SizeType TestValue = LK_BIT(1);
		Test1 = ((TestValue & LK_BIT(1)) == (TestValue & ETestEnum::Value1));
	}
	if (!Test1)
	{
		LK_TEST_ERROR("[{}] Failed: Test 1", GetName());
	}

	bool Test2 = false;
	{
		constexpr SizeType TestValue = LK_BIT(2);
		Test2 = ((TestValue & LK_BIT(2)) == (TestValue & ETestEnum::Value2));
	}
	if (!Test2)
	{
		LK_TEST_ERROR("[{}] Failed: Test 2", GetName());
	}

	bool Test3 = false;
	{
		constexpr SizeType TestValue = LK_BIT(3);
		Test3 = ((TestValue & LK_BIT(3)) == (TestValue & ETestEnum::Value3));
	}
	if (!Test3)
	{
		LK_TEST_ERROR("[{}] Failed: Test 3", GetName());
	}

	/* TODO: Use enum-range impl here instead. */
	bool Test4 = false;
	{
		int Failed = 0;
		bool CurrentResult = false;

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 1. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue & LK_BIT(1)) == (TestValue & ETestEnum::Value1));
			if (!CurrentResult)
			{
				Failed++;
				LK_PRINTLN("[{}] Test 4: ({}) Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 2. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue & LK_BIT(2)) == (TestValue & ETestEnum::Value2));
			if (!CurrentResult)
			{
				Failed++;
				LK_TEST_ERROR("[{}] Test 4: ({}) Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 3. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue & LK_BIT(3)) == (TestValue & ETestEnum::Value3));
			if (!CurrentResult)
			{
				Failed++;
				LK_TEST_ERROR("[{}] Test 4: ({}) Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		Test4 = (Failed == 0);
	}

	bool Test5 = false;
	{
		bool CurrentResult = false;

		for (int i = 0; i < TestIterations; i++)
		{
			for (ETestEnum Enum : TEnumRange<ETestEnum>())
			{
				const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();

				SizeType TestValue1 = TestValue;
				SizeType TestValue2 = TestValue;
				TestValue1 &= (SizeType)Enum;  /* Integer bitwise operator. */
				TestValue2 &= Enum;            /* Custom bitwise operator. */

				CurrentResult = (TestValue1 == TestValue2);
				if (!CurrentResult)
				{
					LK_TEST_ERROR("[{}] [Test 5] Value: {} ", GetName(), TestValue);
					/* Exit directly if a single test iteration fails. */
					break;
					//return false;
				}
				//else LK_TEST_INFO("[{}] [Test 5] [OK] '{}'  Value1={}  Value2={} ({})", GetName(), TestValue, TestValue1, TestValue2, Enum::ToString(Enum));
			}

			if (!CurrentResult)
			{
				break;
			}
		}

		Test5 = CurrentResult;
	}

	return (Test1 && Test2 && Test3 && Test4 && Test5);
};

LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Bitwise_OR)
{
	using SizeType = std::underlying_type_t<ETestEnum>;

	bool Test1 = false;
	{
		constexpr SizeType TestValue = LK_BIT(1);
		Test1 = ((TestValue | LK_BIT(1)) == (TestValue | ETestEnum::Value1));
		if (!Test1)
		{
			LK_TEST_ERROR("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		constexpr SizeType TestValue = LK_BIT(2);
		Test2 = ((TestValue | LK_BIT(2)) == (TestValue | ETestEnum::Value2));
		if (!Test2)
		{
			LK_TEST_ERROR("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		constexpr SizeType TestValue = LK_BIT(3);
		Test3 = ((TestValue | LK_BIT(3)) == (TestValue | ETestEnum::Value3));
		if (!Test3)
		{
			LK_TEST_ERROR("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	bool Test4 = false;
	{
		int Failed = 0;
		bool CurrentResult = false;

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 1. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue | LK_BIT(1)) == (TestValue | ETestEnum::Value1));
			if (!CurrentResult)
			{
				Failed++;
				LK_TEST_ERROR("[{}] Test 4: Value1 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 2. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue | LK_BIT(2)) == (TestValue | ETestEnum::Value2));
			if (!CurrentResult)
			{
				Failed++;
				LK_TEST_ERROR("[{}] Test 4: Value2 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 3. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue | LK_BIT(3)) == (TestValue | ETestEnum::Value3));
			if (!CurrentResult)
			{
				Failed++;
				LK_TEST_ERROR("[{}] Test 4: Value3 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		for (int i = 0; i < TestIterations; i++)
		{
			/* Value 4. */
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			CurrentResult = ((TestValue | LK_BIT(4)) == (TestValue | ETestEnum::Value4));
			if (!CurrentResult)
			{
				Failed++;
				LK_TEST_ERROR("[{}] Test 4: Value4 ({}), Value: {}  Failed: {}", GetName(), i, TestValue, Failed); 
			}
		}

		Test4 = (Failed == 0);
	}

	bool Test5 = false;
	{
		bool CurrentResult = false;

		for (int i = 0; i < TestIterations; i++)
		{
			for (ETestEnum Enum : TEnumRange<ETestEnum>())
			{
				const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();

				SizeType TestValue1 = TestValue;
				SizeType TestValue2 = TestValue;
				TestValue1 |= (SizeType)Enum;  /* Integer bitwise operator. */
				TestValue2 |= Enum;            /* Custom bitwise operator. */

				CurrentResult = (TestValue1 == TestValue2);
				if (!CurrentResult)
				{
					LK_TEST_ERROR("[{}] [Test 5] Value: {} ", GetName(), TestValue);
					/* Exit directly if a single test iteration fails. */
					break;
					//return false;
				}
				//else LK_TEST_INFO("[{}] [Test 5] [OK] '{}'  Value1={}  Value2={} ({})", GetName(), TestValue, TestValue1, TestValue2, Enum::ToString(Enum));
			}

			if (!CurrentResult)
			{
				break;
			}
		}

		Test5 = CurrentResult;
	}

	return (Test1 && Test2 && Test3 && Test4 && Test5);
};

/*-------------------------------------------------------------------
	Comparison Operator: Less Than
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_LessThan)
{
	using SizeType = std::underlying_type_t<ETestEnum>;

	bool Test1 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test1 = ((TestValue < LK_BIT(1)) == (TestValue < ETestEnum::Value1));
		if (!Test1)
		{
			LK_TEST_ERROR("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test2 = ((TestValue < LK_BIT(2)) == (TestValue < ETestEnum::Value2));
		if (!Test2)
		{
			LK_TEST_ERROR("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test3 = ((TestValue < LK_BIT(3)) == (TestValue < ETestEnum::Value3));
		if (!Test3)
		{
			LK_TEST_ERROR("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	bool Test4 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test4 = ((TestValue < LK_BIT(4)) == (TestValue < ETestEnum::Value4));
		if (!Test4)
		{
			LK_TEST_ERROR("[{}] Test 4: {}", GetName(), (Test4 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3 && Test4);
}

/*-------------------------------------------------------------------
	Comparison Operator: Less Than or Equal To
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_LessThanOrEqualTo)
{
	using SizeType = std::underlying_type_t<ETestEnum>;

	bool Test1 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test1 = ((TestValue <= LK_BIT(1)) == (TestValue <= ETestEnum::Value1));
		if (!Test1)
		{
			LK_TEST_ERROR("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test2 = ((TestValue <= LK_BIT(2)) == (TestValue <= ETestEnum::Value2));
		if (!Test2)
		{
			LK_TEST_ERROR("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test3 = ((TestValue <= LK_BIT(3)) == (TestValue <= ETestEnum::Value3));
		if (!Test3)
		{
			LK_TEST_ERROR("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	bool Test4 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test4 = ((TestValue <= LK_BIT(4)) == (TestValue <= ETestEnum::Value4));
		if (!Test4)
		{
			LK_TEST_ERROR("[{}] Test 4: {}", GetName(), (Test4 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3 && Test4);
}

/*-------------------------------------------------------------------
	Comparison Operator: Greater Than
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_GreaterThan)
{
	using SizeType = std::underlying_type_t<ETestEnum>;

	bool Test1 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test1 = ((TestValue > LK_BIT(1)) == (TestValue > ETestEnum::Value1));
		if (!Test1)
		{
			LK_TEST_ERROR("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
		}
	}

	bool Test2 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test2 = ((TestValue > LK_BIT(2)) == (TestValue > ETestEnum::Value2));
		if (!Test2)
		{
			LK_TEST_ERROR("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
		}
	}

	bool Test3 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test3 = ((TestValue > LK_BIT(3)) == (TestValue > ETestEnum::Value3));
		if (!Test3)
		{
			LK_TEST_ERROR("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
		}
	}

	bool Test4 = false;
	{
		const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
		Test4 = ((TestValue > LK_BIT(4)) == (TestValue > ETestEnum::Value4));
		if (!Test4)
		{
			LK_TEST_ERROR("[{}] Test 4: {}", GetName(), (Test4 ? "OK" : "Failed"));
		}
	}

	return (Test1 && Test2 && Test3 && Test4);
}

/*-------------------------------------------------------------------
	Comparison Operator: Greater Than or Equal To
 -------------------------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(Enum_EnumClass_Operator_Comparison_GreaterThanOrEqualTo)
{
	using SizeType = std::underlying_type_t<ETestEnum>;
	const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();

	/* TODO: Patch this with the enum-ranged for-loop approach instead. */

	int Errors = 0; /* TODO: Remove this and replace with internal error counter in the test class. */

	for (int i = 0; i < TestIterations; i++)
	{
		bool Test1 = false;
		{
			Test1 = ((TestValue >= LK_BIT(1)) == (TestValue >= ETestEnum::Value1));
			if (!Test1)
			{
				LK_TEST_ERROR("[{}] Test 1: {}", GetName(), (Test1 ? "OK" : "Failed"));
				AddError(std::format("Test 1 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}

		bool Test2 = false;
		{
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			Test2 = ((TestValue >= LK_BIT(2)) == (TestValue >= ETestEnum::Value2));
			if (!Test2)
			{
				LK_TEST_ERROR("[{}] Test 2: {}", GetName(), (Test2 ? "OK" : "Failed"));
				AddError(std::format("Test 2 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}

		bool Test3 = false;
		{
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			Test3 = ((TestValue >= LK_BIT(3)) == (TestValue >= ETestEnum::Value3));
			if (!Test3)
			{
				LK_TEST_ERROR("[{}] Test 3: {}", GetName(), (Test3 ? "OK" : "Failed"));
				AddError(std::format("Test 3 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}

		bool Test4 = false;
		{
			const SizeType TestValue = LkEngine::Math::GenerateRandomNumber<SizeType>();
			Test4 = ((TestValue >= LK_BIT(4)) == (TestValue >= ETestEnum::Value4));
			if (!Test4)
			{
				LK_TEST_ERROR("[{}] Test 4: {}", GetName(), (Test4 ? "OK" : "Failed"));
				AddError(std::format("Test 4 failed (iteration: {}) for '{}' with enum: ETestEnum::{}", GetName(), i, Enum::ToString(ETestEnum::Value4)));
				Errors++;
			}
		}
	}

	return (Errors == 0);
}

