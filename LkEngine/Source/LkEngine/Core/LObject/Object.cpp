#include "LKpch.h"
#include "Object.h"

#include "LkEngine/Core/Template/LkTemplate.h"


namespace LkEngine {

	FOnObjectCreated GOnObjectCreated{};

	LObject::LObject()
		: Handle(FObjectHandle())
	{
		Flags = EObjectFlag::NeedInitialization;
	}

	void LObject::Initialize()
	{
		LK_CORE_ASSERT(Handle != 0, "Object initialization failed, handle is 0");

		Flags = EObjectFlag::None;
		bInitialized = true;
	}

	void LObject::BeginDestroy()
	{
		OnDestructBegin.Broadcast(Handle);
	}

	void LObject::FinalizeDestruction()
	{
		OnDestructEnd.Broadcast(Handle);
	}

	/* TODO */
	bool LObject::IsSelected() const
	{
		return false;
	}

}
