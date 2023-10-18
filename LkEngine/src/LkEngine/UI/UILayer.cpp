#include "LKpch.h"
#include "LkEngine/UI/UILayer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Application.h"


namespace LkEngine {

    ImGuiWindowClass* UILayer::UILayerWindowClass = nullptr;
    ImGuiWindowClass* UILayer::RendererWindowClass = nullptr;
    ImGuiWindowClass* UILayer::ExternalWindowClass = nullptr;
    bool UILayer::ShowImGuiDemoWindow = false;

    void UILayer::Init()
    {
        UILayerWindowClass = new ImGuiWindowClass();
        UILayerWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton; // | ImGuiDockNodeFlags_NoResize;
        UILayerWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        UILayerWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;

        RendererWindowClass = new ImGuiWindowClass();
        RendererWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingInCentralNode; // | ImGuiDockNodeFlags_NoResize;
        RendererWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        RendererWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoSplit;

        ExternalWindowClass = new ImGuiWindowClass();
        ExternalWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;
        ExternalWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
    }
    
    void UILayer::BeginMainRenderWindow()
    {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        auto& io = ImGui::GetIO();
        ImGui::SetNextWindowClass(RendererWindowClass);
        ImGui::Begin(RENDER_WINDOW, NULL, flags);
    }

    void UILayer::EndMainRenderWindow()
    {
        ImGui::End();
    }

    void UILayer::Begin()
    {
        LkEngine::DockSpace::Begin();
    	TopBar();
		BottomBar();
        LeftSidebar();
        RightSidebar();

        SceneMenu();
		AppInfo();

		BeginMainRenderWindow(); 
    }

    void UILayer::End()
    {
        EndMainRenderWindow();
        LkEngine::DockSpace::End();
    }

    void UILayer::OnAttach()
    {
    }

    void UILayer::OnDetach()
    {
    }

    void UILayer::OnImGuiRender()
    {
    }

