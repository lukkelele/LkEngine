#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui-node-editor/imgui_node_editor.h>

#include <glm/glm.hpp>

#include "LkEngine/Scene/Components.h"


// Forward declarations
namespace LkEngine {
    class Window;
    class Entity;
}

namespace LkEngine {

    constexpr const char* LkEngine_DockSpace = "##LkEngine-DockSpace";


}

namespace LkEngine::UI {

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

    void Begin(std::string windowTitle, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None, bool* open = nullptr);
    void Begin(const char* windowTitle, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None, bool* open = nullptr);
    void Begin(ImGuiWindowFlags windowFlags, bool* open = nullptr);
    void End();

    // Insert UI widgets inside already existing windows on the screen.
    // Is determined by SelectedEntityWindow
    void BeginSubwindow(const char* windowName, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);
    void EndSubwindow();
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);
    const char* GetSelectedEntityWindowName();

    void BeginDockSpace(const char* dockspaceID);
    void BeginViewport(const char* viewportID, Window* window, ImGuiViewport* viewport);

    // Implemented in either OpenGLImgui.cpp or VulkanImGui.cpp
    void Image(const s_ptr<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));


    extern ImGuiWindowFlags CoreViewportFlags;
    extern ImGuiWindowFlags HostWindowFlags;
    extern ImGuiWindowFlags SidebarFlags;
    extern ImGuiWindowFlags MenuBarFlags;
    extern ImGuiWindowFlags TabBarFlags;
    extern ImGuiDockNodeFlags DockspaceFlags; 
    extern const char* SelectedEntityWindow;

}
