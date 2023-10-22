#include "LKpch.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Math/Math.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/Application.h"
#include "LkEngine/UI/ImGuiUtilities.h"


namespace LkEngine {

	uint64_t EditorLayer::SelectedEntityID;
	ImVec2 EditorLayer::SelectedEntityMenuSize = ImVec2(0, 440);
    std::string EditorLayer::SelectedEntityLabel; // remove

	EditorLayer::EditorLayer(s_ptr<Scene> scene)
		: m_Scene(scene)
	{
		m_Name = "Editor Layer";
		m_ViewportBounds[0] = {0, 0};//;, ; {0, 0}
		m_ViewportBounds[1] = {Window::Get()->GetViewportWidth(), Window::Get()->GetViewportHeight()};//;, ; {0, 0}
	}

	void EditorLayer::OnImGuiRender()
	{
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();

		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		static ImGuiWindowFlags core_viewport_flags = ImGuiWindowFlags_NoDocking;
		core_viewport_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		core_viewport_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_MenuBar;

		static ImGuiWindowFlags sidebar_flags = ImGuiWindowFlags_NoDocking;
		sidebar_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		sidebar_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		auto* window = static_cast<GLFWwindow*>(*Application::Get()->GetWindow()->GetGlfwWindow());
		bool isMaximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin(CORE_VIEWPORT, nullptr, core_viewport_flags);
		ImGui::PopStyleColor(); // MenuBarBg
		ImGui::PopStyleVar(2);

		ImGui::PopStyleVar(2);

		UI_HandleManualWindowResize();

		// Dockspace
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe;
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingOverEmpty;
        dockspace_flags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar;
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
		    core_viewport_flags |= ImGuiWindowFlags_NoBackground;
			window_flags |= ImGuiWindowFlags_NoBackground;
		}
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID(LkEngine_DockSpace), ImVec2(0, 0), dockspace_flags);
		style.WindowMinSize.x = minWinSizeX;
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(LkEngine_DockSpace, NULL, window_flags);
		ImGui::PopStyleVar(1);
		{
			// Top menubar
			ImGui::BeginMenuBar();
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) { }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
			//--------------------------------------------------

			static float sidebar_left_width = 340.0f;
			static float sidebar_left_height = ImGui::GetContentRegionAvail().y;
			ImGui::SetNextWindowSize(ImVec2(sidebar_left_width, sidebar_left_height), ImGuiCond_Once);
			ImGui::SetNextWindowSizeConstraints(ImVec2(sidebar_left_width, sidebar_left_height), ImVec2(600, 4000));
			ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetCursorPosY()));
			ImGui::Begin("Left Sidebar", nullptr, sidebar_flags);
			{
				//ImGui::SetCursorScreenPos(ImVec2(0, 100));
				ImGui::Text("Left Content");

				if (m_Scene)
				{
					auto& registry = m_Scene->GetRegistry();
					registry.each([&](auto entityID)
						{
							Entity entity{ entityID, m_Scene.get() };
							DrawEntityNode(entity);
						});

					if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
						SelectedEntityID = {};
				}
				Entity SelectedEntity = m_Scene->GetEntityWithUUID(SelectedEntityID);
				{
					ImGui::BeginChild("Properties");
					{
						ImGui::Text("Properties");
						if (SelectedEntity)
						{
							DrawComponents(SelectedEntity);
						}
					}
					ImGui::EndChild(); // Properties
				}

				static ImGuiTreeNodeFlags gizmo_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
				static ImVec2 button_size(76.0f, 30.0f);

				if (SelectedEntity && m_GizmoType != -1)
				{
					DrawImGuizmo(SelectedEntity);
				}
			}
			ImGui::End(); // Left Sidebar
			//--------------------------------------------------

			// Right sidebar
			static float sidebar_right_width = 340.0f;
			static float sidebar_right_height = ImGui::GetContentRegionAvail().y;
			ImGui::SetNextWindowSize(ImVec2(sidebar_right_width, sidebar_right_height), ImGuiCond_Once);
			ImGui::SetNextWindowSizeConstraints(ImVec2(sidebar_right_width, sidebar_right_height), ImVec2(600, 4000));
			ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - sidebar_right_width, ImGui::GetCursorPosY()));
			ImGui::Begin("##lkengine-sidebar-right", nullptr, sidebar_flags);
			{
				ImGui::Text("Right sidebar content");
			}
			ImGui::End();
			//--------------------------------------------------


		}
		ImGui::End(); // Viewport
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
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen 
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
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

	void EditorLayer::DrawComponents(Entity entity)
	{
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

		DrawComponent<TransformComponent>("Transform", entity, [&entity](auto& component)
		{
			/* Translation / Position */
			glm::vec3 translation = component.Translation;
			//UI::Property::Vector3Control("Position", translation);
			component.Translation = translation;

			/* Scale */
			glm::vec3 scale = component.Scale;
			//UI::Property::Vector3Control("Scale", scale);
			component.Scale = scale;
		});

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

    void EditorLayer::SelectedEntityMenu()
    {
		static float pos_step_size = 5.0f;
        auto& scene = *Scene::ActiveScene;
        Entity entity = scene.FindEntity(SelectedEntityLabel);

        if (!entity)
            return;

        static uint32_t last_id = 0;
        auto window = Window::Get();

        ImGui::SetCursorPosY(window->GetHeight() - SelectedEntityMenuSize.y);
        ImGui::BeginChild("##selected-entity-menu", SelectedEntityMenuSize, true);
        if (entity && entity.HasComponent<TransformComponent>() && entity.HasComponent<MeshComponent>())
        {
            ImGui::SeparatorText(SelectedEntityLabel.c_str());
            ImGui::Indent();
			uint32_t id = entity;
		    ImGui::PushID(id);

			MeshComponent& mesh = entity.GetComponent<MeshComponent>();
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
            ImGui::Text("Position");
			UI::Property::PositionXY(entity, transform.Translation, pos_step_size);

            DrawImGuizmo(entity);

            static float temp_scale = 1.0f;
            ImGui::Text("Scale");
            ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 5.0f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
            transform.Rotation = glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f));

            ImGui::Text("Color");
			UI::Property::RGBAColor(entity, mesh.Color);

            ImGui::Unindent();

            last_id = id;
			ImGui::PopID();
		}
        ImGui::EndChild();
    }

    void EditorLayer::DrawImGuizmo(Entity& entity)
    {
        auto& scene = *Scene::ActiveScene;
        auto& cam = *scene.GetActiveCamera();
        auto& cam_pos = cam.GetPos();
        auto& view_matrix = cam.GetView();
        auto& proj_matrix = cam.GetProjection();
		MeshComponent& mesh = entity.GetComponent<MeshComponent>();
		TransformComponent& tc = entity.GetComponent<TransformComponent>();
        glm::mat4& transform_matrix = tc.GetTransform();

        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::Begin(RENDER_WINDOW, NULL, flags);
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist();
        float width = (float)ImGui::GetWindowWidth();
        float height = (float)ImGui::GetWindowHeight();

        float pos_x = ImGui::GetWindowPos().x;
        float pos_y = ImGui::GetWindowPos().y;

        SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
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
		auto* window = static_cast<GLFWwindow*>(*Application::Get()->GetWindow()->GetGlfwWindow());
		const bool maximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
		ImVec2 newSize, newPosition;
		if (!maximized && UI::UpdateWindowManualResize(ImGui::GetCurrentWindow(), newSize, newPosition))
		{
			glfwSetWindowPos(window, newPosition.x, newPosition.y);
			glfwSetWindowSize(window, newSize.x, newSize.y);
		}
	}




}