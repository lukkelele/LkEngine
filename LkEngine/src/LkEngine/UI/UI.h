#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine {

    constexpr const char* RENDER_WINDOW = "##__main-render-window";
    constexpr const char* Main_DockSpace = "##__main-dockspace";
    constexpr const char* TOP_BAR = "##__top-bar";
    constexpr const char* BOTTOM_BAR = "##__lower-bar";
    constexpr const char* SIDEBAR_LEFT = "##__left-sidebar";
    constexpr const char* SIDEBAR_RIGHT = "##__right-sidebar";


    class UI
    {
    public:
        UI() = default;
        ~UI() = default;

        static void Init();
        static void BeginMainRenderWindow();
        static void EndMainRenderWindow();
        static void TopBar();
        static void BottomBar();
        static void LeftSidebar();
        static void RightSidebar();
        static void AppInfo();

        // -- Dockspace --
        static void BeginViewportDockSpace();
        static void EndViewportDockSpace();
        static void ApplyDockSpaceLayout();

    public:
        static ImGuiID MainDockSpaceID, RenderWindowDockID, BottomBarDockID;
        //static ImGuiID RenderWindowDockID;
        static ImVec2 LastViewportSize;
        static ImGuiWindowClass* UIWindowClass;        
        static ImGuiWindowClass* RendererWindowClass;   
        static ImGuiWindowClass* ExternalWindowClass;   
        static bool Initialized, ShowImGuiDemoWindow;
        static float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;
    };

}