#pragma once
#include "LkEngine/Core/Base.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#define UI_SIDEBAR_LEFT  0
#define UI_SIDEBAR_RIGHT 1


namespace LkEngine::UI {

    class Viewport
    {
    public:
        Viewport() = default;
        ~Viewport() = default;

        static void Init();
        static void BeginViewport();
        static void EndViewport();

    private:
        static void BeginViewportDockSpace();
        static void EndViewportDockSpace();
        static void ApplyDockSpaceLayout();

    private:
        static bool m_Initialized;
        static unsigned int Viewport_StyleVarCount;
        static float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;

        static ImGuiID m_MainDockSpaceID;
        static ImGuiDockNodeFlags m_MainDockSpaceFlags; 
    };


    constexpr float Viewport_Left_Sidepane_Coverage  = 0.18f;
    constexpr float Viewport_Right_Sidepane_Coverage = 0.18f;
    constexpr const char* MainRenderWindow_Label = "##__main-render-window";
    constexpr const char* Main_DockSpace = "##__main-dockspace";
    constexpr const char* TopBar_Label = "##__top-bar";
    constexpr const char* Bottom_Bar_Label = "##__lower-bar";
    constexpr const char* Left_DockingWindow = "##__left-docking-window";
    constexpr const char* Right_DockingWindow = "##__right-docking-window";
    constexpr const char* Sidebar_Left_Label = "##__left-sidebar";
    constexpr const char* Sidebar_Right_Label = "##__right-sidebar";

}