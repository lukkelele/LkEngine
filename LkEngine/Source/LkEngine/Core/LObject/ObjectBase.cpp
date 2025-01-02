#include "LKpch.h"
#include "ObjectBase.h"

#include "LkEngine/Core/LObject/Class.h"


namespace LkEngine {

	void LObjectBase::SetClass(LClass* InClass)
	{
		LK_CORE_VERIFY(InClass, "Class object is nullptr");
		LK_CORE_VERIFY(ClassPrivate == nullptr, "SetClass called multiple times for: '{}'", ClassPrivate->GetName());
		ClassPrivate = InClass;
	}

}
