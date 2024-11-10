#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


#define LK_UI_ENTITY_ADD_PROPERTY(_PROPERTY) _PROPERTY

namespace LkEngine::UI {

    bool UpdateWindowManualResize(ImGuiWindow* imgui_window, ImVec2& new_size, ImVec2& new_pos);

}