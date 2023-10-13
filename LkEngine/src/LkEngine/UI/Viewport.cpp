#include "LKpch.h"
#include "LkEngine/UI/Viewport.h"
#include "LkEngine/UI/UI.h"


namespace LkEngine::UI {

    // TODO: CLEAN THIS UP
    unsigned int Viewport::Viewport_StyleVarCount = 0;
    bool Viewport::m_Initialized = false;
    float Viewport::Sidebar_Left_Ratio = 0.20f;
    float Viewport::Sidebar_Right_Ratio = 0.20f;
    float Viewport::TopBottom_Ratio = 0.80f;
    ImGuiID Viewport::m_MainDockSpaceID = NULL;
    ImGuiDockNodeFlags Viewport::m_MainDockSpaceFlags = NULL;

    void Viewport::Init()
    {
        m_MainDockSpaceFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;
        UIWindowClass = new ImGuiWindowClass();
        UIWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;
        RendererWindowClass = new ImGuiWindowClass();
        RendererWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_PassthruCentralNode;
    }

    void Viewport::BeginViewport()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!m_Initialized)
        {
            m_MainDockSpaceID = ImGui::GetID(Main_DockSpace); 
            ApplyDockSpaceLayout();
            m_Initialized = true;
        }

        BeginViewportDockSpace();
    }

    void Viewport::EndViewport()
    {
        EndViewportDockSpace();
    }


    void Viewport::BeginViewportDockSpace()
    {
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        // Setup dockspace over viewport
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        Viewport_StyleVarCount += 3;
        ImGuiWindowFlags viewport_window_flags = 0;
        viewport_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
        viewport_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags viewport_dock_flags = 0;
        viewport_dock_flags |= ImGuiDockNodeFlags_PassthruCentralNode;

        ImGui::SetNextWindowPos(main_viewport->WorkPos);
        ImGui::SetNextWindowSize(main_viewport->WorkSize);
        ImGui::SetNextWindowViewport(main_viewport->ID);
        ImGui::Begin(Main_DockSpace, NULL, viewport_window_flags);
        ImGui::DockSpace(ImGui::GetID(Main_DockSpace), ImVec2(0, 0), m_MainDockSpaceFlags);
        ImGui::PopStyleVar(Viewport_StyleVarCount);
    }

    void Viewport::EndViewportDockSpace()
    {
        ImGui::End(); // Main_Dockspace
        Viewport_StyleVarCount = 0;
    }

    void Viewport::ApplyDockSpaceLayout()
    {
        LK_ASSERT(m_MainDockSpaceID != NULL);
        m_MainDockSpaceID = ImGui::GetID(Main_DockSpace);
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        // Reset layout
        ImGui::DockBuilderRemoveNode(m_MainDockSpaceID); 
        ImGui::DockBuilderAddNode(m_MainDockSpaceID, m_MainDockSpaceFlags);
        ImGui::DockBuilderSetNodeSize(m_MainDockSpaceID, viewport->Size);

        // Create layout
        // Top / Bottom
        auto dock_id_top = ImGui::DockBuilderSplitNode(m_MainDockSpaceID, ImGuiDir_Up, TopBottom_Ratio, nullptr, &m_MainDockSpaceID);
        auto dock_id_bottom = m_MainDockSpaceID; // Bottom part is the remaining space in dockspace_ID
        // Split the top horizontally to create the top bar
        auto dock_id_middle = dock_id_top;
        auto dock_id_new_top = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Up, 0.10f, nullptr, &dock_id_top);
        // Split vertically to create sidebars
        auto dock_id_left = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Left, Sidebar_Left_Ratio, nullptr, &dock_id_top);
        auto dock_id_right = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Right, Sidebar_Right_Ratio, nullptr, &dock_id_top);
        auto dock_id_center = dock_id_top;  // Center part is the remaining space in dock_id_top

        // Build dockspace
        ImGui::DockBuilderDockWindow(TopBar_Label, dock_id_new_top);
        ImGui::DockBuilderDockWindow(Sidebar_Left_Label, dock_id_left);
        ImGui::DockBuilderDockWindow(Sidebar_Right_Label, dock_id_right);
        ImGui::DockBuilderDockWindow(Bottom_Bar_Label, dock_id_bottom);
        ImGui::DockBuilderDockWindow(MainRenderWindow_Label, dock_id_center);
        ImGui::DockBuilderFinish(m_MainDockSpaceID);
    }


}