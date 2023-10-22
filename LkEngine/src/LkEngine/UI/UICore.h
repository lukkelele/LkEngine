#pragma once
#include <imgui/imgui.h>


namespace LkEngine::UI {

    const char* GenerateID();
    void PushID();
    void PopID();
    void Separator(ImVec2 size, ImVec4 color);

    bool IsInputEnabled();
    void SetInputEnabled(bool enabled);
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);

}