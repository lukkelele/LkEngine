/*****************************************************************
 * UIDebug
 *
 *****************************************************************/
#pragma once

#if LK_UI_DEBUG_DOCKNODES 
#	define LK_UI_DEBUG_DOCKNODE(Name) \
			if (ImGuiDockNode* DockNode = ImGui::DockBuilderGetNode(ImGui::GetWindowDockID())) \
			{ \
				ImGui::Text("Dock ID: %lld", DockNode->ID); \
				if (DockNode->ParentNode) ImGui::Text("Parent Node: %lld", DockNode->ParentNode->ID); \
				else ImGui::Text("Parent Node: None"); \
				ImGui::Text("Windows: %d", DockNode->Windows.Size); \
				ImGui::Text("Dock Size: (%.2f, %.2f)", DockNode->Size.x, DockNode->Size.y); \
				ImGui::Text("Dock Pos: (%.2f, %.2f)", DockNode->Pos.x, DockNode->Pos.y); \
				ImGui::Separator(); \
			} 
#else
#	define LK_UI_DEBUG_DOCKNODE(Name) 
#endif

#if LK_UI_DEBUG_WINDOWS_ON_HOVER
#	define LK_UI_DEBUG_WINDOW_ON_HOVER(...)                                         \
		if (Debug::UI::bDisplayWindowSizeOnHover)                                   \
		{                                                                           \
			if (ImGui::IsWindowHovered())                                           \
			{                                                                       \
				const ImVec2 WindowSize = ImGui::GetWindowSize();                   \
				ImGui::SetTooltip("Size (%.2f, %.2f)", WindowSize.x, WindowSize.y); \
			}                                                                       \
		}                                                                           
#else
#	define LK_UI_DEBUG_WINDOW_ON_HOVER(...) 
#endif

/** Draw bounding box around an item. */
#define LK_UI_DEBUG_BOUNDING_BOX(Color) \
			if (Debug::UI::bBoundingBoxesOnHover && ImGui::IsItemHovered()) \
			{                                                          \
				const ImVec2 RectMin = ImGui::GetItemRectMin();        \
				const ImVec2 RectMax = ImGui::GetItemRectMax();        \
				ImDrawList* DrawList = ImGui::GetWindowDrawList();     \
				DrawList->AddRect(RectMin, RectMax, Color);            \
			}

