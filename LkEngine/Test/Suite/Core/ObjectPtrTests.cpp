#include "ObjectPtrTests.h"

#include "LTesto/Runtime/TestManager.h"
#include "LTesto/Core/DummyTestClass.h"

#include <LkEngine/Core/Utility/StringUtils.h>

using namespace LkEngine;
using namespace LkEngine::Test;

/*=================================================================
                           Core Tests

 =================================================================*/ 

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne) 
{
	return TestEqual(1, 1, GetName());
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo) 
{
	return TestEqual((1 < 2), true, GetName());
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne) 
{
	return TestEqual((2 > 1), true, GetName());
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree) 
{
	return TestLessThan(2, 3, GetName());
};


/*--------------------------------------------------
	 Object Pointer Tests.
 --------------------------------------------------*/
LK_DEFINE_AUTOMATION_TEST(ObjectPtr_Operator_Copy)
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	if (TestObject && (TestObject.Get() != nullptr))
	{
		LK_TEST_TRACE_TAG(GetName(), "TestObject is valid for testing");
	}
	else
	{
		LK_TEST_ERROR_TAG(GetName(), "Failed to begin test, TestObject is not valid");
		return false;
	}

	bool Test1 = false;
	{
		constexpr int ArraySize = 10;
		LK_TEST_DEBUG_TAG(GetName(), "Creating array, size: {}", ArraySize);
		std::vector<TObjectPtr<LObject>> Array;
		Array.reserve(ArraySize);
		for (int Idx = 0; Idx < ArraySize; Idx++)
		{
			TObjectPtr<LObject> CopiedObject = TestObject;
			LK_TEST_DEBUG_TAG(GetName(), "{}) Copied Object: {}", Idx, TestObject->GetReferenceCount());

			const bool TestObjectValid = (TestObject.Get() != nullptr);
			const bool CopiedObjectValid = (CopiedObject.Get() != nullptr);
			if (!TestObjectValid || !CopiedObjectValid)
			{
				if (!TestObjectValid)
				{
					LK_TEST_ERROR_TAG(GetName(), "TestObject is not valid");
				}
				if (!CopiedObjectValid)
				{
					LK_TEST_ERROR_TAG(GetName(), "CopiedObject is not valid");
				}
				return false;
			}

			Array.emplace_back(CopiedObject);
		}

		Test1 = TestEqual(TestObject->GetReferenceCount(), 1 + ArraySize, GetName());
	}

	/* The previously released copies in the vector should result in 1 reference left. */
	const bool Test2 = ((TestObject->GetReferenceCount() == 1) && TestObject);

	return Test1 && Test2;
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyConstructor)
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();

	bool Test1 = false;
	{
		constexpr int ArraySize = 10;
		LK_TEST_DEBUG_TAG(GetName(), "Creating array, size: {}", ArraySize);
		std::vector<TObjectPtr<LObject>> Array;
		Array.reserve(ArraySize);
		for (int Idx = 0; Idx < ArraySize; Idx++)
		{
			TObjectPtr<LObject> CopiedObject(TestObject);
			LK_TEST_DEBUG_TAG(GetName(), "{}) Copied Object: {}", Idx, TestObject->GetReferenceCount());
			Array.emplace_back(CopiedObject);
		}

		Test1 = TestEqual(TestObject->GetReferenceCount(), 1 + ArraySize, GetName());
	}

	/* The previously released copies in the vector should result in 1 reference left. */
	const bool Test2 = (TestObject->GetReferenceCount() == 1);

	return Test1 && Test2;
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation) 
{
	using namespace LkEngine::Test;
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	return TestEqual(TestObject->GetReferenceCount(), 1, GetName());
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	return TestEqual(TestObject->GetReferenceCount(), 2, GetName());
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyTwice) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	TObjectPtr<LObject> SecondCopiedObject(CopiedObject);
	return TestEqual(TestObject->GetReferenceCount(), 3, GetName());
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_MoveConstructor)
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> MovedObject(std::move(TestObject));

	const bool Test1 = (MovedObject && (MovedObject->GetReferenceCount() == 1));

	return Test1;
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_Function_PassedAsCopy)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;
	bool Test5 = false;
	bool Test6 = false;
	bool Test7 = false;

	struct FTestInfo
	{
		int Num = -1;
		int Expected = -1;
	};

	/* Function 1: LObject */
	auto Func1_LObject = [](TObjectPtr<LObject> InObject, const FTestInfo& TestInfo) -> bool
	{
		const bool ObjectValid = (InObject.Get() != nullptr);
		if (!ObjectValid)
		{
			LK_TEST_ERROR_TAG("Func1_LObject", "Object not valid");
			return false;
		}

		const bool RefCountCorrect = (InObject->GetReferenceCount() == TestInfo.Expected);

		return ObjectValid && RefCountCorrect;
	};


	/* Function 2: LDummyTestClass */
	auto Func2_DTC = [](TObjectPtr<LDummyTestClass> InObject, const FTestInfo& TestInfo) -> bool
	{
		const bool ObjectValid = (InObject.Get() != nullptr);
		if (!ObjectValid)
		{
			LK_TEST_ERROR_TAG("Func2_DTC", "Object not valid");
			return false;
		}

		const bool RefCountCorrect = (InObject->GetReferenceCount() == TestInfo.Expected);

		return ObjectValid && RefCountCorrect;
	};

	/* Function 3: LDummyTestClassExpanded */
	auto Func3_DTCExpanded = [](TObjectPtr<LDummyTestClass> InObject, const FTestInfo& TestInfo) -> bool
	{
		const bool ObjectValid = (InObject.Get() != nullptr);
		if (!ObjectValid)
		{
			LK_TEST_ERROR_TAG("Func3_DTCExpanded", "Object not valid");
			return false;
		}

		const bool RefCountCorrect = (InObject->GetReferenceCount() == TestInfo.Expected);

		return ObjectValid && RefCountCorrect;
	};

	/*-----------------------------------------------------------------------------*/

	/* Test: 1 */
	{
		const FTestInfo TestInfo = { .Num = 1, .Expected = 2 };
		TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();

		const bool FuncResult = Func1_LObject(TestObject, TestInfo);
		Test1 = FuncResult && TestObject;
		if (!Test1)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func1_LObject)", TestInfo.Num);
		}
	}

	/* Test: 2 */
	{
		const FTestInfo TestInfo = { .Num = 2, .Expected = 2 };
		TObjectPtr<LDummyTestClass> TestObject = TObjectPtr<LDummyTestClass>::Create();

		const bool FuncResult = Func2_DTC(TestObject, TestInfo);
		Test2 = FuncResult && TestObject;
		if (!Test2)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func2_DTC)", TestInfo.Num);
		}
	}

	/* Test: 3 (Store TestObject as LObject but create as LDummyTestClass. */
	{
		const FTestInfo TestInfo = { .Num = 3, .Expected = 2 };
		TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();

		const bool FuncResult = Func1_LObject(TestObject, TestInfo);
		Test3 = FuncResult && TestObject;
		if (!Test3)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func1_LObject)", TestInfo.Num);
		}
	}

	/* Test: 4 (Store TestObject as LObject but create as LDummyTestClass. */
	{
		const FTestInfo TestInfo = { .Num = 4, .Expected = 3 }; /* Expected is 3 because of the TObjectPtr::As call. */
		TObjectPtr<LObject> TestObject1 = TObjectPtr<LDummyTestClass>::Create();
		TObjectPtr<LDummyTestClass> TestObject = TestObject1.As<LDummyTestClass>();

		const bool FuncResult = Func2_DTC(TestObject, TestInfo);
		Test4 = FuncResult && TestObject;
		if (!Test4)
		{
			std::string Error;
			if (!TestObject)
			{
				Error = "TestObject is nullptr";
			}
			else
			{
				Error = LK_FMT_LIB::format("Reference count mismatch, object has {}", TestObject->GetReferenceCount());
			}

			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func2_DTC), error: {}", TestInfo.Num, Error);
		}
	}

	/* Test: 5 (TestObject: LDummyTestClassExpanded) */
	{
		const FTestInfo TestInfo = { .Num = 5, .Expected = 2 };
		TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClassExpanded>::Create();

		const bool FuncResult = Func1_LObject(TestObject, TestInfo);
		Test5 = FuncResult && TestObject;
		if (!Test5)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func1_LObject)", TestInfo.Num);
		}
	}

	/* Test: 6 (TestObject: LDummyTestClassExpanded) */
	{
		const FTestInfo TestInfo = { .Num = 6, .Expected = 2 };
		TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClassExpanded>::Create();

		const bool FuncResult = Func2_DTC(TestObject, TestInfo);
		Test6 = FuncResult && TestObject;
		if (!Test6)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func2_DTC)", TestInfo.Num);
		}
	}

	/* Test: 7 (TestObject: LDummyTestClassExpanded) */
	{
		/* Expect 3 references, 1 from creation, 1 from TObjectPtr::As invocation and 1 from the copy in the function. */
		const FTestInfo TestInfo = { .Num = 7, .Expected = 3 };

		TObjectPtr<LObject> TestObjectRaw = TObjectPtr<LDummyTestClassExpanded>::Create();
		TObjectPtr<LObject> TestObject = TestObjectRaw.As<LDummyTestClassExpanded>();

		const bool FuncResult = Func3_DTCExpanded(TestObject, TestInfo);
		Test7 = FuncResult && TestObject;
		if (!Test7)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func2_DTC)", TestInfo.Num);
		}
	}

	return Test1 && Test2 && Test3 && Test4 && Test5 && Test6 && Test7;
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_Function_PassedAsReference)
{
	bool Test1 = false;
	bool Test2 = false;
	bool Test3 = false;
	bool Test4 = false;

	const std::string TestName = GetName();

	struct FTestInfo
	{
		int Num = -1;
		int Expected = -1;
	};

	/* Function 1: LObject */
	auto Func1_LObject = [](TObjectPtr<LObject>& InObject, const FTestInfo& TestInfo) -> bool
	{
		const bool ObjectValid = (InObject.Get() != nullptr);
		if (!ObjectValid)
		{
			LK_TEST_ERROR_TAG("Func1_LObject", "Object not valid");
			return false;
		}

		const bool RefCountCorrect = (InObject->GetReferenceCount() == TestInfo.Expected);

		return ObjectValid && RefCountCorrect;
	};

	/* Function 2: LDummyTestClass */
	auto Func2_DTC = [](TObjectPtr<LDummyTestClass>& InObject, const FTestInfo& TestInfo) -> bool
	{
		const bool ObjectValid = (InObject.Get() != nullptr);
		if (!ObjectValid)
		{
			LK_TEST_ERROR_TAG("Func2_DTC", "Object not valid");
			return false;
		}

		const bool RefCountCorrect = (InObject->GetReferenceCount() == TestInfo.Expected);

		return ObjectValid && RefCountCorrect;
	};

	/* Test: 1 */
	{
		const FTestInfo TestInfo = { .Num = 1, .Expected = 1 };
		TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();

		const bool FuncResult = Func1_LObject(TestObject, TestInfo);
		Test1 = FuncResult && TestObject;
		if (!Test1)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test 1 failed (Func1_LObject)");
		}
	}

	/* Test: 2 */
	{
		const FTestInfo TestInfo = { .Num = 2, .Expected = 1 };
		TObjectPtr<LDummyTestClass> TestObject = TObjectPtr<LDummyTestClass>::Create();

		const bool FuncResult = Func2_DTC(TestObject, TestInfo);
		Test2 = FuncResult && TestObject;
		if (!Test2)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func2_DTC)", TestInfo.Num);
		}
	}

	/* Test: 3 (Store TestObject as LObject but create as LDummyTestClass. */
	{
		const FTestInfo TestInfo = { .Num = 3, .Expected = 1 };
		TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();

		const bool FuncResult = Func1_LObject(TestObject, TestInfo);
		Test3 = FuncResult && TestObject;
		if (!Test3)
		{
			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func1_LObject)", TestInfo.Num);
		}
	}

	/* Test: 4 (Store TestObject as LObject but create as LDummyTestClass. */
	{
		const FTestInfo TestInfo = { .Num = 4, .Expected = 2 }; /* Expected is 2 because of the TObjectPtr::As call. */
		TObjectPtr<LObject> TestObject1 = TObjectPtr<LDummyTestClass>::Create();
		TObjectPtr<LDummyTestClass> TestObject = TestObject1.As<LDummyTestClass>();

		const bool FuncResult = Func2_DTC(TestObject, TestInfo);
		Test4 = FuncResult && TestObject;
		if (!Test4)
		{
			std::string Error;
			if (!TestObject)
			{
				Error = "TestObject is nullptr";
			}
			else
			{
				Error = LK_FMT_LIB::format("Reference count mismatch, object has {}", TestObject->GetReferenceCount());
			}

			LK_TEST_ERROR_TAG(GetName(), "Test {} failed (Func2_DTC), error: {}", TestInfo.Num, Error);
		}
	}

	return Test1 && Test2 && Test3 && Test4;
};
