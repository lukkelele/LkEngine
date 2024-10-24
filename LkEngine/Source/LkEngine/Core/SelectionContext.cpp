#include "LKpch.h"
#include "SelectionContext.h"


namespace LkEngine {

	TObjectPtr<LObject> GSelectedObject{};

	LSelectionContext& LSelectionContext::Get()
	{
		static LSelectionContext SelectionContext;
		return SelectionContext;
	}

	LSelectionContext::LSelectionContext()
	{
		LCLASS_REGISTER();
		LK_CORE_TRACE_TAG("SelectionContext", "StaticClassName: \"{}\"", StaticClass());
	}

}
