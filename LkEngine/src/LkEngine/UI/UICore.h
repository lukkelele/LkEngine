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
    void PushID(const char* id);
    void PopID();
    void PopID(const char* id);
    void Separator(ImVec2 size, ImVec4 color);
    bool IsInputEnabled();
    bool IsMouseEnabled();
    bool IsKeyboardEnabled();
    void SetInputEnabled(bool enabled);

    // Insert UI widgets inside already existing windows on the screen.
    // Is determined by SelectedEntityWindow
    void BeginSubwindow(const char* windowName);
    void EndSubwindow();
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);
    const char* GetSelectedEntityWindowName();


}