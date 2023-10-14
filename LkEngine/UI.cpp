#include "LKpch.h"
#include "LkEngine/UI/UI.h"
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine::UI {

    ImGuiWindowClass* UIWindowClass = nullptr;
    ImGuiWindowClass* RendererWindowClass = nullptr;
    ImGuiID MainDockSpaceID;
    bool Initialized;
    float Sidebar_Left_Ratio = 0.15f;
    float Sidebar_Right_Ratio = 0.15f;
    float TopBottom_Ratio = 0.82f;
    unsigned int Viewport_StyleVarCount = 0;

    void Init()
    {
        UIWindowClass = new ImGuiWindowClass();
        UIWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;
        RendererWindowClass = new ImGuiWindowClass();
        RendererWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
    }
    
    void BeginMainRenderWindow()
    {
        //ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        //ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground;
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration;

        ImGui::SetNextWindowClass(RendererWindowClass);
        //ImGui::SetNextWindowBgAlpha(0);
        //ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255.0f, 255.0f, 255.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.5, 0.5, 0.5, 1.0));
        ImGui::Begin(MainRenderWindow_Label, NULL, flags);
        ImGui::Text("MAIN WINDOW CONTENT");
        ImGui::PopStyleColor(1);
    }

    void EndMainRenderWindow()
    {
        // ImGui::PopStyleColor(1);
        ImGui::End();
    }

    void BeginViewportDockSpace()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        // Setup dockspace over viewport
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

       if (!Initialized)
        {
            ApplyDockSpaceLayout();
            Initialized = true;
        }
        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::SetNextWindowPos(main_viewport->WorkPos);
        ImGui::SetNextWindowSize(main_viewport->WorkSize);
        ImGui::SetNextWindowViewport(main_viewport->ID);
        ImGui::Begin(Main_DockSpace, NULL, window_flags);
        ImGuiID dockspace_id = ImGui::GetID(Main_DockSpace);
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
        ImGui::PopStyleVar(3);
    }

    void EndViewportDockSpace()
    {
        ImGui::End(); // Main_DockSpace
    }

    void TopBar()
    {
        static bool top_bar_open = true;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        static int val = 0;
        ImGui::Begin(TopBar_Label, &top_bar_open, flags);
        ImGui::Text("Top Bar Content");
        ImGui::End();
    }

    void BottomBar()
    {
        static bool bottom_bar_open = true;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGui::Begin(Bottom_Bar_Label, &bottom_bar_open, flags);
        ImGui::Text("Bottom Bar");
        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void LeftSidebar()
    {
        static bool sidebar_open = true;
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        float sidebar_left_height = viewport->WorkSize.y;
        float sidebar_left_width = viewport->WorkSize.x;
        static float sidebar_left_minwidth = 140.0f;
        ImVec2 Sidebar_Left_Size = ImVec2(sidebar_left_width, sidebar_left_height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::SetNextWindowClass(UIWindowClass);
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoNav;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowSize(ImVec2(sidebar_left_width, sidebar_left_height));
        ImGui::Begin(Sidebar_Left_Label, &sidebar_open, flags);
        ImGui::PopStyleVar(1);
        ImGui::Text("Left Sidebar");

        ImGui::BeginGroup();
        if (ImGui::Button("Button 1")) {}
        if (ImGui::Button("Button 2")) {}
        if (ImGui::Button("Button 3")) {}
        ImGui::EndGroup();

        ImGui::End();
    }

    void RightSidebar()
    {
        static bool sidebar_open = true;
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        static float min_width = 160.0f;
        static float width = 180.0f;
        static float height = viewport->WorkSize.y;
        ImVec2 LeftSidebar_Size = ImVec2(width, height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin(Sidebar_Right_Label, &sidebar_open, flags);
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

    void ApplyDockSpaceLayout()
    {
        //LK_ASSERT(MainDockSpaceID != NULL);
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDocking;
        MainDockSpaceID = ImGui::GetID(Main_DockSpace);
        ImGuiIO& io = ImGui::GetIO();
        ImGuiContext& g = *GImGui;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        // Reset layout
        ImGui::DockBuilderRemoveNode(MainDockSpaceID); 
        ImGui::DockBuilderAddNode(MainDockSpaceID, dockspace_flags);
        ImGui::DockBuilderSetNodeSize(MainDockSpaceID, viewport->Size);

        // Create layout
        // Top / Bottom
        auto dock_id_top = ImGui::DockBuilderSplitNode(MainDockSpaceID, ImGuiDir_Up, TopBottom_Ratio, nullptr, &MainDockSpaceID);
        auto dock_id_bottom = MainDockSpaceID; // Bottom part is the remaining space in dockspace_ID
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
        ImGui::DockBuilderFinish(MainDockSpaceID);
    }


}