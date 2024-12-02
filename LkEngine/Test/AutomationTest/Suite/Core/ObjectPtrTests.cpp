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

#if defined(LK_AUTOMATION_TEST_CORE)

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation) 
{
	using namespace LkEngine::Test;
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	return TestEqual("Reference count is expected to be 1", TestObject->GetReferenceCount(), 1);
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyOnce) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	return TestEqual("Reference count is expected to be 2", TestObject->GetReferenceCount(), 2);
};

LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_CopyTwice) 
{
	TObjectPtr<LObject> TestObject = TObjectPtr<LDummyTestClass>::Create();
	TObjectPtr<LObject> CopiedObject(TestObject);
	TObjectPtr<LObject> SecondCopiedObject(CopiedObject);
	return TestEqual("Reference count is expected to be 3", TestObject->GetReferenceCount(), 3);
};

#endif
