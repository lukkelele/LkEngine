#include "ObjectPtrTests.h"

#include "LkEngine/Test/TestManager.h"

using namespace LkEngine;

class TestClass : public LObject
{
public:
	TestClass()
	{
		LCLASS_REGISTER();
	}
	~TestClass() = default;

private:
	LCLASS(TestClass);
};

/** 
 * Test Definitions. 
 */

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne) 
{
	return TestEqual(GetName(), 1, 1);
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo) 
{
	return TestEqual(GetName(), (1 < 2), true);
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne) 
{
	return TestEqual(GetName(), (2 > 1), true);
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanThree) 
{
	return TestLessThan(GetName(), 2, 3);
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<TestClass>::Create();
	return TestEqual("Reference count is expected to be 1", TestObject->GetReferenceCount(), 1);
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<TestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	return TestEqual("Reference count is expected to be 2", TestObject->GetReferenceCount(), 2);
};

