#include "LKpch.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Debug/DebugLayer.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/UI/ImGuiUtilities.h"
#include "LkEngine/Editor/Editor.h"


namespace LkEngine {

	ImVec2 EditorLayer::SelectedEntityMenuSize = ImVec2(0, 440); // TODO: patch out

	EditorLayer::EditorLayer(Scene& scene)
		: Layer("EditorLayer")
		, m_Scene(&scene)
	{
		s_Instance = this;
		m_ViewportBounds[0] = { 0, 0 };
		m_ViewportBounds[1] = { Window::Get()->GetViewportWidth(), Window::Get()->GetViewportHeight() };
		m_SecondViewportBounds[0] = { 0, 0 };
		m_SecondViewportBounds[1] = { 0, 0 };
		
		m_ShowStackTool = true;
		Enabled = true;
	}

	void EditorLayer::OnImGuiRender()
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		auto* window = Application::Get()->GetWindow()->GetGlfwWindow();
		bool isMaximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		// Push 4 style var, 1 color
		ImGui::Begin(UI_CORE_VIEWPORT, NULL, UI::CoreViewportFlags);
		ImGui::PopStyleColor(); 
		ImGui::PopStyleVar(4);

		UI_HandleManualWindowResize();

		// Dockspace
		if (UI::DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
		    UI::CoreViewportFlags |= ImGuiWindowFlags_NoBackground;
			UI::HostWindowFlags |= ImGuiWindowFlags_NoBackground;
		}

		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID(LkEngine_DockSpace), ImVec2(0, 0), UI::DockspaceFlags);
		style.WindowMinSize.x = minWinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin(LkEngine_DockSpace, NULL, UI::HostWindowFlags);

		static float center_window_width = 0;         // viewport->Size.x - sidebar_left_width - sidebar_right_width;
		static float center_window_height = 0;        // viewport->WorkSize.y - bottombar_height;
		static float center_window_xpos = 0;          // sidebar_left_width;
		static float center_window_ypos = 0;          // bottombar_height;
		static ImVec2 center_window_pos = { 0, 0 };   // { center_window_xpos, center_window_ypos };
		static ImVec2 center_window_size = { 0, 0 };  // { center_window_width, center_window_height };

		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
		{
			//--------------------------------------------------
			// TOP MENUBAR
			//--------------------------------------------------
			static float topbar_height = 30.0f;

			ImGui::PushID(UI_TOP_BAR);
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, topbar_height), ImGuiCond_Always);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
			ImGui::Begin(UI_TOP_BAR, NULL, UI::TopbarFlags);
			ImGui::PopStyleVar(1);
			{
				ImGui::BeginMenuBar();
				topbar_height = ImGui::GetFrameHeight();
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New")) { }
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Render Information"))
				{
					ShowRenderInformationWindow = !ShowRenderInformationWindow;
				}
				ImGui::EndMenuBar();
			}
			ImGui::End();
			ImGui::PopID();
			//--------------------------------------------------

			SelectedEntity = m_Scene->GetEntityWithUUID(SelectedEntityID);

			//--------------------------------------------------
			// LEFT SIDEBAR
			//--------------------------------------------------
			static float sidebar_left_width = 340.0f;
			static float sidebar_left_height = m_ViewportBounds[1].y;
			ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetCursorPosY()));
			ImGui::SetNextWindowSize(ImVec2(sidebar_left_width, sidebar_left_height), ImGuiCond_Always);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 12));

			ImGui::Begin(UI_SIDEBAR_LEFT, nullptr, UI::SidebarFlags);
			{
				ImGui::PopStyleVar(1);
				{
					// TODO: initialize this bool by checking the depth in the graphics context
					static bool depth_test_checkbox = false;
					ImGui::BeginGroup();
					if (ImGui::Checkbox("Depth Testing", &depth_test_checkbox))
					{
						if (depth_test_checkbox == true)
						{
							Window::Get()->SetDepthEnabled(true);
						}
						else
						{
							Window::Get()->SetDepthEnabled(false);
						}
					}

					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNode("Colors"))
					{
					    static ImGuiSliderFlags bg_slider_flags = ImGuiSliderFlags_None;
					    ImGui::PushID("##lkengine-leftpanel-colors");
					    ImGui::Text("Background"); 
					    ImGui::SliderFloat("##x", &Renderer::s_BackgroundColor.x, 0.0f, 1.0f, " %.3f", bg_slider_flags);
					    ImGui::SliderFloat("##y", &Renderer::s_BackgroundColor.y, 0.0f, 1.0f, " %.3f", bg_slider_flags);
					    ImGui::SliderFloat("##z", &Renderer::s_BackgroundColor.z, 0.0f, 1.0f, " %.3f", bg_slider_flags);
					    ImGui::SliderFloat("##w", &Renderer::s_BackgroundColor.w, 0.0f, 1.0f, " %.3f", bg_slider_flags);
					    ImGui::SliderFloat("UI Alpha", &colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f, " %.2f", bg_slider_flags);
					    ImGui::PopID();

					    ImGui::TreePop();
					}
					//UI_SceneContent();

					if (SelectedEntity)
					{
						DrawComponents(SelectedEntity);
					}
					ImGui::EndGroup();
				}
				ImGui::Text("Selected ID: %llu", SelectedEntityID);
				ImGui::Text("Selected Entity Handle: %llu", SelectedEntity.IsValid() ? SelectedEntity.GetUUID() : 0);

				if (m_Scene)
				{
					auto& registry = m_Scene->GetRegistry();
					registry.each([&](auto entityID)
					{
							Entity entity{ entityID, m_Scene.get() };
							DrawEntityNode(entity);
					});

					if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					{
						SelectedEntityID = {};
					}

				}

			}
			ImGui::End();  
			//--------------------------------------------------

			//--------------------------------------------------
			// RIGHT SIDEBAR
			//--------------------------------------------------
			static float sidebar_right_width = 340.0f;
			static float sidebar_right_height = m_ViewportBounds[1].y;
			ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - sidebar_right_width, ImGui::GetCursorPosY()));
			ImGui::SetNextWindowSize(ImVec2(sidebar_right_width, sidebar_right_height), ImGuiCond_Always);
			ImGui::Begin(UI_SIDEBAR_RIGHT, nullptr, UI::SidebarFlags);
			{
				ImGui::BeginGroup();
				if (ImGui::Checkbox("Style Editor", &m_ShowStyleEditor)) { }
				//if (ImGui::Checkbox("Stack Debugger", &m_ShowStackTool)) { }

				if (SelectedEntity && m_GizmoType != -1)
				{
					DrawImGuizmo(SelectedEntity);
				}

				if (m_ShowStackTool) ImGui::ShowStackToolWindow();
				if (m_ShowStyleEditor)
				{
					ImGui::Begin("Style Editor");
					ImGui::ShowStyleEditor();
					ImGui::End();
				}
				if (ImGui::Checkbox("Left dock", &DockSpace::Sidebar_Left_Enabled))
				{
					DockSpace::ApplyDockSpaceLayout();
				}
				//UI_SelectedEntityProperties();

				ImGui::EndGroup();
			}
			ImGui::End();


			//--------------------------------------------------
			// BOTTOM BAR
			//--------------------------------------------------
			static float bottombar_height = 180.0f;
			ImGui::SetNextWindowSize(ImVec2(sidebar_right_width, sidebar_right_height), ImGuiCond_Once);
			float min_width = viewport->Size.x - (sidebar_right_width + sidebar_left_width);
			ImGui::SetNextWindowSizeConstraints(ImVec2(min_width, 180.0f), ImVec2(4000, 800));
			ImGui::SetNextWindowPos(ImVec2(sidebar_left_width, viewport->Size.y - bottombar_height));
			ImGui::Begin(UI_BOTTOM_BAR, nullptr, UI::SidebarFlags);
			{
				ImGui::BeginGroup();
				auto mouse_pos = Mouse::GetMousePos();
				Mouse::Pos = Mouse::GetMousePos();
				Mouse::Pos.y = viewport->Size.y - bottombar_height - Mouse::Pos.y;
				Mouse::Pos.x -= sidebar_left_width;

				ImGui::Text("Mouse (%1.f, %1.f)", Mouse::Pos.x, Mouse::Pos.y);
				ImGui::SameLine(0, 20);

				float scale_x = center_window_width / m_ViewportBounds[1].x;
				float scale_y = center_window_height / m_ViewportBounds[1].y;
				ViewportScalers.x = scale_x;
				ViewportScalers.y = scale_y;

				ImGui::BeginGroup();
				{
					ImGui::Text("Center Window (%1.f, %1.f)", center_window_width, center_window_height);
					ImGui::SameLine(0, 20);
					ImGui::Text("Scaled res (%.1f, %.1f)", center_window_width / Mouse::Scalers.x, center_window_height / Mouse::Scalers.y );
					ImGui::SameLine(0, 20);
					ImGui::Text("Centered window pos (%1.f, %1.f) - (%1.f, %1.f)", m_SecondViewportBounds[0].x, m_SecondViewportBounds[0].y, m_SecondViewportBounds[1].x, m_SecondViewportBounds[1].y);
				}
				ImGui::EndGroup();

				ImGui::Dummy({ 0, 2 });

				ImGui::BeginGroup();
				{
					auto textureLibrary = Application::Get()->GetTextureLibrary();
					auto renderer2D = Renderer2D::Get();
					ImGui::Text("Texture Data");
					ImGui::SameLine();

					int boundSlotsCount = renderer2D->GetBoundTextureSlotsCount();
					ImGui::Text("Total slots: %d", boundSlotsCount);
					for (int i = 0; i < boundSlotsCount; i++)
					{
						auto boundTexture = renderer2D->GetBoundTexture(i);
						ImGui::Text("Slot %d: %s,   id: %d", i, boundTexture->GetName().c_str(), boundTexture->GetRendererID());
					}

				}
				ImGui::EndGroup();
				ImGui::EndGroup();

				Mouse::ScaledPos = { (Mouse::Pos.x) / scale_x, (Mouse::Pos.y) / scale_y };
				Mouse::Scalers = { scale_x, scale_y };

				Mouse::SetCenterPos(
					(Mouse::Pos.x / center_window_width) * 2.0f - 1.0f, 
					((Mouse::Pos.y / center_window_height) * 2.0f - 1.0f) * -1.0f
				);

				auto& active_cam = *Scene::GetActiveScene()->GetActiveCamera();
				glm::vec2 cam_pos = active_cam.GetPos();
				ImGui::Text("Camera Pos (%1.f, %1.f)", cam_pos.x, cam_pos.y);

			}
			ImGui::End();
			//--------------------------------------------------

			center_window_width = viewport->Size.x - sidebar_left_width - sidebar_right_width;
			center_window_height = viewport->Size.y - bottombar_height - topbar_height;
			center_window_size = { center_window_width, center_window_height };

			// Lower left point/bound 
			center_window_xpos = sidebar_left_width;
			center_window_ypos = bottombar_height;

			// Top right point/bound
			m_SecondViewportBounds[0] = { center_window_xpos, topbar_height };
			m_SecondViewportBounds[1] = { 
				(center_window_xpos + center_window_width), 
				(center_window_ypos + center_window_height - bottombar_height + topbar_height)
			};

			EditorWindowSize = { center_window_size.x, center_window_size.y };
			EditorWindowPos = { center_window_xpos, center_window_ypos };

			glViewport(center_window_xpos, center_window_ypos, center_window_width, center_window_height);
		}
		ImGui::End(); // Viewport

		HandleExternalWindows();
		ImGui::End(); // Core Viewport
	}

	void EditorLayer::HandleExternalWindows()
	{
		if (ShowRenderInformationWindow)
		{
			UI_RenderSettingsInformation();
		}
	}

	void EditorLayer::SelectEntity(Entity& entity)
	{
		if (SelectedEntityID != entity.GetUUID())
			SelectedEntityID = entity.GetUUID();
	}

	void EditorLayer::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		bool entity_selected = (SelectedEntityID == entity.GetUUID());
		ImGuiTreeNodeFlags flags = (entity_selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			SelectedEntityID = entity.GetUUID();
		}

		bool entityDeleted = false;
		bool entityReset = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete object"))
				entityDeleted= true;
			else if (ImGui::MenuItem("Reset object"))
				entityReset = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Scene->DestroyEntity(entity);
			// will crash ??
			if (SelectedEntityID == entity.GetUUID())
				SelectedEntityID = {};
		}

	}

	template<typename T, typename UIFunction>
	void EditorLayer::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		static const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen 
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (!entity.HasComponent<T>())
			return;

		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

		if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (component.Removable == true && ImGui::MenuItem("Remove component"))
				removeComponent = true;
			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
		{
			// Check if component can be removed
			auto& component = entity.GetComponent<T>();
			if (component.Removable == true)
			{
				LOG_DEBUG("Removing component from {}", entity.GetName());
				entity.RemoveComponent<T>();
			}
		}
		
	}

	template<typename T>
	void EditorLayer::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void EditorLayer::DrawComponents(Entity ent)
	{
		Entity entity = m_Scene->GetEntityWithUUID(SelectedEntityID);
		if (!entity || !entity.IsValid())
			return;

		UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO);

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [&entity](auto& transform)
		{
			ImGui::Text("Position");
			UI::Property::PositionXY(transform.Translation, 2.0);

			ImGui::Text("Scale");
			ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 15.0f, "%.2f");

			ImGui::Text("Rotation");
			ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
			transform.Rotation = glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f));
		});

		DrawComponent<SpriteComponent> ("Color", entity, [&entity](auto& sprite)
		{
			UI::Property::RGBAColor(sprite.Color);
		});

		UI::EndSubwindow();
	}

	std::pair<float, float> EditorLayer::GetMouseViewportSpace(bool primaryViewport)
	{
		auto [mx, my] = ImGui::GetMousePos();
		const auto& viewportBounds = primaryViewport ? m_ViewportBounds : m_SecondViewportBounds;
		mx -= viewportBounds[0].x;
		my -= viewportBounds[0].y;
		auto viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
		auto viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

		return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

    void EditorLayer::UI_SelectedEntityProperties()
    {
		static float pos_step_size = 5.0f;
        auto& scene = *Scene::GetActiveScene();
        Entity entity = scene.GetEntityWithUUID(SelectedEntityID);
		if (!entity)
			return;

        auto window = Window::Get();

		UI::PushID();
        ImGui::BeginChild(UI_SELECTED_ENTITY_INFO, SelectedEntityMenuSize, true);
        if (entity.HasComponent<TransformComponent>())
        {
            ImGui::SeparatorText(SelectedEntity.GetName().c_str());
            ImGui::Indent();

            //DrawImGuizmo(entity);

			TransformComponent& transform = entity.GetComponent<TransformComponent>();
            ImGui::Text("Position");
			UI::Property::PositionXY(transform.Translation, pos_step_size);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 15.0f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
            transform.Rotation = glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f));

			if (entity.HasComponent<SpriteComponent>())
			{
				ImGui::Text("Sprite Color");
				SpriteComponent& sc = entity.GetSpriteComponent();
				UI::Property::RGBAColor(sc.Color);
			}

            ImGui::Unindent();
		}
        ImGui::EndChild();
		UI::PopID();
    }

    void EditorLayer::DrawImGuizmo(Entity& entity)
    {
		TransformComponent& tc = entity.GetComponent<TransformComponent>();
		if (&tc == nullptr) 
			return;
        glm::mat4& transform_matrix = tc.GetTransform();

		auto& scene = *Scene::GetActiveScene();
        auto& cam = *scene.GetActiveCamera();
        auto& cam_pos = cam.GetPos();
        auto& view_matrix = cam.GetView();
        auto& proj_matrix = cam.GetProjection();

        SpriteComponent& sc = entity.GetComponent<SpriteComponent>();

		float pos_x = m_SecondViewportBounds[0].x;
		float pos_y = m_SecondViewportBounds[0].y;
		float width  = m_SecondViewportBounds[1].x - pos_x;
		float height = m_SecondViewportBounds[1].y - pos_y;

		auto window = ImGui::FindWindowByName(UI_CORE_VIEWPORT);
		ImGui::Begin(window->Name);
		ImGui::SetNextWindowViewport(window->ID);
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();
		auto [windowWidth, windowHeight] = ImGui::GetWindowSize();
		//auto spriteRect = Editor::Sprite_GetEdgePoints(sc, tc);
		//auto& left_lower = spriteRect[0];
		//ImGuizmo::SetRect(center_x, center_y, width, height);
        ImGuizmo::SetRect(pos_x, pos_y, width, height);

        ImGuizmo::Manipulate(
            glm::value_ptr(view_matrix), 
            glm::value_ptr(proj_matrix), 
            ImGuizmo::TRANSLATE, 
            ImGuizmo::LOCAL, 
            glm::value_ptr(transform_matrix)
        );

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, scale;
            glm::quat rotation;
            Math::DecomposeTransform(transform_matrix, translation, rotation, scale);
            tc.Translation = translation;
            tc.Scale = scale;
            tc.Rotation = rotation;
        }
		ImGui::End();
    }

	void EditorLayer::UI_HandleManualWindowResize()
	{
		auto window = Application::Get()->GetWindow()->GetGlfwWindow();
		const bool maximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
		ImVec2 newSize, newPosition;
		if (!maximized && UI::UpdateWindowManualResize(ImGui::GetCurrentWindow(), newSize, newPosition))
		{
			glfwSetWindowPos(window, newPosition.x, newPosition.y);
			glfwSetWindowSize(window, newSize.x, newSize.y);
		}
	}

	void EditorLayer::UI_SceneMenu()
	{
		ImGui::PushID("##lkengine-ui-scene-menu");
        ImGui::BeginGroup();
        ImGui::SeparatorText("Scene");

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

        ImGui::SeparatorText("Entity");
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
            }
        }
        ImGui::EndGroup();
        //ImGui::EndChild();
        //ImGui::PopStyleVar(1);
        ImGui::PopID();
	}

	void EditorLayer::UI_SceneContent()
	{
        auto window = Window::Get();
		float menu_height = window->GetHeight() - SelectedEntityMenuSize.y;
		constexpr const char* id = "##lkengine-scene-content";
		UI::PushID(id);
		ImGui::BeginGroup();
		ImGui::SeparatorText("Scene");

        auto& scene = *Scene::GetActiveScene();
        auto& registry = scene.GetRegistry();
        static ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;

		auto entities = registry.view<TransformComponent>();
        for (auto& ent : entities)
        {
            Entity entity = { ent, &scene };
			bool is_selected = SelectedEntity == entity;
            std::string label = fmt::format("{}", entity.GetName());
            if (ImGui::Selectable(label.c_str(), &is_selected, selectable_flags))
            {
                LOG_DEBUG("Selecting {}", label);
				SelectedEntityID = entity.GetComponent<IDComponent>().ID;
				SelectedEntity = entity;
            }
        }
		ImGui::EndGroup();

		UI::PopID(id);
	}

	glm::vec2 EditorLayer::GetEditorWindowSize() const
	{
		glm::vec2 size = { 
			m_SecondViewportBounds[1].x - m_SecondViewportBounds[0].x, 
			m_SecondViewportBounds[1].y - m_SecondViewportBounds[0].y 
		};
		return size;
	}

	void EditorLayer::UI_RenderSettingsInformation()
	{
		if (ImGui::Begin("Render Settings", &ShowRenderInformationWindow, ImGuiWindowFlags_NoDocking))
		{
			auto* graphicsCtx = GraphicsContext::Get();
			ImGui::SeparatorText("Blend Function");

			if (ImGui::BeginCombo(fmt::format("Source: {}", graphicsCtx->GetSourceBlendFunctionName()).c_str(), nullptr, ImGuiComboFlags_NoPreview))
			{
				if (ImGui::MenuItem("Zero"))
					graphicsCtx->SetSourceBlendFunction(SourceBlendFunction::Zero);
				else if (ImGui::MenuItem("One"))
					graphicsCtx->SetSourceBlendFunction(SourceBlendFunction::One);
				else if (ImGui::MenuItem("Alpha"))
					graphicsCtx->SetSourceBlendFunction(SourceBlendFunction::Alpha);
				else if (ImGui::MenuItem("Color"))
					graphicsCtx->SetSourceBlendFunction(SourceBlendFunction::Color);
				else if (ImGui::MenuItem("One_Minus_DestinationAlpha"))
					graphicsCtx->SetSourceBlendFunction(SourceBlendFunction::One_Minus_DestinationAlpha);
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo(fmt::format("Destination: {}", graphicsCtx->GetDestinationBlendFunctionName()).c_str(), nullptr, ImGuiComboFlags_NoPreview))
			{
				if (ImGui::MenuItem("Zero"))
					graphicsCtx->SetDestinationBlendFunction(DestinationBlendFunction::Zero);
				else if (ImGui::MenuItem("One"))
					graphicsCtx->SetDestinationBlendFunction(DestinationBlendFunction::One);
				else if (ImGui::MenuItem("Alpha"))
					graphicsCtx->SetDestinationBlendFunction(DestinationBlendFunction::Alpha);
				else if (ImGui::MenuItem("Color"))
					graphicsCtx->SetDestinationBlendFunction(DestinationBlendFunction::Color);
				else if (ImGui::MenuItem("One_Minus_SourceAlpha"))
					graphicsCtx->SetDestinationBlendFunction(DestinationBlendFunction::One_Minus_SourceAlpha);
				ImGui::EndCombo();
			}

			ImGui::End();
		}
	}


}