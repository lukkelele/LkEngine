#include "LKpch.h"
#include "SelectionContext.h"


namespace LkEngine {

	UUID LSelectionContext::SelectedEntityID = 0;
	TObjectPtr<LObject> GSelectedObject{};

	LSelectionContext& LSelectionContext::Get()
	{
		static LSelectionContext SelectionContext;
		return SelectionContext;
	}

	LSelectionContext::LSelectionContext()
	{
		LK_CORE_TRACE_TAG("SelectionContext", "Class Name: \"{}\"", GetClass()->GetName());
	}

}
