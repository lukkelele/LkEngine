#include "LkEngine/UI/Viewport.h"


namespace LkEngine::UI {

    // TODO: CLEAN THIS UP
    unsigned int Viewport::Viewport_StyleStackCount = 0;
    unsigned int Viewport::MainWindow_StyleStackCount = 0;
    bool Viewport::Top_Bar_Open= true;
    bool Viewport::Bottom_Bar_Open = true;
    bool Viewport::Sidebar_Left_Open = true;
    bool Viewport::Sidebar_Right_Open = true;
    bool Viewport::m_Initialized = false;
    float Viewport::Sidebar_Left_MinWidth = 160.0f;
    float Viewport::Sidebar_Left_Width = 180.0f;
    float Viewport::Sidebar_Left_Ratio = Viewport_Left_Sidepane_Coverage;
    float Viewport::Sidebar_Right_Ratio = Viewport_Right_Sidepane_Coverage;
    float Viewport::TopBottom_Ratio = Viewport_TopBottom_Split;
    ImGuiID Viewport::m_MainDockspaceID = NULL;
    ImGuiDockNodeFlags Viewport::m_MainDockspaceFlags = NULL;
    ImGuiWindowClass* Viewport::DockWindowClass = nullptr;

    Viewport::Viewport()
    {
    }

    void Viewport::Init()
    {
        m_MainDockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        DockWindowClass = new ImGuiWindowClass();
        DockWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;
        LK_ASSERT(DockWindowClass);
        m_Initialized = true;
    }

    void Viewport::BeginViewport()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        //ImGuiID dockspace_ID = ImGui::GetID("ViewportDocking");
        //ImGuiID lower_dockspace_ID = ImGui::GetID("lower-dockspace");

        if (!m_Initialized)
        {
            m_MainDockspaceID = ImGui::GetID("__ID__MainDockspaceID");
            m_MainDockspaceFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoCloseButton;
            ApplyDockspaceLayout();
            m_Initialized = true;
        }

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration 
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;

        //ImGui::DockSpaceOverViewport(main_viewport, m_MainDockspaceFlags);
        //StyleStackCount++;

        // Setup dockspace over viewport
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        Viewport_StyleStackCount++;
        ImGui::SetNextWindowSize(main_viewport->Size);
        ImGui::SetNextWindowPos(main_viewport->Pos);
        ImGui::Begin(Main_Dockspace, (bool*)true, flags);
        ImGui::DockSpace(ImGui::GetID(Main_Dockspace), main_viewport->Size, m_MainDockspaceFlags);

        DrawTopBar();
        DrawBottomBar();
        DrawLeftSidebar();
        DrawRightSidebar();

        ImGui::PopStyleVar(Viewport_StyleStackCount);

