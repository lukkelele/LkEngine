#pragma once

#include <imgui/imgui.h>


namespace LkEngine::UI {

    extern ImGuiWindowFlags CoreViewportFlags;
    extern ImGuiWindowFlags HostWindowFlags;
    extern ImGuiWindowFlags SidebarFlags;
    extern ImGuiWindowFlags TopbarFlags;
    extern ImGuiDockNodeFlags DockspaceFlags; 
    extern const char* SelectedEntityWindow;

    const char* GenerateID();
    void PushID();
    void PopID();
    void Separator(ImVec2 size, ImVec4 color);
    bool IsInputEnabled();
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);
    void SetInputEnabled(bool enabled);
    const char* GetSelectedEntityWindowName();

    // Insert UI widgets inside already existing windows on the screen.
    // Is determined by SelectedEntityWindow
    void BeginSubwindow(const char* windowName);
    void EndSubwindow();


}