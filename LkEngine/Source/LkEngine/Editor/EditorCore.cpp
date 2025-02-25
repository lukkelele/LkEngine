#include "LKpch.h"
#include "EditorCore.h"

#include "LkEngine/Renderer/UI/UICore.h"


namespace LkEngine {

	namespace UI::Debug 
	{
		bool bDisplayWindowSizeOnHover = false;
		bool bBoundingBoxesOnHover = false;
		bool bGridBorders = false;
		int  GridBorders = (int)UI::EBorder::None;

		namespace ContentBrowser 
		{
			bool bDrawOutlinerBorders = false;
			ImVec4 OutlinerBorderColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); /* RGBA */
		}
	}

}