        BeginMainWindow();
    }

    void Viewport::EndViewport()
    {
        EndMainWindow();
        ImGui::End();
        Viewport_StyleStackCount = 0;
    }

    void Viewport::BeginMainWindow()
    {
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags MainWindow_Flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;
        MainWindow_Flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowClass(DockWindowClass);
        ImGui::Begin(MainWindow_Label, WINDOW_ALWAYS_OPEN, MainWindow_Flags);
   
        ImGui::Text("Main Window");
        ImGui::SeparatorText("Main Window Content");
        static float v_angle = 0.0f;
        ImGui::SliderAngle("Angle Slider", &v_angle, -360.0f, 360.0f, "%.00f");
    }

    void Viewport::EndMainWindow()
    {
        ImGui::End();
        ImGui::PopStyleVar(MainWindow_StyleStackCount);
        MainWindow_StyleStackCount = 0;
    }


    void Viewport::DrawLeftSidebar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        float Sidebar_Left_Height = viewport->WorkSize.y;
        ImVec2 Sidebar_Left_Size = ImVec2(Sidebar_Left_Width, Sidebar_Left_Height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowClass(DockWindowClass);
        ImGui::SetNextWindowSize(ImVec2(Sidebar_Left_Width, Sidebar_Left_Height));
        
        ImGui::Begin(Sidebar_Left_Label, &Sidebar_Left_Open, Viewport_SidebarFlags);
        ImGui::Text("Left Sidebar");

        ImGui::BeginGroup();
        {
            if (ImGui::Button("Button 1")) {}
            if (ImGui::Button("Button 2")) {}
            if (ImGui::Button("Button 3")) {}
        }
        ImGui::EndGroup();

        ImGui::BeginGroup();
        {
            ImGui::Text("Width");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::SliderFloat("##left-sidebar-width", &Sidebar_Left_Width, Sidebar_Left_MinWidth, 380.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndGroup();
        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void Viewport::DrawRightSidebar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        static float Sidebar_Right_MinWidth = 160.0f;
        static float Sidebar_Right_Width = 180.0f;
        float Sidebar_Right_Height = viewport->WorkSize.y;
        ImVec2 LeftSidebar_Size = ImVec2(Sidebar_Right_Width, Sidebar_Right_Height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::SetNextWindowClass(DockWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin(Sidebar_Right_Label, &Sidebar_Right_Open, Viewport_SidebarFlags);
        ImGui::Text("Right Sidebar");

        ImGui::BeginGroup();
        {
            if (ImGui::Button("Button 1")) {}
            if (ImGui::Button("Button 2")) {}
            if (ImGui::Button("Button 3")) {}
        }
        ImGui::EndGroup();

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void Viewport::DrawBottomBar()
    {
        ImGui::SetNextWindowClass(DockWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGuiWindowFlags Bottom_Bar_Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;// | ImGuiDockNodeFlags_NoTabBar;
        ImGui::Begin(Bottom_Bar_Label, &Bottom_Bar_Open, Bottom_Bar_Flags);
        ImGui::Text("Bottom Bar");

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void Viewport::DrawTopBar()
    {
        ImGui::SetNextWindowClass(DockWindowClass);
        ImGuiWindowFlags TopBar_Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
        static int val = 0;
        ImGui::Begin(TopBar_Label, &Top_Bar_Open, TopBar_Flags);

        ImGui::Text("Top Bar Content");
        //ImGui::SliderInt("Slider Int", &val, 0, 10, "%.0d");
        ImGui::End();
    }

    void Viewport::ApplyDockspaceLayout()
    {
        LK_ASSERT(m_MainDockspaceID != NULL);
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ResetDockspaceLayout(m_MainDockspaceID, m_MainDockspaceFlags, viewport->Size);

        // Create layout
        // auto dock_id_top = ImGui::DockBuilderSplitNode(m_MainDockspaceID, ImGuiDir_Up, 0.5, nullptr, &m_MainDockspaceID);
        auto dock_id_top = ImGui::DockBuilderSplitNode(m_MainDockspaceID, ImGuiDir_Up, 0.2, nullptr, &m_MainDockspaceID);
        auto dock_id_bottom = m_MainDockspaceID; // Bottom part is the remaining space in dockspace_ID

        // Split top node horizontally
        auto dock_id_new_top = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Up, 0.20f, nullptr, &dock_id_top);
        auto dock_id_middle = dock_id_bottom;

        auto dock_id_left = ImGui::DockBuilderSplitNode(dock_id_middle, ImGuiDir_Left, Sidebar_Left_Ratio, nullptr, &dock_id_middle);
        auto dock_id_right = ImGui::DockBuilderSplitNode(dock_id_middle, ImGuiDir_Right, Sidebar_Right_Ratio, nullptr, &dock_id_middle);
        auto dock_id_center = dock_id_middle;  // Center part is the remaining space in dock_id_top

        // Build it
        ImGui::DockBuilderDockWindow(TopBar_Label, dock_id_new_top);
        ImGui::DockBuilderDockWindow(Sidebar_Left_Label, dock_id_left);
        ImGui::DockBuilderDockWindow(Sidebar_Right_Label, dock_id_right);
        ImGui::DockBuilderDockWindow(MainWindow_Label, dock_id_center);
        ImGui::DockBuilderDockWindow(Bottom_Bar_Label, dock_id_bottom);

        ImGui::DockBuilderFinish(m_MainDockspaceID);
    }

    void Viewport::ResetDockspaceLayout(ImGuiID& dockspace_id, ImGuiDockNodeFlags flags, const ImVec2& size)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, flags);
        ImGui::DockBuilderSetNodeSize(dockspace_id, size);
    }

}