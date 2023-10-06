#include "LkEngine/UI/Viewport.h"


namespace LkEngine::UI {

    unsigned int Viewport::StyleStackCount = 0;
    unsigned int Viewport::MainWindow_StyleStackCount = 0;
    bool Viewport::Left_Sidebar_Open = true;
    bool Viewport::Right_Sidebar_Open = true;
    bool Viewport::Bottom_Bar_Open = true;
    float Viewport::Left_Sidebar_MinWidth = 160.0f;
    float Viewport::Left_Sidebar_Width = 180.0f;

    void Viewport::BeginViewportBase()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration 
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar;

        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        ImGuiID dockspace_ID = ImGui::GetID("ViewportDocking");
        ImGuiID lower_dockspace_ID = ImGui::GetID("lower-dockspace");

        static ImGuiDockNodeFlags dockspace_Flags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_Flags);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        StyleStackCount++;
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::Begin(Main_Dockspace, (bool*)true, flags);
        ImGui::DockSpace(dockspace_ID, ImVec2(0, 0), dockspace_Flags);
        // ImGui::DockSpace(lower_dockspace_ID);
        // static ImGuiDockNodeFlags dockspace_Flags = ImGuiDockNodeFlags_PassthruCentralNode;
        static bool first_time = true;
        if (first_time)
        {
	        ImGui::DockBuilderRemoveNode(dockspace_ID); // clear any previous layout
	        ImGui::DockBuilderAddNode(dockspace_ID, dockspace_Flags | ImGuiDockNodeFlags_DockSpace);
	        ImGui::DockBuilderSetNodeSize(dockspace_ID, viewport->Size);

	        auto dockID_top = ImGui::DockBuilderSplitNode(dockspace_ID, ImGuiDir_Up, 0.6f, nullptr, &dockspace_ID);
	        auto dockID_down = ImGui::DockBuilderSplitNode(dockspace_ID, ImGuiDir_Down, 0.2f, nullptr, &dockspace_ID);
	        auto dockID_left = ImGui::DockBuilderSplitNode(dockspace_ID, ImGuiDir_Left, 0.6f, nullptr, &dockspace_ID);
	        auto dockID_right = ImGui::DockBuilderSplitNode(dockspace_ID, ImGuiDir_Right, 0.15f, nullptr, &dockspace_ID);

	        // we now dock our windows into the docking node we made above
	        ImGui::DockBuilderDockWindow(Left_Sidebar_Label, dockID_left);
	        ImGui::DockBuilderDockWindow(Right_Sidebar_Label, dockID_right);
	        // ImGui::DockBuilderDockWindow(MainWindow_Label, dockID_top);
	        // ImGui::DockBuilderDockWindow(Bottom_Bar_Label, dockID_down);
	        ImGui::DockBuilderFinish(dockspace_ID);

            first_time = false;
        }
        // Main Menu
        // ImGui::BeginMenuBar();
        // if (ImGui::BeginMenu("File")) 
        // {
        //     ImGui::MenuItem("New");
        //     ImGui::EndMenu();
        // }
        // if (ImGui::MenuItem("Edit", NULL))
        // {
        // }
        // ImGui::EndMenuBar();
        //---------------------
        DrawLeftSidebar();
        DrawRightSidebar();
        DrawBottomBar();

        ImGui::PopStyleVar(StyleStackCount);
        ImGui::SameLine();

        BeginMainWindow();
    }

    void Viewport::BeginMainWindow()
    {
        static ImVec2 MainWindow_Size = ImVec2(1400, 900);
        ImGuiWindowFlags MainWindow_Flags = ImGuiWindowFlags_None;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        MainWindow_StyleStackCount++;
        ImGui::Begin(MainWindow_Label, WINDOW_ALWAYS_OPEN, MainWindow_Flags);
   
        ImGui::Text("Main Window");
        ImGui::SeparatorText("Hej Main Window");
    }

    void Viewport::EndMainWindow()
    {
        ImGui::End();
        ImGui::PopStyleVar(MainWindow_StyleStackCount);
        MainWindow_StyleStackCount = 0;
    }

    void Viewport::EndViewportBase()
    {
        EndMainWindow();

        ImGui::End();
        StyleStackCount = 0;
    }

    void Viewport::DrawLeftSidebar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        float Left_Sidebar_Height = viewport->WorkSize.y;
        ImVec2 Left_Sidebar_Size = ImVec2(Left_Sidebar_Width, Left_Sidebar_Height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowSize(ImVec2(Left_Sidebar_Width, Left_Sidebar_Height));
        
        ImGui::Begin("##LeftSideBar", &Left_Sidebar_Open, SidebarFlags);
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
            ImGui::SliderFloat("##left-sidebar-width", &Left_Sidebar_Width, Left_Sidebar_MinWidth, 380.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndGroup();

        ImGui::End();

        ImGui::PopStyleVar(1);
        // ImGui::PopStyleColor(2);
    }

    void Viewport::DrawRightSidebar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        static float Right_Sidebar_MinWidth = 160.0f;
        static float Right_Sidebar_Width = 180.0f;
        float Right_Sidebar_Height = viewport->WorkSize.y;
        ImVec2 LeftSidebar_Size = ImVec2(Right_Sidebar_Width, Right_Sidebar_Height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin("##RightSidebar", &Right_Sidebar_Open, SidebarFlags);
        ImGui::Text("Right Sidebar");

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
            ImGui::SliderFloat("##right-sidebar-width", &Right_Sidebar_Width, Right_Sidebar_MinWidth, 380.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndGroup();
        ImGui::End();

        ImGui::PopStyleVar(1);
        // ImGui::PopStyleColor(2);
    }

    void Viewport::DrawBottomBar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        // ImGuiWindowFlags Bottom_Bar_Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGuiWindowFlags Bottom_Bar_Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin(Bottom_Bar_Label, &Bottom_Bar_Open, Bottom_Bar_Flags);
        ImGui::Text("Bottom Bar");

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

}