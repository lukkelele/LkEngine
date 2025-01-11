#include "LKpch.h"
#include "SelectionContext.h"


namespace LkEngine {

	LUUID LSelectionContext::SelectedHandle = 0;
	std::unordered_map<ESelectionContext, std::vector<LUUID>> LSelectionContext::SelectionContextMap;

	LSelectionContext& LSelectionContext::Get()
	{
		static LSelectionContext SelectionContext;
		return SelectionContext;
	}

	LSelectionContext::LSelectionContext()
	{
		LOBJECT_REGISTER();

		SelectionContextMap.clear();
		//SelectionContextMap.insert({ ESelectionContext::Global, {} });
		SelectionContextMap.insert({ ESelectionContext::Scene,  {} });
		SelectionContextMap.insert({ ESelectionContext::ContentBrowser, {} });
	}

}
