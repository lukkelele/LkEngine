#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/Delegate/Delegate.h"


namespace LkEngine {

	LK_DECLARE_MULTICAST_DELEGATE(FEditorOnSelectionChanged, const LObject&);

	/** 
	 * GEditorOnSelectionChanged
	 * 
	 *  Updates on selection changes.
	 */
	static FEditorOnSelectionChanged GEditorOnSelectionChanged;


	namespace Debug 
	{
		namespace UI 
		{
			extern bool bDisplayWindowSizeOnHover;
			extern bool bBoundingBoxesOnHover;

			namespace ContentBrowser 
			{
				extern bool bDrawOutlinerBorders;
				extern ImVec4 OutlinerBorderColor;
			}
		}
	}

}
