#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui-node-editor/imgui_node_editor.h>

#include <glm/glm.hpp>

#include "LkEngine/Scene/Components.h"


namespace LkEngine 
{
    class LWindow;
    class LEntity;

    static constexpr const char* LkEngine_DockSpace = "##LkEngine-DockSpace";
    
	/// UPDATE/REFACTOR
    static constexpr const char* UI_CORE_VIEWPORT = "##lkengine-core-Viewport";
    static constexpr const char* UI_RENDER_WINDOW = "##lkengine-render-window";
    static constexpr const char* UI_TOP_BAR = "##lkengine-top-bar";
    static constexpr const char* UI_BOTTOM_BAR = "##lkengine-lower-bar";
    static constexpr const char* UI_SELECTED_ENTITY_INFO = "##lkengine-selected-entity-info";
}

namespace LkEngine::UI {

    static constexpr const char* Sidebar1 = "##LkEngine-SIDEBAR-1";
    static constexpr const char* Sidebar2 = "##LkEngine-SIDEBAR-2";

    const char* GenerateID();
    void PushID();
    void PushID(const char* ID);
    void PopID();
    void PopID(const char* ID);
    void Separator(ImVec2 Size, ImVec4 Color);
    bool IsInputEnabled();
    bool IsMouseEnabled();
    bool IsKeyboardEnabled();
    void SetInputEnabled(bool Enabled);

    void Begin(std::string WindowTitle, 
               const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None, 
               bool* Open = nullptr);

    void Begin(const char* WindowTitle, 
               const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None, 
               bool* Open = nullptr);

    void Begin(ImGuiWindowFlags WindowFlags, 
               bool* Open = nullptr);

    void End();

    /* Core Viewport, also known as Main Window */
    void BeginCoreViewport();
    void EndCoreViewport();

    /// REFACTOR
    /**
     * Insert UI widgets inside already existing windows on the screen.
     * Is determined by SelectedEntityWindow
     */
    void BeginSubwindow(const char* WindowName, 
                        const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None);
    void EndSubwindow();
    bool IsWindowFocused(const char* WindowName, 
                         const bool CheckRootWindow);
    //const char* GetSelectedEntityWindowName();

    void BeginDockSpace(const char* DockspaceID);
    void BeginViewport(const char* ViewportID, LWindow* WindowRef, ImGuiViewport* Viewport);

    /// FIXME
    void Image(const TObjectPtr<LTexture2D>& texture, 
               const ImVec2& Size, 
               const ImVec2& UV0 = ImVec2(0, 0), 
               const ImVec2& UV1 = ImVec2(1, 1), 
               const ImVec4& TintColor = ImVec4(1, 1, 1, 1), 
               const ImVec4& BorderColor = ImVec4(0, 0, 0, 0));

    void Image(const TObjectPtr<LTexture2D>& Image, 
               const glm::vec2& Size, 
               const glm::vec2& UV0 = glm::vec2(0, 0), 
               const glm::vec2& UV1 = glm::vec2(1, 1), 
               const glm::vec4& TintColor = glm::vec4(1, 1, 1, 1), 
               const glm::vec4& BorderColor = glm::vec4(0, 0, 0, 0));

    void Image(const TObjectPtr<LImage2D>& Image, 
               const ImVec2& Size, 
               const ImVec2& UV0 = ImVec2(0, 0), 
               const ImVec2& UV1 = ImVec2(1, 1), 
               const ImVec4& TintColor = ImVec4(1, 1, 1, 1), 
               const ImVec4& BorderColor = ImVec4(0, 0, 0, 0));

    void Image(const TObjectPtr<LImage2D>& Image, 
               const glm::vec2& Size, 
               const glm::vec2& UV0 = glm::vec2(0, 0), 
               const glm::vec2& UV1 = glm::vec2(1, 1), 
               const glm::vec4& TintColor = glm::vec4(1, 1, 1, 1), 
               const glm::vec4& BorderColor = glm::vec4(0, 0, 0, 0));
    // ~

    void PushStyleVar(const ImGuiStyleVar StyleVar, const ImVec2& Var);
    void PushStyleVar(const ImGuiStyleVar StyleVar, const glm::vec2& Var);
    void PopStyleVar(uint8_t VarsToPop = 1);

    void PushStyleColor(const ImGuiCol ColorVar, const ImVec4& Color);
    void PushStyleColor(const ImGuiCol ColorVar, const glm::vec4& Color);
    void PopStyleColor(const uint8_t VarsToPop = 1);
    void PopStyleStack(); // Pop entire stack 
   
    /* Flags. */
    extern ImGuiWindowFlags CoreViewportFlags;
    extern ImGuiWindowFlags HostWindowFlags;
    extern ImGuiWindowFlags SidebarFlags;
    extern ImGuiWindowFlags SidebarDockspaceFlags;
    extern ImGuiWindowFlags MenuBarFlags;
    extern ImGuiWindowFlags TabBarFlags;
    extern ImGuiWindowFlags ViewportTextureFlags;
    extern ImGuiDockNodeFlags DockspaceFlags; 
    //extern const char* SelectedEntityWindow; /// FIXME

    constexpr const char* VIEWPORT_TEXTURE = "LkEngine-ViewportTexture";

}
