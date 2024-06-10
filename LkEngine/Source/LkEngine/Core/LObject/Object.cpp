#include "LKpch.h"
#include "Object.h"

#include "LkEngine/Core/MetadataRegistry.h"


namespace LkEngine {

	LObject::LObject()
		: Handle(FObjectHandle())
	{
		//LCLASS_REGISTER();
	}

	void LObject::Initialize()
	{
		bInitialized = true;

		LK_CORE_VERIFY(Handle != 0, "Object initialization failed, handle is 0");
	}

	bool LObject::IsSelected() const
	{
		/// TODO
		return false;
	}

}
