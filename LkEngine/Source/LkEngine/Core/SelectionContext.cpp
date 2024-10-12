#include "LKpch.h"
#include "SelectionContext.h"


namespace LkEngine {

	TObjectPtr<LObject> GSelectedEntity{};

	LSelectionContext& LSelectionContext::Get()
	{
		static LSelectionContext SelectionContext;
		return SelectionContext;
	}

	LSelectionContext::LSelectionContext()
	{
		LCLASS_REGISTER();

		LK_CORE_DEBUG_TAG("SelectionContext", "StaticClassName: \"{}\"", GetStaticClass());
	}

}