    void UILayer::TopBar()
    {
        static bool top_bar_open = true;
        ImGui::SetNextWindowClass(UILayerWindowClass);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGui::Begin(TOP_BAR, &top_bar_open, flags);

        ImGui::SeparatorText("LkEngine Configuration");
        ImGui::Checkbox("Demo Window", &ShowImGuiDemoWindow);

        ImGui::SameLine();

        int draw_mode = Renderer::DrawMode;
        static int selected_draw_mode = -1;
        std::string draw_label = fmt::format("{}", Renderer::DrawMode == LK_DRAW_TRIANGLES ? "Triangles" : "Lines");
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo("Draw Mode", draw_label.c_str()))
        {
            if (ImGui::Selectable("Triangles", selected_draw_mode == 0))
            {
                if (Renderer::DrawMode != LK_DRAW_TRIANGLES) 
                    Renderer::SetDrawMode(LK_DRAW_TRIANGLES);
            }
            if (ImGui::Selectable("Lines", selected_draw_mode == 1))
            {
                if (Renderer::DrawMode != LK_DRAW_LINES)
                    Renderer::SetDrawMode(LK_DRAW_LINES);
            }
            ImGui::EndCombo();
        }

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_DockHierarchy))
        {
            if (ShowImGuiDemoWindow)
                ImGui::SetNextWindowFocus();
        }
        if (ShowImGuiDemoWindow)
            ImGui::ShowDemoWindow();

        ImGui::End();
    }

    void UILayer::BottomBar()
    {
        static bool bottom_bar_open = true;
        ImGui::SetNextWindowClass(UILayerWindowClass);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGui::Begin(BOTTOM_BAR, &bottom_bar_open, flags);

        ImGui::End();
    }

    void UILayer::LeftSidebar()
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

        ImGui::SetNextWindowClass(UILayerWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowSize(ImVec2(sidebar_left_width, sidebar_left_height));
        ImGui::Begin(SIDEBAR_LEFT, &sidebar_open, flags);
        ImGui::PopStyleVar(1);

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Colors"))
        {
            static ImGuiSliderFlags bg_slider_flags = ImGuiSliderFlags_None;
            ImGui::Text("Background Color"); 
            ImGui::SliderFloat("##member-color_slider-x", &Renderer::BackgroundColor.x, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##member-color_slider-y", &Renderer::BackgroundColor.y, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##member-color_slider-z", &Renderer::BackgroundColor.z, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##member-color_slider-w", &Renderer::BackgroundColor.w, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::TreePop();
        }

        ImGui::End();
    }

    void UILayer::RightSidebar()
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
        ImGui::SetNextWindowClass(UILayerWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin(SIDEBAR_RIGHT, &sidebar_open, flags);

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void UILayer::AppInfo()
    {
        auto app = Application::Get();
        bool keyboard_enabled = app->IsKeyboardEnabled();
        bool mouse_enabled = app->IsMouseEnabled();

        ImGui::Begin(BOTTOM_BAR);

        ImGui::BeginChild("##appinfo-child");
        ImGui::Text("Keyboard: %s", keyboard_enabled ? "ON" : "OFF");
        ImGui::Text("Mouse: %s", mouse_enabled ? "ON" : "OFF");
        ImGui::EndChild();

        ImGui::End();
    }

    void UILayer::SceneMenu()
    {
        ImGui::Begin(SIDEBAR_RIGHT);
        ImGui::PushID(ImGui::GetID("_scene-menu"));

        static const char* type_geometry = "Geometry";
        static const char* type_rigidbody = "Rigidbody";
        const char* entity_types[] = { type_geometry, type_rigidbody };
        static int current_type_idx = 0;
        const char* entity_type_preview = entity_types[current_type_idx];
        static ImGuiComboFlags combo_flags = ImGuiComboFlags_None;
        combo_flags |= ImGuiComboFlags_PopupAlignLeft;

        static float rect_width = 0.20f;
        static float rect_height = 0.40f;

        ImGui::SeparatorText("Scene Menu");
        //ImGui::BeginChild("##scene-menu");
        ImGui::BeginGroup();
        if (ImGui::BeginCombo("Type", entity_type_preview, combo_flags))
        {
            for (int n = 0; n < LK_ARRAYSIZE(entity_types); n++)
            {
                const bool is_selected = (current_type_idx == n);
                if (ImGui::Selectable(entity_types[n], is_selected))
                    current_type_idx = n;
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        const char* current_type = entity_types[current_type_idx];

        ImGui::Text("Item Properties");
        // Show different menues depending on what type is to be created and added to the scene
        if (current_type == type_geometry)
        {
            ImGui::Text("Rectangle");
            ImGui::SliderFloat("Width", &rect_width, 0.01f, 1.50f, "%.2f");
            ImGui::SliderFloat("Height", &rect_height, 0.01f, 1.50f, "%.2f");
        }
        if (current_type == type_rigidbody)
        {
        }

        if (ImGui::Button("Add")) 
        {
            if (current_type == type_geometry)
            {
                auto main_node = ImGui::DockBuilderGetNode(DockSpace::RenderWindowDockID);
                ImVec2 window_size = main_node->Size;
                //glm::vec2 center_pos = { (window_size.x * 0.50f) - rect_width * 0.50f, (window_size.y * 0.50f) - rect_height * 0.50f };
                //glm::vec2 p1 = { center_pos.x - rect_width, center_pos.y - rect_height };
                //glm::vec2 p2 = { center_pos.x + rect_width, center_pos.y + rect_width };
                glm::vec2 p1 = { -rect_width * 0.50f, -rect_width * 0.50f };
                glm::vec2 p2 = { rect_width * 0.50f, rect_height * 0.50f };
                EntityFactory::CreateRectangle(*Scene::ActiveScene, p1, p2);
            }
        }
        ImGui::EndGroup();
        //ImGui::EndChild();

        ImGui::PopID();
        ImGui::End();
    }

}