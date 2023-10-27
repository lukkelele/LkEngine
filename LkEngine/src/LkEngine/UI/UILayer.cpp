#include "LKpch.h"
#include "LkEngine/UI/UILayer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    float UILayer::PositionStepSize = 5;
    ImGuiWindowClass* UILayer::UILayerWindowClass = nullptr;
    ImGuiWindowClass* UILayer::RendererWindowClass = nullptr;
    ImGuiWindowClass* UILayer::ExternalWindowClass = nullptr;
    //uint32_t UILayer::SelectedEntityID = 0;
    std::string UILayer::SelectedEntityLabel = "";
    ImVec2 UILayer::SceneContentMenuSize = ImVec2(0, 500);
    ImVec2 UILayer::SelectedEntityMenuSize = ImVec2(0, 440);
    bool UILayer::ShowImGuiDemoWindow = false;
    bool UILayer::Initialized = false;

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
        RendererWindowClass->ViewportFlagsOverrideSet |= ImGuiViewportFlags_NoInputs;

        ExternalWindowClass = new ImGuiWindowClass();
        ExternalWindowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;
        ExternalWindowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
    }
    
    void UILayer::BeginMainRenderWindow()
    {
#if 0
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        auto& io = ImGui::GetIO();
        //ImGui::SetNextWindowClass(RendererWindowClass);
        //ImGui::Begin(RENDER_WINDOW, NULL, flags);
#endif
    }

    void UILayer::EndMainRenderWindow()
    {
        //ImGui::End();
    }

    void UILayer::Begin()
    {
        //LkEngine::DockSpace::Begin();
    	//TopBar();
		//BottomBar();
        //LeftSidebar();
        //RightSidebar();
    }

    void UILayer::End()
    {
        //EndMainRenderWindow();
        //LkEngine::DockSpace::End();
    }

    void UILayer::OnAttach()
    {
#if 0
        if (!Initialized)
        {
            Init();
            Initialized = true;
        }
#endif
    }

    void UILayer::OnDetach()
    {
    }

    void UILayer::OnImGuiRender()
    {
#if 0
    	TopBar();
		BottomBar();
        LeftSidebar();
        RightSidebar();
#endif
    }

    void UILayer::TopBar()
    {
#if 0
        ImGui::SetNextWindowClass(UILayerWindowClass);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        //ImGui::Begin(TOP_BAR, NULL, flags);
        ImGui::BeginChild("##top-bar", ImVec2(0, 100), flags);

        ImGui::SeparatorText("LkEngine Configuration");
        ImGui::Checkbox("Demo Window", &ShowImGuiDemoWindow);
        ImGui::SameLine(0, 10);
        ImGui::Checkbox("Docking Enabled", &DockSpace::DockingEnabled);
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

        ImGui::SameLine();
        auto mouse_pos = Mouse::GetMousePosDockWindow();
        //ImGui::Text("Mouse (%1.f, %1.f)", Mouse::GetMouseX(), Mouse::GetMouseY());
        ImGui::Text("Mouse (%1.f, %1.f)", mouse_pos.x, mouse_pos.y);
        ImGui::SameLine(0, 10);
        ImGui::Text("Mouse normalized (%.4f, %.4f)", mouse_pos.x / DockSpace::CenterWindowSize.x, mouse_pos.y / DockSpace::CenterWindowSize.y);
        ImGui::SameLine(0, 10);
        float scale_x = DockSpace::CenterWindowSize.x / Window::Get()->GetViewportWidth();
        ImGui::Text("Mouse scaled (%.4f, %.4f  x_scale: %.2f)", 
            (mouse_pos.x / DockSpace::CenterWindowSize.x) / scale_x, 
            (mouse_pos.y / DockSpace::CenterWindowSize.y) / scale_x, scale_x);

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_DockHierarchy))
        {
            if (ShowImGuiDemoWindow)
                ImGui::SetNextWindowFocus();
        }
        if (ShowImGuiDemoWindow)
            ImGui::ShowDemoWindow();

        ImGui::EndChild();
