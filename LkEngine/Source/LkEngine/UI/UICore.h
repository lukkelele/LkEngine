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

    // Core Viewport, aka main screen/window
    void BeginCoreViewport();
    void EndCoreViewport();

    // Insert UI widgets inside already existing windows on the screen.
    // Is determined by SelectedEntityWindow
    void BeginSubwindow(const char* windowName, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);
    void EndSubwindow();
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);
    const char* GetSelectedEntityWindowName();

    void BeginDockSpace(const char* dockspaceID);
    void BeginViewport(const char* viewportID, Window* window, ImGuiViewport* viewport);

    // Implemented in either OpenGLImgui.cpp or VulkanImGui.cpp
    void Image(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    void Image(const Ref<Texture2D>& image, const glm::vec2& size, const glm::vec2& uv0 = glm::vec2(0, 0), const glm::vec2& uv1 = glm::vec2(1, 1), const glm::vec4& tint_col = glm::vec4(1, 1, 1, 1), const glm::vec4& border_col = glm::vec4(0, 0, 0, 0));
    void Image(const Ref<Image2D>& image, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    void Image(const Ref<Image2D>& image, const glm::vec2& size, const glm::vec2& uv0 = glm::vec2(0, 0), const glm::vec2& uv1 = glm::vec2(1, 1), const glm::vec4& tint_col = glm::vec4(1, 1, 1, 1), const glm::vec4& border_col = glm::vec4(0, 0, 0, 0));

    void PushStyleVar(ImGuiStyleVar styleVar, const ImVec2& var);
    void PushStyleVar(ImGuiStyleVar styleVar, const glm::vec2& var);
    void PopStyleVar(uint8_t popVars = 1);

    void PushStyleColor(ImGuiCol colorVar, const ImVec4& color);
    void PushStyleColor(ImGuiCol colorVar, const glm::vec4& color);
    void PopStyleColor(uint8_t popVars = 1);
    void PopStyleStack(); // Pop entire stack 
   

    extern ImGuiWindowFlags CoreViewportFlags;
    extern ImGuiWindowFlags HostWindowFlags;
    extern ImGuiWindowFlags SidebarFlags;
    extern ImGuiWindowFlags SidebarDockspaceFlags;
    extern ImGuiWindowFlags MenuBarFlags;
    extern ImGuiWindowFlags TabBarFlags;
    extern ImGuiWindowFlags ViewportTextureFlags;
    extern ImGuiDockNodeFlags DockspaceFlags; 
    extern const char* SelectedEntityWindow;

    constexpr const char* VIEWPORT_TEXTURE = "LkEngine-ViewportTexture";

}
