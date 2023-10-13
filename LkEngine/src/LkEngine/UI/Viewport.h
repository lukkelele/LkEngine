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
        static void BeginMainWindow();
        static void EndMainWindow();
        static void DrawTopBar();
        static void DrawBottomBar();
        static void DrawLeftSidebar();
        static void DrawRightSidebar();

    private:
        static void BeginViewportDockSpace();
        static void EndViewportDockSpace();
        static void ApplyDockSpaceLayout();
        static void ResetDockSpaceLayout(ImGuiID& dockspace_id, ImGuiDockNodeFlags flags, const ImVec2& size);

    private:
        static bool m_Initialized;
        static unsigned int Viewport_StyleVarCount;
        static float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;
        static bool Sidebar_Left_Open, Sidebar_Right_Open, Bottom_Bar_Open, Top_Bar_Open;

        static ImGuiID m_MainDockSpaceID;
        static ImGuiWindowClass* UIWindowClass;        
        static ImGuiWindowClass* RendererWindowClass;   
        static ImGuiDockNodeFlags m_MainDockSpaceFlags; 
    };


    constexpr float Viewport_Left_Sidepane_Coverage  = 0.18f;
    constexpr float Viewport_Right_Sidepane_Coverage = 0.18f;
    constexpr const char* MainWindow_Label = "##__main-window";
    constexpr const char* Main_DockSpace = "##__main-dockspace";
    constexpr const char* TopBar_Label = "##__top-bar";
    constexpr const char* Bottom_Bar_Label = "##__lower-bar";
    constexpr const char* Left_DockingWindow = "##__left-docking-window";
    constexpr const char* Right_DockingWindow = "##__right-docking-window";
    constexpr const char* Sidebar_Left_Label = "##__left-sidebar";
    constexpr const char* Sidebar_Right_Label = "##__right-sidebar";

}