#include "ObjectPtrTests.h"

#include "LkEngine/Test/TestManager.h"
#include "LkEngine/Test/DummyTestClass.h"

using namespace LkEngine;
using namespace LkEngine::Test;

/** 
 * Test Definitions. 
 */

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

#if defined(LK_AUTOMATION_TEST_CORE)

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

#endif
