#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine::UI {


    bool UpdateWindowManualResize(ImGuiWindow* imgui_window, ImVec2& new_size, ImVec2& new_pos);

}