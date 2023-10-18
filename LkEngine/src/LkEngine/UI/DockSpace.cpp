#include "LKpch.h"
#include "LkEngine/UI/DockSpace.h"

namespace LkEngine {

    ImVec2 DockSpace::LastViewportSize = ImVec2(0, 0);
    ImGuiID DockSpace::DockSpaceID;
    ImGuiID DockSpace::RenderWindowDockID;
    ImGuiID DockSpace::BottomBarDockID;
    float DockSpace::Sidebar_Left_Ratio = 0.20f;
    float DockSpace::Sidebar_Right_Ratio = 0.20f;
    float DockSpace::TopBottom_Ratio = 0.80f;

    void DockSpace::Init()
    {
    }

    void DockSpace::Begin()
    {
        static bool initialized = false;
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingOverEmpty;
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDocking;

        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMouseInputs;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        if (!initialized)
        {
            ApplyDockSpaceLayout();
            initialized = true;
        }
        // Adjust viewport if window has been resized
        ImVec2 viewport_size = main_viewport->WorkSize;
        if (LastViewportSize.x != viewport_size.x || LastViewportSize.y != viewport_size.y)
        {
            LastViewportSize = viewport_size;
            ApplyDockSpaceLayout();
        }

        ImGui::DockSpaceOverViewport(main_viewport, dockspace_flags, NULL);
    }

    void DockSpace::End()
    {
    }

    void DockSpace::ApplyDockSpaceLayout()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoResize; 
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
        DockSpaceID = ImGui::GetID(LkEngine_DockSpace);
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Reset layout
        ImGui::DockBuilderRemoveNode(DockSpaceID); 
        ImGui::DockBuilderAddNode(DockSpaceID, dockspace_flags);
        ImGui::DockBuilderSetNodeSize(DockSpaceID, viewport->Size);

        // Create layout
        // Top / Bottom
        auto dock_id_top = ImGui::DockBuilderSplitNode(DockSpaceID, ImGuiDir_Up, TopBottom_Ratio, nullptr, &DockSpaceID);
        auto dock_id_bottom = DockSpaceID; // Bottom part is the remaining space in dockspace_ID
        // Split the top horizontally to create the top bar
        auto dock_id_middle = dock_id_top;
        auto dock_id_new_top = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Up, 0.08f, nullptr, &dock_id_top);
        // Split vertically to create sidebars
        auto dock_id_left = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Left, Sidebar_Left_Ratio, nullptr, &dock_id_top);
        auto dock_id_right = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Right, Sidebar_Right_Ratio, nullptr, &dock_id_top);
        auto dock_id_center = dock_id_top;  // Center part is the remaining space in dock_id_top

        // Store dock ID's to be able to fetch size and position of the center positioned render window
        RenderWindowDockID = dock_id_center;
        BottomBarDockID = dock_id_bottom;
        // Build dockspace
        ImGui::DockBuilderDockWindow(TOP_BAR, dock_id_new_top);
        ImGui::DockBuilderDockWindow(SIDEBAR_LEFT, dock_id_left);
        ImGui::DockBuilderDockWindow(SIDEBAR_RIGHT, dock_id_right);
        ImGui::DockBuilderDockWindow(BOTTOM_BAR, dock_id_bottom);
        ImGui::DockBuilderDockWindow(RENDER_WINDOW, dock_id_center);
        ImGui::DockBuilderFinish(DockSpaceID);
    }

}