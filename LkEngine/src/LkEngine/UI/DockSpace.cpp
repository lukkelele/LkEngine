#include "LKpch.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/UI/UILayer.h"
#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    ImGuiID DockSpace::DockSpaceID;
    ImGuiID DockSpace::RenderWindowDockID;
    ImGuiID DockSpace::BottomBarDockID;
    ImGuiID DockSpace::SidebarLeftDockID;
    ImGuiID DockSpace::SidebarRightDockID;
    //ImVec2 DockSpace::LastViewportSize = ImVec2(0, 0);
    //ImVec2 DockSpace::LastCenterWindowSize = ImVec2(0, 0);
    //ImVec2 DockSpace::BottomBarSize = ImVec2(0, 0);
    //ImVec2 DockSpace::CenterWindowSize = ImVec2(0, 0);
    //ImVec2 DockSpace::TopBarSize = ImVec2(0, 0);
    //ImVec2 DockSpace::SidebarLeftSize = ImVec2(0, 0);
    //ImVec2 DockSpace::SidebarRightSize = ImVec2(0, 0);
    float DockSpace::Sidebar_Left_Ratio = 0.20f;
    float DockSpace::Sidebar_Right_Ratio = 0.20f;
    float DockSpace::TopBottom_Ratio = 0.80f;
    bool DockSpace::DockingEnabled = true;
    bool DockSpace::Sidebar_Left_Enabled = true;
    bool DockSpace::Sidebar_Left_Dragged = false;
    bool DockSpace::Sidebar_Right_Enabled = true;
    bool DockSpace::Sidebar_Right_Dragged = false;
    bool DockSpace::ShouldUpdateDockSpace = false;

    void DockSpace::Init()
    {
    }

    void DockSpace::Begin()
    {
        static bool initialized = false;
        static ImVec2 last_center_window_size = CenterWindowSize;

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        if (!DockingEnabled)
        {
            ImGui::DockSpaceOverViewport(main_viewport, ImGuiDockNodeFlags_KeepAliveOnly, NULL);
            return;
        }
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        //dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingOverEmpty;
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDocking;

        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMouseInputs;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;
        window_flags |= ImGuiWindowFlags_NoBackground;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        if (!initialized)
        {
            ApplyDockSpaceLayout();
            initialized = true;
        }

        //ImGui::Begin(LkEngine_DockSpace, NULL, window_flags);
        // Adjust viewport if window has been resized
        //ImVec2 viewport_size = main_viewport->WorkSize;
        //if (LastViewportSize.x != viewport_size.x || LastViewportSize.y != viewport_size.y)
        //{
        //    LastViewportSize = viewport_size;
        //    ApplyDockSpaceLayout();
        //}
        //if (LastCenterWindowSize.x != CenterWindowSize.x || LastCenterWindowSize.y != CenterWindowSize.y)
        //{
        //    LastCenterWindowSize = CenterWindowSize;
        //    ApplyDockSpaceLayout();
        //}

        //ImGui::DockSpaceOverViewport(main_viewport, dockspace_flags, NULL);
    }

    void DockSpace::End()
    {
        if (!DockingEnabled)
            return;
    }

    ImGuiDockNode* DockSpace::GetNode(ImGuiID& id)
    {
        return ImGui::DockBuilderGetNode(id);
    }

    void DockSpace::ApplyDockSpaceLayout()
    {
        //static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoResize; 
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDocking; 
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
        if (!DockingEnabled)
        {
            dockspace_flags = ImGuiDockNodeFlags_KeepAliveOnly;
        }
        DockSpaceID = ImGui::GetID(LkEngine_DockSpace);
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGuiID dock_id_right, dock_id_left;

        // Reset layout
        ImGui::DockBuilderRemoveNode(DockSpaceID); 
        ImGui::DockBuilderAddNode(DockSpaceID, dockspace_flags);
        ImGui::DockBuilderSetNodeSize(DockSpaceID, viewport->Size);

        // Top / Bottom
        auto dock_id_top = ImGui::DockBuilderSplitNode(DockSpaceID, ImGuiDir_Up, TopBottom_Ratio, nullptr, &DockSpaceID);
        auto dock_id_bottom = DockSpaceID; // Bottom part is the remaining space in dockspace_ID
        // Split the top horizontally to create the top bar
        auto dock_id_middle = dock_id_top;
        auto dock_id_new_top = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Up, 0.08f, nullptr, &dock_id_top);
        // Split vertically to create sidebars
        if (Sidebar_Left_Enabled)
            dock_id_left = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Left, Sidebar_Left_Ratio, nullptr, &dock_id_top);
        else
            dock_id_left = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Left, 0, nullptr, &dock_id_top);

            //dock_id_right = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Right, Sidebar_Right_Ratio, nullptr, &dock_id_top);
        if (Sidebar_Right_Enabled)
            dock_id_right = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Right, Sidebar_Right_Ratio, nullptr, &dock_id_top);
        else
            dock_id_right = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Right, 0, nullptr, &dock_id_top);
        auto dock_id_center = dock_id_top;  // Center part is the remaining space in dock_id_top

        // Store dock ID's to be able to fetch size and position of the center positioned render window
        RenderWindowDockID = dock_id_center;
        BottomBarDockID = dock_id_bottom;
        SidebarLeftDockID = dock_id_left;
        SidebarRightDockID = dock_id_right;
        // Build dockspace
        ImGui::DockBuilderDockWindow(UI_TOP_BAR, dock_id_new_top);
        ImGui::DockBuilderDockWindow(UI_SIDEBAR_LEFT, dock_id_left);
        ImGui::DockBuilderDockWindow(UI_SIDEBAR_RIGHT, dock_id_right);
        ImGui::DockBuilderDockWindow(UI_BOTTOM_BAR, dock_id_bottom);
        ImGui::DockBuilderDockWindow(UI_RENDER_WINDOW, dock_id_center);
        ImGui::DockBuilderFinish(DockSpaceID);

        auto center_node = ImGui::DockBuilderGetNode(dock_id_center);
        ImVec2 size = center_node->Size;

        auto top_node = ImGui::DockBuilderGetNode(dock_id_new_top);
        auto left_sidebar_node = ImGui::DockBuilderGetNode(dock_id_left);
        auto right_sidebar_node = ImGui::DockBuilderGetNode(dock_id_right);
        auto bottom_node = ImGui::DockBuilderGetNode(dock_id_bottom);
        TopBarSize = top_node->Size;
        SidebarLeftSize = left_sidebar_node->Size;
        SidebarRightSize = right_sidebar_node->Size;
        BottomBarSize = bottom_node->Size;
        CenterWindowSize = center_node->Size;

        auto window = Window::Get();
        window->SetWidth(size.x);
        window->SetHeight(size.y);
    }

}