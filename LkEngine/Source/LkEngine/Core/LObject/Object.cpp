#include "LKpch.h"
#include "Object.h"

#include "LkEngine/Core/MetadataRegistry.h"


namespace LkEngine {

	LObject::LObject()
		: Handle(FObjectHandle())
		, Name("LObject")
	{
	}

	void LObject::Initialize()
	{
		bInitialized = true;

		LK_CORE_VERIFY(Handle != 0, "Object initialization failed, handle is 0");
	}

	/* TODO */
	bool LObject::IsSelected() const
	{
		return false;
	}

}
