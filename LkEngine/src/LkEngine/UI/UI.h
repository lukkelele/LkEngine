#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine::UI {

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


    extern ImGuiWindowClass* UIWindowClass;        
    extern ImGuiWindowClass* RendererWindowClass;   
    extern ImGuiID MainDockSpaceID;
    extern ImGuiDockNodeFlags MainDockSpaceFlags; 
    extern unsigned int Viewport_StyleVarCount;
    extern float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;
    extern bool Initialized;


    void Init();
    void BeginMainRenderWindow();
    void EndMainRenderWindow();
    void TopBar();
    void BottomBar();
    void LeftSidebar();
    void RightSidebar();
    // -- Dockspace --
    void BeginViewportDockSpace();
    void EndViewportDockSpace();
    void ApplyDockSpaceLayout();


}