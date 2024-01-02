#pragma once

#include <imgui/imgui.h>

#include <glm/glm.hpp>


namespace LkEngine {

    // Forward declarations
    class Entity;
    struct TransformComponent;
    struct SpriteComponent;

}

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

    void SetOriginInMiddleOfScreen(TransformComponent& tc);
    void SetOriginInMiddleOfScreen(glm::vec2& vec);

    glm::vec2 Sprite_GetPoint_BottomLeft(const SpriteComponent& sc, Entity& entity);
    glm::vec2 Sprite_GetPoint_BottomRight(const SpriteComponent& sc, Entity& entity);
    glm::vec2 Sprite_GetPoint_TopLeft(const SpriteComponent& sc, Entity& entity);
    glm::vec2 Sprite_GetPoint_TopRight(const SpriteComponent& sc, Entity& entity);

    // Insert UI widgets inside already existing windows on the screen.
    // Is determined by SelectedEntityWindow
    void BeginSubwindow(const char* windowName, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);
    void EndSubwindow();
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);
    const char* GetSelectedEntityWindowName();


}