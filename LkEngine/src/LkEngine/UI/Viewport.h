#pragma once
#include "LkEngine/Core/Base.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#define UI_SIDEBAR_LEFT  0
#define UI_SIDEBAR_RIGHT 1
#define WINDOW_ALWAYS_OPEN (bool*)true

namespace LkEngine::UI {
    constexpr float Viewport_TopBottom_Split = 0.80f; 
    constexpr float Viewport_Left_Sidepane_Coverage  = 0.22f;
    constexpr float Viewport_Right_Sidepane_Coverage = 0.14f;
    constexpr const char* MainWindow_Label = "##__main-window";
    constexpr const char* Main_Dockspace = "##__main-dockspace";
    constexpr const char* Sidebar_Left_Label = "##__left-sidebar";
    constexpr const char* Sidebar_Right_Label = "##__right-sidebar";
    constexpr const char* Left_DockingWindow = "##__left-docking-window";
    constexpr const char* Right_DockingWindow = "##__right-docking-window";
    constexpr const char* TopBar_Label = "##__top-bar";
    constexpr const char* Bottom_Bar_Label = "##__lower-bar";
    constexpr ImGuiWindowFlags Viewport_SidebarFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;
}

namespace LkEngine::UI {

    class Viewport
    {
    public:
        Viewport();
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

    // ---- Dockspace ----
    private:
        static void ApplyDockspaceLayout();
        static void ResetDockspaceLayout(ImGuiID& dockspace_id, ImGuiDockNodeFlags flags, const ImVec2& size);

    private:
        static ImGuiWindowClass* DockWindowClass; // Overridden flags
        static ImGuiID m_MainDockspaceID;
        static ImGuiDockNodeFlags m_MainDockspaceFlags; // = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_PassthruCentralNode;
    private:
        static bool m_Initialized;
        static unsigned int Viewport_StyleStackCount;
        static unsigned int MainWindow_StyleStackCount;
        static float Sidebar_Left_Ratio, Sidebar_Right_Ratio;
        static float Sidebar_Left_MinWidth, Sidebar_Left_Width;
        static float TopBottom_Ratio;
        static bool Sidebar_Left_Open, Sidebar_Right_Open, Bottom_Bar_Open, Top_Bar_Open;

    };

}