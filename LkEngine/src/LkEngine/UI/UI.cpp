#include "LKpch.h"
#include "LkEngine/UI/UI.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Application.h"


namespace LkEngine {

    ImGuiWindowClass* UI::UIWindowClass = nullptr;
    ImGuiWindowClass* UI::RendererWindowClass = nullptr;
    ImGuiWindowClass* UI::ExternalWindowClass = nullptr;
    ImGuiID UI::MainDockSpaceID, UI::RenderWindowDockID, UI::BottomBarDockID;
    ImVec2 UI::LastViewportSize = ImVec2(0, 0);
    bool UI::Initialized = false, UI::ShowImGuiDemoWindow = false;
    float UI::Sidebar_Left_Ratio = 0.20f;
    float UI::Sidebar_Right_Ratio = 0.20f;
    float UI::TopBottom_Ratio = 0.86f;


    void UI::Init()
    {
        UIWindowClass = new ImGuiWindowClass();
        UIWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton; // | ImGuiDockNodeFlags_NoResize;
        UIWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        UIWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;

        RendererWindowClass = new ImGuiWindowClass();
        RendererWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingInCentralNode; // | ImGuiDockNodeFlags_NoResize;
        RendererWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        RendererWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;

        ExternalWindowClass = new ImGuiWindowClass();
        ExternalWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;
        ExternalWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
    }
    
    void UI::BeginMainRenderWindow()
    {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        auto& io = ImGui::GetIO();
        ImGui::SetNextWindowClass(RendererWindowClass);
        ImGui::Begin(RENDER_WINDOW, NULL, flags);

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_DockHierarchy))
        {
            //LOG_INFO("Mouse is inside Window");
            if (ShowImGuiDemoWindow)
                ImGui::SetNextWindowFocus();
        }
        if (ShowImGuiDemoWindow)
            ImGui::ShowDemoWindow();
    }

    void UI::EndMainRenderWindow()
    {

        ImGui::End();
    }

    void UI::BeginViewportDockSpace()
    {
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

        if (!Initialized)
        {
            ApplyDockSpaceLayout();
            Initialized = true;
        }
        // Adjust viewport if window has been resized
        ImVec2 viewport_size = main_viewport->WorkSize;
        if (LastViewportSize.x != viewport_size.x || LastViewportSize.y != viewport_size.y)
        {
            LastViewportSize = viewport_size;
            ApplyDockSpaceLayout();
        }

        ImGui::DockSpaceOverViewport(main_viewport, dockspace_flags, NULL);

        //ImGui::BeginMainMenuBar();
        //if (ImGui::BeginMenu("File"))
        //{
        //}
        //if (ImGui::BeginMenu("Edit"))
        //{
        //}
        //ImGui::EndMainMenuBar();
    }

    void UI::EndViewportDockSpace()
    {
    }

    void UI::TopBar()
    {
        static bool top_bar_open = true;
        ImGui::SetNextWindowClass(UIWindowClass);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGui::Begin(TOP_BAR, &top_bar_open, flags);

        ImGui::End();
    }

    void UI::BottomBar()
    {
        static bool bottom_bar_open = true;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGui::Begin(BOTTOM_BAR, &bottom_bar_open, flags);

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void UI::LeftSidebar()
    {
        static bool sidebar_open = true;
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        float sidebar_left_height = viewport->WorkSize.y;
        float sidebar_left_width = viewport->WorkSize.x;
        static float sidebar_left_minwidth = 140.0f;
        ImVec2 Sidebar_Left_Size = ImVec2(sidebar_left_width, sidebar_left_height);

        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowSize(ImVec2(sidebar_left_width, sidebar_left_height));
        ImGui::Begin(SIDEBAR_LEFT, &sidebar_open, flags);
        ImGui::PopStyleVar(1);


        ImGui::SeparatorText("LkEngine Configuration");

        ImGui::BeginGroup();
        if (ImGui::Checkbox("Demo Window", &ShowImGuiDemoWindow))
        {
        }
        ImGui::EndGroup();


        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Colors"))
        {
            static ImGuiSliderFlags bg_slider_flags = ImGuiSliderFlags_None;
            ImGui::Text("Background Color"); 
            ImGui::SliderFloat("##member-ColorSlider-x", &Renderer::BackgroundColor.x, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##member-ColorSlider-y", &Renderer::BackgroundColor.y, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##member-ColorSlider-z", &Renderer::BackgroundColor.z, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##member-ColorSlider-w", &Renderer::BackgroundColor.w, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::TreePop();
        }


        ImGui::End();
    }

    void UI::RightSidebar()
    {
        static bool sidebar_open = true;
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        static float min_width = 160.0f;
        static float width = 180.0f;
        static float height = viewport->WorkSize.y;
        ImVec2 LeftSidebar_Size = ImVec2(width, height);
		auto& colors = ImGui::GetStyle().Colors;

        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin(SIDEBAR_RIGHT, &sidebar_open, flags);

        //ImGui::ShowStyleEditor();

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void UI::ApplyDockSpaceLayout()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoResize; 
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
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
        ImGui::DockBuilderFinish(MainDockSpaceID);
    }

    void UI::AppInfo()
    {
        auto app = Application::Get();
        bool keyboard_enabled = app->IsKeyboardEnabled();

        // Place in right sidebar
        ImGui::Begin(SIDEBAR_RIGHT);

        ImGui::Text("Keyboard: %s", keyboard_enabled ? "ON" : "OFF");
        
        ImGui::Separator();
        ImGui::End();
    }

}