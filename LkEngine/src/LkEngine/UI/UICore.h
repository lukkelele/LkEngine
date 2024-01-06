#pragma once

#include <imgui/imgui.h>

#include <glm/glm.hpp>

#include "LkEngine/Scene/Components.h"

namespace LkEngine {

    // Forward declarations
    class Entity;
    //struct TransformComponent;
    //struct SpriteComponent;

}

namespace LkEngine {

	enum OriginPlacement
	{
		None = 0,
		BottomLeft,
		Center,
		TopRight
	};

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

    // Insert UI widgets inside already existing windows on the screen.
    // Is determined by SelectedEntityWindow
    void BeginSubwindow(const char* windowName, ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);
    void EndSubwindow();
    bool IsWindowFocused(const char* windowName, const bool checkRootWindow);
    const char* GetSelectedEntityWindowName();


    // Implemented in either OpenGLImgui.cpp or VulkanImGui.cpp
    void Image(const s_ptr<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
}
