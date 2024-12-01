#include "ObjectPtrTests.h"

/** 
 * Test Definitions. 
 */

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneEqualOne) 
{
	return TestEqual(GetName(), 1, 1);
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_OneLessThanTwo) 
{
	//return TestEqual(GetName(), (1 < 2), true);
	return TestEqual(GetName(), 0, 1);
};

LK_DEFINE_AUTOMATION_TEST(Dummy_Arithmetic_TwoGreaterThanOne) 
{
	//return TestEqual(GetName(), (2 > 1), true);
	return TestEqual(GetName(), 0, 1);
};


LK_DEFINE_AUTOMATION_TEST(ObjectPtr_ReferenceCount_Creation) 
{
	return true;
};

