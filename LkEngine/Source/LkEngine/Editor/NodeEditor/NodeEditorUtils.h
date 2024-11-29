#pragma once

#include <imgui/imgui.h>
#include <imgui-node-editor/imgui_node_editor.h>


#if defined(LK_USE_MODDED_IMGUI)
namespace ImGui {

    IMGUI_API void BeginHorizontal(const char* str_id, const ImVec2& size = ImVec2(0, 0));
    IMGUI_API void BeginHorizontal(const void* ptr_id, const ImVec2& size = ImVec2(0, 0));
    IMGUI_API void EndHorizontal();
    IMGUI_API void BeginVertical(const char* str_id, const ImVec2& size = ImVec2(0, 0));
    IMGUI_API void BeginVertical(const void* ptr_id, const ImVec2& size = ImVec2(0, 0));
    IMGUI_API void EndVertical();
    IMGUI_API void Spring(float weight = 1.0f, float spacing = -1.0f);

}
#endif

namespace LkEngine::UI::Nodes {

	enum class IconType : ImU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond };

	void Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));
	void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);

}
