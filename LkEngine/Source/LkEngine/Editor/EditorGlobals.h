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


	namespace UI
	{
		namespace Debug
		{
			extern bool bDisplayWindowSizeOnHover;
			extern bool bBoundingBoxesOnHover;
			extern bool bGridBorders;
			extern int  GridBorders;

			namespace ContentBrowser 
			{
				extern bool bDrawOutlinerBorders;
				extern ImVec4 OutlinerBorderColor;
			}
		}
	}

}
