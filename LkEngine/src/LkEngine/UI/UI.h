#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine::UI {

    constexpr const char* RENDER_WINDOW = "##__main-render-window";
    constexpr const char* Main_DockSpace = "##__main-dockspace";
    constexpr const char* TOP_BAR = "##__top-bar";
    constexpr const char* BOTTOM_BAR = "##__lower-bar";
    constexpr const char* SIDEBAR_LEFT = "##__left-sidebar";
    constexpr const char* SIDEBAR_RIGHT = "##__right-sidebar";

    extern ImGuiWindowClass* UIWindowClass;        
    extern ImGuiWindowClass* RendererWindowClass;   
    extern ImGuiWindowClass* ExternalWindowClass;   
    extern ImGuiID MainDockSpaceID;
    extern unsigned int Viewport_StyleVarCount;
    extern float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;
    extern ImVec2 LastViewportSize;
    extern bool Initialized, ShowImGuiDemoWindow;


    void Init();
    void BeginMainRenderWindow();
    void EndMainRenderWindow();
    void TopBar();
    void BottomBar();
    void LeftSidebar();
    void RightSidebar();
    void AppInfo();

    // -- Dockspace --
    void BeginViewportDockSpace();
    void EndViewportDockSpace();
    void ApplyDockSpaceLayout();


}