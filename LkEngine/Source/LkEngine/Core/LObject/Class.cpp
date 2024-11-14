#include "LKpch.h"
#include "Class.h"


namespace LkEngine {

	/* TODO: */
	bool LClass::IsChildOf(const LClass* OtherClass) const
	{
		if (!OtherClass)
		{
			return false;
		}

		return (TypeID == OtherClass->TypeID);
	}

}
