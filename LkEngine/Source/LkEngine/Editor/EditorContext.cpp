#include "LKpch.h"
#include "EditorContext.h"

#include "LkEngine/Renderer/UI/UICore.h"


namespace LkEngine {

	void LEditorContext::Cache()
	{
		ImGuiContext& G = *GImGui;
		EditorContext.LastFrameFocusedWindowID = EditorContext.FocusedWindowID;
		EditorContext.LastFrameFocusedWindowName = EditorContext.FocusedWindowName;
		EditorContext.LastFrameHoveredWindowID = EditorContext.HoveredWindowID;
		EditorContext.LastFrameHoveredWindowName = EditorContext.HoveredWindowName;

		EditorContext.bLastFrameEditorViewportFocused = (EditorContext.LastFrameFocusedWindowID > 0) 
			&& (EditorContext.LastFrameFocusedWindowID == UI::GetWindowID(PanelID::EditorViewport));
		EditorContext.bLastFrameEditorViewportHovered = (EditorContext.LastFrameHoveredWindowID > 0) 
			&& (EditorContext.LastFrameHoveredWindowID == UI::GetWindowID(PanelID::EditorViewport));

		/* Update cached entries. */
		EditorContext.HoveredWindowID = G.HoveredWindow ? G.HoveredWindow->ID : 0;
		EditorContext.HoveredWindowName = G.HoveredWindow ? G.HoveredWindow->Name : "NULL";
	}

}
