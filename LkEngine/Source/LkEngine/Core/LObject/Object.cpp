#include "LKpch.h"
#include "Object.h"


namespace LkEngine {

	LObject::LObject()
		: ObjectHandle(FObjectHandle())
	{
		ObjectFlags = EObjectFlag::NeedInitialization;
	}

	void LObject::Initialize()
	{
		LK_CORE_ASSERT(ObjectHandle != 0, "Object initialization failed, object handle is 0");

		ObjectFlags = EObjectFlag::None;
		bObjectInitialized = true;
	}

}