#endif
    }

    void UILayer::BottomBar()
    {
#if 0
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;
        //ImGui::SetNextWindowClass(UILayerWindowClass);
        ImGui::BeginChild("##lkengine-bottom-bar", ImVec2(0, 400), flags);

        CameraControls();
        ImGui::SameLine(0, 20);
        DockWindowsMenu();

        ImGui::SameLine(0, 50);
        ImGui::ShowStackToolWindow();

        ImGui::EndChild();
#endif
    }

    void UILayer::LeftSidebar()
    {
#if 0
        if (!DockSpace::Sidebar_Left_Enabled)
            return;

        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        //flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        //flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar;
        ////flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;
        //flags |= ImGuiWindowFlags_NoScrollWithMouse;
        //ImGui::SetNextWindowClass(UILayerWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        ImGui::BeginChild("##lkengine-sidebar-left", ImVec2(340, 0), flags);
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Colors"))
        {
            ImGui::PushID("##renderer-background-color");
            static ImGuiSliderFlags bg_slider_flags = ImGuiSliderFlags_None;
            ImGui::Text("Background"); 
            ImGui::SliderFloat("##color_slider-x", &Renderer::BackgroundColor.x, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##color_slider-y", &Renderer::BackgroundColor.y, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##color_slider-z", &Renderer::BackgroundColor.z, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::SliderFloat("##color_slider-w", &Renderer::BackgroundColor.w, 0.0f, 1.0f, " %.3f", bg_slider_flags);
            ImGui::PopID();
            ImGui::TreePop();
        }
        SceneMenu();

        ImGui::PopStyleVar(1);
        ImGui::EndChild();
#endif
    }

    void UILayer::RightSidebar()
    {
#if 0
        static ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;

        //ImGui::SetNextWindowClass(UILayerWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
        //ImGui::Begin(SIDEBAR_RIGHT, NULL, flags);
        ImGui::BeginChild("##lkengine-sidebar-right", ImVec2(340, 0), flags);

        SceneContentMenu();
        //SelectedEntityMenu();

        ImGui::EndChild();
        ImGui::PopStyleVar(2);
#endif
    }

    void UILayer::SceneMenu()
    {
#if 0
        ImGui::PushID(ImGui::GetID("_scene-menu"));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("##scene-menu");

        static const char* type_geometry = "Geometry";
        static const char* type_rigidbody = "Rigidbody";
        const char* entity_types[] = { type_geometry, type_rigidbody };
        static int current_type_idx = 0;
        const char* entity_type_preview = entity_types[current_type_idx];
        static ImGuiComboFlags combo_flags = ImGuiComboFlags_None;
        combo_flags |= ImGuiComboFlags_PopupAlignLeft;

        float center_window_width = Window::Get()->GetWidth();
        float center_window_height = Window::Get()->GetHeight();
        ImVec2 window_size = { center_window_width, center_window_height };

        static float rect_width = 100.0f;
        static float rect_height = 150.0f;
        static ImVec2 start_placing_point = ImVec2(0, 0);

        ImGui::SeparatorText("Scene Menu");
        //ImGui::BeginChild("##scene-menu");

        ImGui::BeginGroup();
        ImGui::SeparatorText("Create-Entity-Menu");
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
            ImGui::SliderFloat("Width", &rect_width, 50.0f, 500.0f, "%.1f");
            ImGui::SliderFloat("Height", &rect_height, 50.0f, 500.0f, "%.1f");
            ImGui::SliderFloat2("Placing Point", &start_placing_point.x, 0.0f, 1000.0f, "%1.f");
        }
        if (current_type == type_rigidbody)
        {
        }

        if (ImGui::Button("Add")) 
        {
            if (current_type == type_geometry)
            {
                auto window = Window::Get();
                ImVec2 window_size = ImVec2(window->GetWidth(), window->GetHeight());
                glm::vec2 p1 = { start_placing_point.x, start_placing_point.y };
                glm::vec2 p2 = { start_placing_point.x + rect_width, start_placing_point.y + rect_height };
                EntityFactory::CreateRectangle(*Scene::ActiveScene, p1, p2);
            }
        }
        ImGui::EndGroup();

        ImGui::EndChild();
        ImGui::PopStyleVar(1);
        ImGui::PopID();
#endif
    }

    // TODO: Update
    void UILayer::AppInfo()
    {
#if 0
        auto app = Application::Get();
        bool keyboard_enabled = app->IsKeyboardEnabled();
        bool mouse_enabled = app->IsMouseEnabled();
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;
        static ImGuiWindowFlags app_stats_flags = ImGuiWindowFlags_NoDocking;

        //ImGui::Begin(RENDER_WINDOW);
        ImGui::BeginChild("##lkengine-center-window", ImVec2(0, 0), flags);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0);
        ImGui::BeginChild("##lkengine-main-statistics", ImVec2(100, 80), true, app_stats_flags);

        ImGui::Text("Keyboard: %s", keyboard_enabled ? "ON" : "OFF");
        ImGui::Text("Mouse: %s", mouse_enabled ? "ON" : "OFF");

        ImGui::EndChild();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
        ImGui::EndChild();
#endif
    }

    void UILayer::CreateEntityMenu()
    {
    }

    void UILayer::DockWindowsMenu()
    {
#if 0
        ImGui::BeginGroup();
        ImGui::PushID(ImGui::GetID("dockwindows-menu"));
        ImGui::Text("Dock Windows");

        if (ImGui::Checkbox("Right Sidebar", &DockSpace::Sidebar_Right_Enabled))
            DockSpace::ApplyDockSpaceLayout();
        if (ImGui::Checkbox("Left Sidebar", &DockSpace::Sidebar_Left_Enabled))
            DockSpace::ApplyDockSpaceLayout();

        ImGui::PopID();
        ImGui::EndGroup();
#endif
    }

    void UILayer::SceneContentMenu()
    {
#if 0
        //ImGuiDockNode* window = ImGui::GetWindowDockNode();
        auto window = Window::Get();
        SceneContentMenuSize.y = window->GetHeight() - SelectedEntityMenuSize.y;
        ImGui::BeginChild("##scene-content-menu", SceneContentMenuSize, true);
		ImGui::SeparatorText("Scene");

        auto& scene = *Scene::ActiveScene;
        auto& registry = scene.GetRegistry();
        static ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;

		auto entities = registry.view<TransformComponent>();
        for (auto& ent : entities)
        {
            Entity entity = { ent, &scene };
            bool is_selected = SelectedEntityLabel == entity.GetName();
            std::string label = fmt::format("{}", entity.GetName());
            if (ImGui::Selectable(label.c_str(), &is_selected, selectable_flags))
            {
                LOG_TRACE("Selecting {}", label);
                SelectedEntityLabel = label;
            }
        }
        ImGui::EndChild();
#endif
    }


    void UILayer::CameraControls()
    {
#if 0
        auto& scene = *Scene::ActiveScene;
        auto& camera = *scene.GetActiveCamera();
        auto& pos = camera.GetPos();
        //ImGui::BeginChild("##camera-controls");
        ImGui::BeginGroup();
        ImGui::Text("Camera");
        UI::Property::PositionXYZ(camera.ID, pos);
        ImGui::EndGroup();
        //ImGui::EndChild();
#endif
    }

}