#include "LKpch.h"
#include "SelectionContext.h"


namespace LkEngine {

	LUUID LSelectionContext::SelectedEntityID = 0;
	std::vector<LUUID> LSelectionContext::SelectedEntities;
	std::unordered_map<ESelectionContext, std::vector<LUUID>> LSelectionContext::SelectionContextMap;

	TObjectPtr<LObject> GSelectedObject{}; /* TODO: Re-evaluate */

	LSelectionContext& LSelectionContext::Get()
	{
		static LSelectionContext SelectionContext;
		return SelectionContext;
	}

	LSelectionContext::LSelectionContext()
	{
		LOBJECT_REGISTER();
		LK_CORE_ASSERT(GetClass() != nullptr, "GetClass failed");
		LK_CORE_INFO_TAG("SelectionContext", "Class Name: '{}'", GetClass()->GetName());

		SelectionContextMap.clear();
		SelectionContextMap.insert({ ESelectionContext::Global, {} });
		SelectionContextMap.insert({ ESelectionContext::Scene,  {} });
		SelectionContextMap.insert({ ESelectionContext::ContentBrowser, {} });
	}

}
