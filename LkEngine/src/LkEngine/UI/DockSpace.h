#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine {

    constexpr const char* LkEngine_DockSpace = "##__main-dockspace";
    constexpr const char* RENDER_WINDOW = "##__main-render-window";
    constexpr const char* TOP_BAR = "##__top-bar";
    constexpr const char* BOTTOM_BAR = "##__lower-bar";
    constexpr const char* SIDEBAR_LEFT = "##__left-sidebar";
    constexpr const char* SIDEBAR_RIGHT = "##__right-sidebar";

    class DockSpace
    {
    public:
        DockSpace() = default;
        ~DockSpace() = default;

        static void Init();
        static void Begin();
        static void End();

    private:
        static void ApplyDockSpaceLayout();

    public:
        static ImVec2 LastViewportSize;
        static ImGuiID DockSpaceID, RenderWindowDockID, BottomBarDockID;
        static float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;
    };

}