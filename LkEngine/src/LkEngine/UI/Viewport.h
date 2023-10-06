#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#define UI_SIDEBAR_LEFT  0
#define UI_SIDEBAR_RIGHT 1
#define WINDOW_ALWAYS_OPEN (bool*)true


namespace LkEngine::UI {

    constexpr ImGuiWindowFlags SidebarFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar 
        | ImGuiWindowFlags_NoTitleBar;
        // | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
    constexpr const char* MainWindow_Label = "##__main-window";
    constexpr const char* Main_Dockspace = "##__main-dockspace";
    constexpr const char* Left_Sidebar_Label = "##__left-sidebar";
    constexpr const char* Right_Sidebar_Label = "##__right-sidebar";
    constexpr const char* Left_DockingWindow = "##__left-docking-window";
    constexpr const char* Right_DockingWindow = "##__right-docking-window";
    constexpr const char* Bottom_Bar_Label = "##__lower-bar";

    class Viewport
    {
    public:
        Viewport() = default;
        ~Viewport() = default;

        static void BeginViewportBase();
        static void EndViewportBase();
        static void BeginMainWindow();
        static void EndMainWindow();
        static void DrawLeftSidebar();
        static void DrawRightSidebar();
        static void DrawBottomBar();

    private:
        static unsigned int StyleStackCount;
        static unsigned int MainWindow_StyleStackCount;
        static bool Left_Sidebar_Open, Right_Sidebar_Open, Bottom_Bar_Open;
        static float Left_Sidebar_MinWidth;
        static float Left_Sidebar_Width;

    };

}