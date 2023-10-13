#include "LKpch.h"
#include "LkEngine/UI/Viewport.h"


namespace LkEngine::UI {

    // TODO: CLEAN THIS UP
    unsigned int Viewport::Viewport_StyleVarCount = 0;
    bool Viewport::Top_Bar_Open= true;
    bool Viewport::Bottom_Bar_Open = true;
    bool Viewport::Sidebar_Left_Open = true;
    bool Viewport::Sidebar_Right_Open = true;
    bool Viewport::m_Initialized = false;
    float Viewport::Sidebar_Left_Ratio = 0.20f;
    float Viewport::Sidebar_Right_Ratio = 0.20f;
    float Viewport::TopBottom_Ratio = 0.80f;
    ImGuiID Viewport::m_MainDockSpaceID = NULL;
    ImGuiDockNodeFlags Viewport::m_MainDockSpaceFlags = NULL;
    ImGuiWindowClass* Viewport::UIWindowClass = nullptr;
    ImGuiWindowClass* Viewport::RendererWindowClass = nullptr;


    void Viewport::Init()
    {
        UIWindowClass = new ImGuiWindowClass();
        UIWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;
        RendererWindowClass = new ImGuiWindowClass();
        RendererWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_PassthruCentralNode;
        m_MainDockSpaceFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;// | ImGuiDockNodeFlags_PassthruCentralNode;
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

        DrawTopBar();
        DrawBottomBar();
        DrawLeftSidebar();
        DrawRightSidebar();

        //ImGui::PopStyleVar(Viewport_StyleVarCount);

        BeginMainWindow();
    }

    void Viewport::EndViewport()
    {
        EndMainWindow();
        EndViewportDockSpace();
    }

    void Viewport::BeginMainWindow()
    {
        //ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        ImGui::SetNextWindowClass(RendererWindowClass);
        ImGui::SetNextWindowBgAlpha(0);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::Begin(MainWindow_Label, NULL, flags);
        ImGui::PopStyleColor();
    }

    void Viewport::EndMainWindow()
    {
        ImGui::End();
    }

    void Viewport::DrawLeftSidebar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        float Sidebar_Left_Height = viewport->WorkSize.y;
        float Sidebar_Left_Width = viewport->WorkSize.x;
        static float Sidebar_Left_MinWidth = 140.0f;
        ImVec2 Sidebar_Left_Size = ImVec2(Sidebar_Left_Width, Sidebar_Left_Height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::SetNextWindowClass(UIWindowClass);
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoNav;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowSize(ImVec2(Sidebar_Left_Width, Sidebar_Left_Height));
        ImGui::Begin(Sidebar_Left_Label, &Sidebar_Left_Open, flags);
        ImGui::Text("Left Sidebar");

        ImGui::BeginGroup();
        {
            if (ImGui::Button("Button 1")) {}
            if (ImGui::Button("Button 2")) {}
            if (ImGui::Button("Button 3")) {}
        }
        ImGui::EndGroup();

        ImGui::BeginGroup();
        ImGui::Text("Width");
        ImGui::SameLine();
        ImGui::SliderFloat("##left-sidebar-width", &Sidebar_Left_Width, Sidebar_Left_MinWidth, 380.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
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

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin(Sidebar_Right_Label, &Sidebar_Right_Open, flags);
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
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGui::Begin(Bottom_Bar_Label, &Bottom_Bar_Open, flags);
        ImGui::Text("Bottom Bar");

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void Viewport::DrawTopBar()
    {
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        static int val = 0;
        ImGui::Begin(TopBar_Label, &Top_Bar_Open, flags);

        ImGui::Text("Top Bar Content");
        ImGui::End();
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
        ImGui::End();
        Viewport_StyleVarCount = 0;
    }

    void Viewport::ApplyDockSpaceLayout()
    {
        LK_ASSERT(m_MainDockSpaceID != NULL);
        m_MainDockSpaceID = ImGui::GetID(Main_DockSpace);
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::DockBuilderRemoveNode(m_MainDockSpaceID); // clear any previous layout
        ImGui::DockBuilderAddNode(m_MainDockSpaceID, m_MainDockSpaceFlags);
        ImGui::DockBuilderSetNodeSize(m_MainDockSpaceID, viewport->Size);

        // Create layout
        auto dock_id_top = ImGui::DockBuilderSplitNode(m_MainDockSpaceID, ImGuiDir_Up, TopBottom_Ratio, nullptr, &m_MainDockSpaceID);
        auto dock_id_bottom = m_MainDockSpaceID; // Bottom part is the remaining space in dockspace_ID

        auto dock_id_middle = dock_id_top;
        auto dock_id_new_top = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Up, 0.10f, nullptr, &dock_id_top);

        auto dock_id_left = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Left, Sidebar_Left_Ratio, nullptr, &dock_id_top);
        auto dock_id_right = ImGui::DockBuilderSplitNode(dock_id_top, ImGuiDir_Right, Sidebar_Right_Ratio, nullptr, &dock_id_top);
        auto dock_id_center = dock_id_top;  // Center part is the remaining space in dock_id_top

        // Build it
        ImGui::DockBuilderDockWindow(TopBar_Label, dock_id_new_top);
        ImGui::DockBuilderDockWindow(Sidebar_Left_Label, dock_id_left);
        ImGui::DockBuilderDockWindow(Sidebar_Right_Label, dock_id_right);
        ImGui::DockBuilderDockWindow(Bottom_Bar_Label, dock_id_bottom);
        ImGui::DockBuilderDockWindow(MainWindow_Label, dock_id_center);

        ImGui::DockBuilderFinish(m_MainDockSpaceID);
    }

    void Viewport::ResetDockSpaceLayout(ImGuiID& dockspace_id, ImGuiDockNodeFlags flags, const ImVec2& size)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, flags);
        ImGui::DockBuilderSetNodeSize(dockspace_id, size);
    }

}