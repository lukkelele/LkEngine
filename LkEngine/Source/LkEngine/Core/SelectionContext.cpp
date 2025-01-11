#include "LKpch.h"
#include "SelectionContext.h"


namespace LkEngine {

	LUUID LSelectionContext::SelectedHandle = 0;
	std::vector<LUUID> LSelectionContext::SelectedEntities;
	std::unordered_map<ESelectionContext, std::vector<LUUID>> LSelectionContext::SelectionContextMap;

	LSelectionContext& LSelectionContext::Get()
	{
		static LSelectionContext SelectionContext;
		return SelectionContext;
	}

	LSelectionContext::LSelectionContext()
	{
		LOBJECT_REGISTER();
		/* TODO: Remove the test code below. */
		LK_CORE_ASSERT(GetClass() != nullptr, "GetClass failed");
		LK_CORE_DEBUG_TAG("SelectionContext", "Class Name: '{}'", GetClass()->GetName());

		SelectionContextMap.clear();
		SelectionContextMap.insert({ ESelectionContext::Global, {} });
		SelectionContextMap.insert({ ESelectionContext::Scene,  {} });
		SelectionContextMap.insert({ ESelectionContext::ContentBrowser, {} });
	}

}
