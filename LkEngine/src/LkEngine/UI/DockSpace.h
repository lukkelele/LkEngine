#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine {

    constexpr const char* LkEngine_DockSpace = "##__main-dockspace";

    class DockSpace
    {
    public:
        DockSpace() = default;
        ~DockSpace() = default;

        static void Init();
        static void Begin();
        static void End();
        static void ApplyDockSpaceLayout();

        static ImGuiDockNode* GetNode(ImGuiID& id);

    public:
        //static ImVec2 LastViewportSize, LastCenterWindowSize;
        //static ImVec2 BottomBarSize, TopBarSize, SidebarLeftSize, SidebarRightSize, CenterWindowSize;
        static ImGuiID DockSpaceID, RenderWindowDockID, BottomBarDockID, SidebarLeftDockID, SidebarRightDockID;
        inline static ImVec2 BottomBarSize = ImVec2(0, 0);
        inline static ImVec2 CenterWindowSize = ImVec2(0, 0);
        inline static ImVec2 TopBarSize = ImVec2(0, 0);
        inline static ImVec2 SidebarLeftSize = ImVec2(0, 0);
        inline static ImVec2 SidebarRightSize = ImVec2(0, 0);
        static float Sidebar_Left_Ratio, Sidebar_Right_Ratio, TopBottom_Ratio;
        static bool Sidebar_Left_Enabled, Sidebar_Right_Enabled;
        static bool Sidebar_Left_Dragged, Sidebar_Right_Dragged;
        static bool DockingEnabled, ShouldUpdateDockSpace;
    };

}