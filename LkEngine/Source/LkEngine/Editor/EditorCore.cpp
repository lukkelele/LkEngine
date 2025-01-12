#include "LKpch.h"
#include "EditorCore.h"


namespace LkEngine {

	namespace Debug::UI 
	{
		bool bDisplayWindowSizeOnHover = false;
		bool bBoundingBoxesOnHover = false;

		namespace ContentBrowser 
		{
			bool bDrawOutlinerBorders = false;
			ImVec4 OutlinerBorderColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); /* RGBA */
		}
	}

}
