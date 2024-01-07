#include "LKpch.h"
#include "LkEngine/Editor/Editor.h"
#include "LkEngine/Debug/DebugLayer.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/UI/ImGuiUtilities.h"

#include "LkEngine/UI/OpenGLImGui.h"


namespace LkEngine {

	static bool WindowsHaveChangedInSize = true; // Run once when starting

	Editor::Editor()
		: m_Scene(nullptr)
		, m_ActiveWindowType(WindowType::None)
	{
		s_Instance = this;
		m_ShowStackTool = true;
		m_ActiveWindowType = WindowType::Viewport;

		m_ViewportBounds[0] = { 0, 0 };
		m_ViewportBounds[1] = { Window::Get()->GetViewportWidth(), Window::Get()->GetViewportHeight() };
		m_SecondViewportBounds[0] = { 0, 0 };
		m_SecondViewportBounds[1] = { 0, 0 };

		LeftSidebarSize.x = 340.0f;
		LeftSidebarSize.y = m_ViewportBounds[1].y;
		RightSidebarSize.x = 340.0f;
		RightSidebarSize.y = m_ViewportBounds[1].y;

		BottomBarSize.x = 0.0f;
		BottomBarSize.y = 340.0f;

		MenuBarSize.x = 0.0f;
		MenuBarSize.y = 30.0f;

		TabBarSize.x = 0.0f;
		TabBarSize.y = 34.0f;

		EditorWindowPos = { LeftSidebarSize.x, BottomBarSize.y };
		EditorWindowSize.x = m_ViewportBounds[1].x - LeftSidebarSize.x - RightSidebarSize.x;
		EditorWindowSize.y = m_ViewportBounds[1].y - BottomBarSize.y /* - topbar_height */;
		LK_ASSERT(EditorWindowSize.x != 0 && EditorWindowSize.y != 0 && EditorWindowSize.x > 0 && EditorWindowSize.y > 0);

		ViewportScalers.x = EditorWindowSize.x / m_ViewportBounds[1].x;
		ViewportScalers.y = EditorWindowSize.y / m_ViewportBounds[1].y;

		Window::Get()->SetScalers(ViewportScalers.x, ViewportScalers.y);
		Window::Get()->SetWidth(EditorWindowSize.x);
		Window::Get()->SetHeight(EditorWindowSize.y);

		m_EditorCamera = new EditorCamera();
		m_EditorCamera->SetOrthographic(Window::Get()->GetWidth(), Window::Get()->GetHeight(), -1.0f, 1.0f);

		m_TabManager.Init();
		// Add Viewport tab as initial tab which is an unremovable tab
		auto viewportTab = m_TabManager.NewTab("Viewport", EditorTabType::Viewport, true);

		m_Enabled = true;
	}

	void Editor::RenderImGui()
	{
		auto* app = Application::Get();
		auto* window = Window::Get();
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();

		UI::BeginViewport(UI_CORE_VIEWPORT, window, viewport);
		UI_HandleManualWindowResize();
		UI::BeginDockSpace(LkEngine_DockSpace);

		if (m_Scene)
		{
			SelectedEntity = m_Scene->GetEntityWithUUID(SelectedEntityID);
		}

		ViewportScalers.x = EditorWindowSize.x / m_ViewportBounds[1].x;
		ViewportScalers.y = EditorWindowSize.y / m_ViewportBounds[1].y;

		// The window space is calculated from topleft corner, so remove Mouse::Pos.y to get the actual cursor placement
		{
			Mouse::Pos = Mouse::GetRawPos();
			Mouse::Pos.x -= LeftSidebarSize.x;
			Mouse::Pos.y = viewport->WorkSize.y - BottomBarSize.y - Mouse::Pos.y;

			Mouse::ScaledPos.x = (Mouse::Pos.x) / ViewportScalers.x;
			Mouse::ScaledPos.y = (Mouse::Pos.y) / ViewportScalers.y;

			Mouse::CenterPos.x = (Mouse::Pos.x / window->GetWidth()) * 2.0f - 1.0f;
			Mouse::CenterPos.y = ((Mouse::Pos.y / window->GetHeight()) * 2.0f - 1.0f) * -1.0f; // was -1.0f 
		}
#ifdef YAML_CPP_DLL
		std::string s;
#endif

		//--------------------------------------------------
		// Menubar
		//--------------------------------------------------
		ImGui::BeginMainMenuBar();
		{
			MenuBarSize.x = ImGui::GetCurrentWindow()->Size.x;
			MenuBarSize.y = ImGui::GetFrameHeight();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) 
				{ 
				}
				if (ImGui::MenuItem("Save"))
				{
					LOG_DEBUG("File->Save");
					SceneSerializer serializer(m_Scene);
					std::string readString;
					serializer.Serialize("scene.lukkelele");
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Render Information"))
				ShowRenderInformationWindow = !ShowRenderInformationWindow;

		}
		ImGui::EndMainMenuBar();

		//--------------------------------------------------
		// Left Sidebar
		//--------------------------------------------------
		if (ShouldUpdateWindowSizes)
			UpdateLeftSidebarSize(viewport);
		ImGui::Begin(UI_SIDEBAR_LEFT, nullptr, UI::SidebarFlags);
		{
			// TODO: initialize this bool by checking the depth in the graphics context
			static bool depth_test_checkbox = false;
			if (ImGui::Checkbox("Depth Testing", &depth_test_checkbox))
			{
				if (depth_test_checkbox == true)
					Window::Get()->SetDepthEnabled(true);
				else
					Window::Get()->SetDepthEnabled(false);
			}

			//----------------------------------------
			// Colors
			//----------------------------------------
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Colors"))
			{
				UI_BackgroundColorModificationMenu();
				ImGui::TreePop();
			}

			//----------------------------------------
			// Mode Selector
			//----------------------------------------
			ImGui::BeginGroup();
			{
				static ImVec2 modeButtonSize = { 50.0f, 50.0f };
				static ImVec4 modeButtonBgColor = { 0, 0, 0, 0 };
				static ImVec4 modeButtonTintColor = { 1, 1, 1, 1 };
				if (ImGui::ImageButton("##ModeButton-NormalMode", (void*)TextureLibrary::Get()->GetTexture2D("atte_square")->GetRendererID(), modeButtonSize, ImVec2(1, 1), ImVec2(0, 0), modeButtonBgColor, modeButtonTintColor))
				{
					LOG_DEBUG("Push tab");
					m_TabManager.NewTab(fmt::format("Node Editor-{}", m_TabManager.GetTabCount()), EditorTabType::NodeEditor);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton("##ModeButton-NodeEditor", (void*)TextureLibrary::Get()->GetTexture2D("atte_square")->GetRendererID(), modeButtonSize, ImVec2(1, 1), ImVec2(0, 0), modeButtonBgColor, modeButtonTintColor))
				{
					LOG_DEBUG("Pop tab");
					if (m_TabManager.GetTabCount() > 1)
						m_TabManager.PopTab();
				}
			}
			ImGui::EndGroup();


			if (SelectedEntity)
			{
				DrawComponents(SelectedEntity);
			}
			
			ImGui::BeginGroup();
			{
				ImGui::Text("Selected ID: %llu", SelectedEntityID);
				ImGui::Text("Active Scene: %s", Scene::GetActiveSceneName().c_str());
				ImGui::Text("Scenes in memory: %d", Scene::GetSceneCount());
			}
			ImGui::EndGroup();

			// Creator menu
			ImGui::BeginGroup();
			{
				UI_CreateMenu();
			}
			ImGui::EndGroup();

			if (m_Scene)
			{
				ImGui::SeparatorText("Current Scene");
				auto& registry = m_Scene->GetRegistry();
				registry.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Scene };
					DrawEntityNode(entity);
				});

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				{
					SelectedEntityID = {};
				}
			}

			auto windowSize = ImGui::GetWindowSize();
			auto windowPos = ImGui::GetWindowPos();
			if (windowSize.x != last_sidebar_left_size.x || windowSize.y != last_sidebar_left_size.y)
				WindowsHaveChangedInSize = true;
			last_sidebar_left_pos = windowPos;
			last_sidebar_left_size = windowSize;
		}
		ImGui::End();  

		//--------------------------------------------------
		// Right Sidebar
		//--------------------------------------------------
		if (ShouldUpdateWindowSizes)
			UpdateRightSidebarSize(viewport);
		ImGui::Begin(UI_SIDEBAR_RIGHT, nullptr, UI::SidebarFlags);
		{
			ImGui::BeginGroup();
			{
				if (ImGui::Checkbox("Style Editor", &m_ShowStyleEditor));

				if (SelectedEntity && m_GizmoType != -1)
					DrawImGuizmo(SelectedEntity);

				if (m_ShowStackTool) 
					ImGui::ShowStackToolWindow();

				if (m_ShowStyleEditor)
				{
					ImGui::Begin("Style Editor");
					ImGui::ShowStyleEditor();
					ImGui::End();
				}

			}
			ImGui::EndGroup();

			// Window Information
			static bool ShowWindowInfo = false;
			ImGui::Checkbox("Show Window Info", &ShowWindowInfo);
			if (ShowWindowInfo)
				UI_ShowViewportAndWindowDetails();

			// Mouse Information
			static bool ShowMouseInfo = true;
			ImGui::Checkbox("Show Mouse Info", &ShowMouseInfo);
			if (ShowMouseInfo)
				UI_ShowMouseDetails();

			auto windowSize = ImGui::GetWindowSize();
			if (windowSize.x != RightSidebarSize.x || windowSize.y != RightSidebarSize.y)
				WindowsHaveChangedInSize = true;

			last_sidebar_right_pos = ImGui::GetWindowPos();
			last_sidebar_right_size = windowSize;
		}
		ImGui::End();


		//--------------------------------------------------
		// BOTTOM BAR
		//--------------------------------------------------
		if (ShouldUpdateWindowSizes)
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x, viewport->Size.y + MenuBarSize.y - BottomBarSize.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - (LeftSidebarSize.x + RightSidebarSize.x), BottomBarSize.y), ImGuiCond_Always);
		}
		ImGui::Begin(UI_BOTTOM_BAR, nullptr, UI::SidebarFlags);
		{

			ImGui::Dummy({ 0, 2 });

			auto& active_cam = *Scene::GetActiveScene()->GetCamera();
			glm::vec2 cam_pos = active_cam.GetPos();
			ImGui::Text("Camera Pos (%1.f, %1.f)", cam_pos.x, cam_pos.y);

			auto windowSize = ImGui::GetWindowSize();
			if (windowSize.x != BottomBarSize.x || windowSize.y != BottomBarSize.y)
				WindowsHaveChangedInSize = true;

			auto* currentWindow = ImGui::GetCurrentWindow();
			last_bottombar_size = windowSize;

			ImGui::BeginGroup();
			{
				UI_ShowViewportAndWindowDetails();
				ImGui::SameLine();
				UI_ShowEditorWindowsDetails();
			}
			ImGui::EndGroup();


		}
		ImGui::End();


		//--------------------------------------------------
		// TABBAR
		//--------------------------------------------------
		static int lastTabCount = 0;
		m_CurrentTabCount = m_TabManager.GetTabCount();

		if (m_CurrentTabCount > 1)
		{
			// Apply viewport update as the tabbar height might've changed
			ImGui::SetNextWindowPos({ LeftSidebarSize.x, MenuBarSize.y }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ viewport->WorkSize.x, TabBarSize.y }, ImGuiCond_Always);
			ImGui::Begin("##LkTabBar", NULL, UI::TabBarFlags);
			{
				if (ImGui::BeginTabBar("MainTab", ImGuiTabBarFlags_Reorderable))
				{
					for (TabEntry tabEntry: m_TabManager.m_Tabs)
					{
						Tab& tab = *tabEntry.second;
						if (ImGui::BeginTabItem(tabEntry.second->Name.c_str()))
						{
							m_TabManager.SetActiveTab(tabEntry.second);
							if (tab.GetTabType() == EditorTabType::Viewport)
							{
								ImGui::EndTabItem();
								continue;
							}
							LOG_TRACE("BeginTabItem  {}", tabEntry.second->Name);

							ImGui::SetNextWindowPos({ m_SecondViewportBounds[0].x, MenuBarSize.y + TabBarSize.y }, ImGuiCond_Always);
							ImGui::SetNextWindowSize({ EditorWindowSize.x, EditorWindowSize.y }, ImGuiCond_Always);
							ImGui::Begin("##TabWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
							tabEntry.second->OnImGuiRender();
							ImGui::End();

							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}
			}
			ImGui::End();
		}
		// No tabs
		else 
		{
		}

		if (lastTabCount != m_CurrentTabCount)
			WindowsHaveChangedInSize = true;

		lastTabCount = m_TabManager.GetTabCount();

		// If the window sizes have been adjusted, set the bool member to false
		// This must be run BEFORE the 'WindowsHaveChangedInSize' if-statement
		if (ShouldUpdateWindowSizes == true)
			ShouldUpdateWindowSizes = false; 

		glm::vec2 viewportSize = { viewport->WorkSize.x, viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(viewportSize);

		ImGui::End(); // Viewport

		HandleExternalWindows();
		ImGui::End(); // Core Viewport
	}

	void Editor::HandleExternalWindows()
	{
		if (ShowRenderInformationWindow)
		{
			UI_RenderSettingsInformation();
		}
	}

	void Editor::SelectEntity(Entity& entity)
	{
		if (SelectedEntityID != entity.UUID())
		{
			SelectedEntityID = entity.UUID();
			LOG_DEBUG("Editor: Selecting entity {} ({})", entity.Name(), entity.UUID());
		}
	}

	void Editor::DrawEntityNode(Entity entity)
	{
		if (entity.HasComponent<IDComponent>() == false || entity.HasComponent<TagComponent>() == false)
			return;

		auto& tag = entity.GetComponent<TagComponent>().Tag;

		bool entity_selected = (SelectedEntityID == entity.UUID());
		ImGuiTreeNodeFlags flags = (entity_selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			SelectedEntityID = entity.UUID();
			LOG_DEBUG("Selecting entity {} ({})", entity.Name(), entity.UUID());
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
			if (SelectedEntityID == entity.UUID())
				SelectedEntityID = {};
		}

	}

	template<typename T, typename UIFunction>
	void Editor::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
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
				LOG_DEBUG("Removing component from {}", entity.Name());
				entity.RemoveComponent<T>();
			}
		}
		
	}

	template<typename T>
	void Editor::DisplayAddComponentEntry(const std::string& entryName)
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

	void Editor::DrawComponents(Entity ent)
	{
		if (!m_Scene)
			return;

		Entity entity = m_Scene->GetEntityWithUUID(SelectedEntityID);
		if (!entity)
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
			auto rot = transform.GetRotation();
			//transform.Rotation = glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f));
			transform.SetRotation(glm::angleAxis(rot.x, glm::vec3(0.0f, 0.0f, 1.0f)));
			//transform.SetRotation(rot);
		});

		DrawComponent<SpriteComponent>("Sprite", entity, [&entity](auto& sprite)
		{
			ImGui::Text("Sprite Component");
			ImGui::Text("Size");
			ImGui::SameLine();
			ImGui::SliderFloat2("##Size", &sprite.Size.x, 0.0f, 800.0f, "%1.f");
			UI::Property::RGBAColor(sprite.Color);
		});

		UI::EndSubwindow();
	}

	std::pair<float, float> Editor::GetMouseViewportSpace(bool primaryViewport)
	{
		auto [mx, my] = ImGui::GetMousePos();
		const auto& viewportBounds = primaryViewport ? m_ViewportBounds : m_SecondViewportBounds;
		mx -= viewportBounds[0].x;
		my -= viewportBounds[0].y;
		auto viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
		auto viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

		return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

    void Editor::UI_SelectedEntityProperties()
    {
		static float pos_step_size = 5.0f;
        auto& scene = *Scene::GetActiveScene();
        Entity entity = scene.GetEntityWithUUID(SelectedEntityID);
		if (!entity)
			return;

        auto* window = Window::Get();
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;

		UI::PushID("UI_SELECTED_ENTITY_PROPERTIES");
        ImGui::BeginChild(UI_SELECTED_ENTITY_INFO, SelectedEntityMenuSize, true, windowFlags);
        if (entity.HasComponent<TransformComponent>())
        {
            ImGui::SeparatorText(SelectedEntity.Name().c_str());
            ImGui::Indent();

			TransformComponent& transform = entity.GetComponent<TransformComponent>();
            ImGui::Text("Position");
			UI::Property::PositionXY(transform.Translation, pos_step_size);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 15.0f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
			transform.SetRotation(glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f)));

            ImGui::Unindent();
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			ImGui::Text("Sprite Component");
			SpriteComponent& sc = entity.GetComponent<SpriteComponent>();
			UI::Property::RGBAColor(sc.Color);
			ImGui::SliderFloat3("##Piss", &sc.Size.x, 0.0f, 800.0f, "%1.f");
			//ImGui::SliderFloat("##Piss", &sc.Size.x, 0.0f, 800.0f, "%1.f");
			//ImGui::SliderFloat("##Piss", &sc.Size.x, 0.0f, 800.0f, "%1.f");
		}

        ImGui::EndChild();
		UI::PopID();
    }

    void Editor::DrawImGuizmo(Entity& entity)
    {
		if (!entity.HasComponent<TransformComponent>())
			return;

		auto& tc = entity.Transform();
        glm::mat4& transform_matrix = tc.GetTransform();

		auto& scene = *Scene::GetActiveScene();
        auto& cam = *scene.GetCamera();
        auto& cam_pos = cam.GetPos();
		//cam_pos.x += camPosOffset.x;
		//cam_pos.y += camPosOffset.y;
        auto& view_matrix = cam.GetView();
        auto& proj_matrix = cam.GetProjection();

		SpriteComponent& sc = entity.Sprite();

		float pos_x = m_SecondViewportBounds[0].x;
		float pos_y = m_SecondViewportBounds[0].y;
		float width  = m_SecondViewportBounds[1].x - m_SecondViewportBounds[0].x;
		float height = m_SecondViewportBounds[1].y - m_SecondViewportBounds[0].y;

		auto window = ImGui::FindWindowByName(UI_CORE_VIEWPORT);
		ImGui::Begin(window->Name);
		ImGui::SetNextWindowViewport(window->ID);

		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();
		// FIXME: The guizmo is somewhat misplaced and is sensitive to vertical translation for some odd reason.
		//        I suspect some scaler bug, but could be many things. For now I just add '10' to center the guizmo a bit more
		if (m_TabManager.GetTabCount() == 1) // Only 1 tab, aka only the 'Viewport' tab
			ImGuizmo::SetRect(pos_x, (pos_y - BottomBarSize.y + MenuBarSize.y + 10), EditorWindowSize.x, EditorWindowSize.y);
		else
			ImGuizmo::SetRect(pos_x, (pos_y - BottomBarSize.y + MenuBarSize.y + TabBarSize.y), EditorWindowSize.x, EditorWindowSize.y);

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
            tc.SetRotation(rotation);
        }
		ImGui::End();
    }

	void Editor::UI_HandleManualWindowResize()
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

	void Editor::UI_SceneContent()
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
            std::string label = fmt::format("{}", entity.Name());
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

	glm::vec2 Editor::GetEditorWindowSize() const
	{
		glm::vec2 size = { 
			m_SecondViewportBounds[1].x - m_SecondViewportBounds[0].x, 
			m_SecondViewportBounds[1].y - m_SecondViewportBounds[0].y 
		};
		return size;
	}

	void Editor::UI_RenderSettingsInformation()
	{
		if (ImGui::Begin("Render Settings", &ShowRenderInformationWindow, ImGuiWindowFlags_NoDocking))
		{
			auto* graphicsCtx = GraphicsContext::Get();

			if (ImGui::BeginCombo("Drawmode", Renderer::GetDrawModeStr().c_str(), NULL))
			{
				if (ImGui::MenuItem("Triangles"))
				{
					LOG_DEBUG("Selected new drawmode 'Triangles'");
					Renderer::SetDrawMode(RendererDrawMode::Triangles);
				}
				if (ImGui::MenuItem("Lines"))
				{
					LOG_DEBUG("Selected new drawmode 'Lines'");
					Renderer::SetDrawMode(RendererDrawMode::Lines);
				}

				ImGui::EndCombo();
			}

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

	bool Editor::IsEntitySelected() const
	{
		return SelectedEntityID > 0;
	}

	void Editor::SetSelectedEntity(Entity& entity)
	{
		if (entity.HasComponent<IDComponent>())
		{
			SelectedEntityID = entity.UUID();
		}
		else
		{
			SelectedEntityID = 0;
		}
		SelectedEntity = entity;
	}

	float Editor::GetEditorWindowWidth() const
	{
		auto windowSize = GetEditorWindowSize();
		return windowSize.x;
	}

	float Editor::GetEditorWindowHeight() const
	{
		auto windowSize = GetEditorWindowSize();
		return windowSize.y;
	}

	void Editor::UI_CreateMenu()
	{
		UI::PushID();
		ImGui::SeparatorText("Create Menu");

		static char nameInputBuffer[140] = "item";
		ImGui::Text("Name");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(140);
		ImGui::InputText("##LkEngine-CreateMenu-InputBuffer", nameInputBuffer, LK_ARRAYSIZE(nameInputBuffer), ImGuiInputFlags_RepeatRateDefault);

		const char* geometricShapes[] = { "Rectangle", "Circle", "Triangle" };
		static int geometricShapeCurrentIndex = 0; // Here we store our selection data as an index
		const char* geometricPreviewValue = geometricShapes[geometricShapeCurrentIndex]; // Pass in the preview value visible before opening the combo (it could be anything)
		static ImGuiComboFlags geometricCreateMenuFlags = ImGuiComboFlags_HeightLargest;
		static Math::Shape currentGeometricShape = Math::Shape::None;

		// Shape
		ImGui::Text("Shape");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(114);
		if (ImGui::BeginCombo("##LkEngine-CreateMenu", geometricPreviewValue, geometricCreateMenuFlags))
		{
			for (int n = 0; n < ARRAYSIZE(geometricShapes); n++)
			{
				const bool is_selected = (geometricShapeCurrentIndex == n);
				if (ImGui::Selectable(geometricShapes[n], is_selected))
					geometricShapeCurrentIndex = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					LOG_DEBUG("COMBO: Selected Item -> {}", geometricShapes[geometricShapeCurrentIndex]);
				}
			}
			ImGui::EndCombo();
		}

		// Selectable geometric shapes
		// Can be clicked on to select diffent shapes instead of dropdown menu
		auto* textureLibrary = TextureLibrary::Get();
		ImGui::BeginGroup();
		{
			static const ImVec4 tintColor = ImVec4(1, 1, 0.90, 1);
			static const ImVec4 bgColor = ImVec4(0, 0, 0, 0);
			static const ImVec2 imageSize = ImVec2(60, 60);

			// Rectangle Image
			s_ptr<Texture> rectangleTexture = textureLibrary->GetTexture2D("atte_square");
			if (ImGui::ImageButton("##RectangleImage", (void*)rectangleTexture->GetRendererID(), imageSize, ImVec2(1, 1), ImVec2(0, 0), bgColor, tintColor))
			{
				LOG_TRACE("Clicked RectangleImage -> Selecting rectangle shape");
				InCreateItemProcess = true;
				for (int i = 0; i < LK_ARRAYSIZE(geometricShapes); i++)
				{
					if (geometricShapes[i] == "Rectangle")
					{
						geometricShapeCurrentIndex = i;
						break;
					}
				}
			}
			ImGui::SameLine(0, 2);

			// Circle Image
			if (ImGui::ImageButton("##CircleImage", (void*)textureLibrary->GetTexture2D("atte_square")->GetRendererID(), imageSize, ImVec2(1, 1), ImVec2(0, 0), bgColor, tintColor))
			{
				LOG_TRACE("Clicked CircleImage -> Selecting circle shape");
				InCreateItemProcess = true;
				for (int i = 0; i < LK_ARRAYSIZE(geometricShapes); i++)
				{
					if (geometricShapes[i] == "Circle")
					{
						geometricShapeCurrentIndex = i;
						break;
					}
				}
			}
			ImGui::SameLine(0, 2);

			// Triangle Image
			if (ImGui::ImageButton("##TriangleImage", (void*)textureLibrary->GetTexture2D("atte_square")->GetRendererID(), imageSize, ImVec2(1, 1), ImVec2(0, 0), bgColor, tintColor))
			{
				LOG_TRACE("Clicked TriangleImage -> Selecting triangle shape");
				InCreateItemProcess = true;
				for (int i = 0; i < LK_ARRAYSIZE(geometricShapes); i++)
				{
					if (geometricShapes[i] == "Triangle")
					{
						geometricShapeCurrentIndex = i;
						break;
					}
				}
			}

		}
		ImGui::EndGroup();

		constexpr unsigned int shapeSizeColumnSize = 50;
		// Determine selected shape and show size modification menu
		// Shape: Rectangle
		if (geometricShapes[geometricShapeCurrentIndex] == "Rectangle")
		{
			constexpr unsigned int rectangle_ColumnPadding = 60;
			currentGeometricShape = Math::Shape::Rectangle;
			static float rectangle_Width = 1.0f;
			static float rectangle_Height = 1.0f;
			// Width
			ImGui::Text("Width");
			ImGui::SameLine(0, rectangle_ColumnPadding - ImGui::CalcTextSize("Width").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Width", &rectangle_Width, 0.10f ,0.010f, 0.0f, "%.2f");
			// Height
			ImGui::Text("Height");
			ImGui::SameLine(0, rectangle_ColumnPadding - ImGui::CalcTextSize("Height").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Height", &rectangle_Height, 0.10f ,0.010f, 0.0f, "%.2f");
		}
		// Shape: Circle
		else if (geometricShapes[geometricShapeCurrentIndex] == "Circle")
		{
			constexpr unsigned int circle_ColumnPadding = 84;
			currentGeometricShape = Math::Shape::Circle;
			static float circle_Radius = 1.0f;
			static float circle_Thickness = 1.0f;
			// Radius
			ImGui::Text("Radius");
			ImGui::SameLine(0, circle_ColumnPadding - ImGui::CalcTextSize("Radius").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Radius", &circle_Radius, 0.010f ,0.010f, 0.0f, "%.2f");
			// Thickness
			ImGui::Text("Thickness");
			ImGui::SameLine(0, circle_ColumnPadding - ImGui::CalcTextSize("Thickness").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Thickness", &circle_Thickness, 0.10f ,0.10f, 0.0f, "%.2f");
		}
		// Shape: Triangle
		else if (geometricShapes[geometricShapeCurrentIndex] == "Triangle")
		{
			currentGeometricShape = Math::Shape::Triangle;
		}
		
		if (ImGui::Button("Add"))
		{
			// Gather characteristics for creating object
			LOG_TRACE("Creating: {}", nameInputBuffer);
		}

		UI::PopID();
	}

	void Editor::SetUpdateWindowFlag(bool flag) 
	{ 
		// Window width and height has been changed as this function has been called,
		// therefore need to update the viewport bounds
		ShouldUpdateWindowSizes = flag; 

		m_ViewportBounds[0] = { 0, 0 };
		m_ViewportBounds[1] = { Window::Get()->GetViewportWidth(), Window::Get()->GetViewportHeight() };

		// The second viewport bounds are updated in the reoccuring update function in the editor
		//m_SecondViewportBounds[0] = { 0, 0 };
		//m_SecondViewportBounds[1] = { 0, 0 };
	}

	void Editor::UI_ShowViewportAndWindowDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("Window Info");
			ImGui::Text("Second Viewport Bounds[0]: (%1.f, %1.f)", m_SecondViewportBounds[0].x, m_SecondViewportBounds[0].y);
			ImGui::Text("Second Viewport Bounds[1]: (%1.f, %1.f)", m_SecondViewportBounds[1].x, m_SecondViewportBounds[1].y);
			ImGui::Text("Second Viewport Size: (%1.f, %1.f)", 
				m_SecondViewportBounds[1].x - m_SecondViewportBounds[0].x, 
				m_SecondViewportBounds[1].y - m_SecondViewportBounds[0].y
			);
			ImGui::Text("Window Size: (%1.f, %1.f)", (float)Window::Get()->GetWidth(), (float)Window::Get()->GetHeight());
			ImGui::Text("Viewport Window Size: (%1.f, %1.f)", m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
			ImGui::Text("Editor Window Size: (%1.f, %1.f)", EditorWindowSize.x, EditorWindowSize.y);
			ImGui::Text("Center Window (%1.f, %1.f)", EditorWindowSize.x, EditorWindowSize.y);
			ImGui::SameLine(0, 20);
			ImGui::Text("Scaled res (%.1f, %.1f)", EditorWindowSize.x / ViewportScalers.x, EditorWindowSize.y / ViewportScalers.y);
			ImGui::Text("Centered window pos (%1.f, %1.f) - (%1.f, %1.f)", m_SecondViewportBounds[0].x, m_SecondViewportBounds[0].y, m_SecondViewportBounds[1].x, m_SecondViewportBounds[1].y);

			glm::vec2 sceneCameraPos = Scene::GetActiveScene()->GetCamera()->GetPos();
			ImGui::Text("SceneCamera Pos: (%.1f, %.1f)", sceneCameraPos.x, sceneCameraPos.y);
		}
		ImGui::EndGroup();
	}

	void Editor::UI_ShowMouseDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::SeparatorText("Mouse Info");
			ImGui::Text("Raw Pos (%1.f, %1.f)", Mouse::Pos.x, Mouse::Pos.y);
			ImGui::Text("Scaled Pos (%.1f, %.1f)", Mouse::ScaledPos.x, Mouse::ScaledPos.y);
			ImGui::Text("Center Normalized (%.2f, %.2f)", Mouse::CenterPos.x, Mouse::CenterPos.y);
			ImGui::Text("Center Scaled (%.2f, %.2f)", (Mouse::CenterPos.x * EditorWindowSize.x * 0.50f) / ViewportScalers.x, Mouse::CenterPos.y * EditorWindowSize.y * 0.50f / ViewportScalers.y) ;
			ImGui::Text("Mouse Scalers (%.2f, %.2f)", ViewportScalers.x, ViewportScalers.y);
			ImGui::Separator();
			ImGui::Text("Last Right Sidebar Size: (%1.f, %1.f)", last_sidebar_right_size.x, last_sidebar_right_size.y);
		}
		ImGui::EndGroup();
	}

	void Editor::UI_ShowEditorWindowsDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("MenuBarSize: (%1.f, %1.f)", MenuBarSize.x, MenuBarSize.y);
			ImGui::Text("TabBarSize: (%1.f, %1.f)", TabBarSize.x, TabBarSize.y);
			ImGui::Text("RightSidebarSize: (%1.f, %1.f)", RightSidebarSize.x, RightSidebarSize.y);
			ImGui::Text("BottomBarSize: (%1.f, %1.f)", BottomBarSize.x, BottomBarSize.y);
			ImGui::Text("Last Bottombar Size: (%1.f, %1.f)", last_bottombar_size.x, last_bottombar_size.y);
			ImGui::Text("Current Tab: %s", m_TabManager.GetActiveTabName().c_str());
		}
		ImGui::EndGroup();
	}

	void Editor::UI_SyncEditorWindowSizes(const glm::vec2& viewportSize)
	{
		// Update editor window size
		EditorWindowSize.x = viewportSize.x - LeftSidebarSize.x - RightSidebarSize.x;
		EditorWindowSize.y = viewportSize.y - BottomBarSize.y;

		// Top right point/bound
		m_SecondViewportBounds[0] = { LeftSidebarSize.x, BottomBarSize.y };
		m_SecondViewportBounds[1] = { 
			(m_SecondViewportBounds[0].x + EditorWindowSize.x),
			(m_SecondViewportBounds[0].y + EditorWindowSize.y)
		};

		// Check to see if any of the editor windows have changed in size and if they have
		// then readjust the viewport
		if (WindowsHaveChangedInSize)
		{
			//auto* viewport = ImGui::GetMainViewport();
			LeftSidebarSize = { last_sidebar_left_size.x, last_sidebar_left_size.y };
			RightSidebarSize = { last_sidebar_right_size.x, last_sidebar_right_size.y };
			BottomBarSize = { last_bottombar_size.x, last_bottombar_size.y };
			EditorWindowPos = { LeftSidebarSize.x, BottomBarSize.y };
			EditorWindowSize.x = viewportSize.x - LeftSidebarSize.x - RightSidebarSize.x;

			// Only take the size of the TabBar into account if any tabs exist
			if (m_CurrentTabCount > 1)
				EditorWindowSize.y = viewportSize.y - BottomBarSize.y - MenuBarSize.y - TabBarSize.y;
			else
				EditorWindowSize.y = viewportSize.y - BottomBarSize.y - MenuBarSize.y;

			// Update viewport scalers as the resolution has been altered
			ViewportScalers.x = EditorWindowSize.x / m_ViewportBounds[1].x;
			ViewportScalers.y = EditorWindowSize.y / m_ViewportBounds[1].y;

			Window::Get()->SetScalers(ViewportScalers);
			Window::Get()->SetWidth(EditorWindowSize.x);
			Window::Get()->SetHeight(EditorWindowSize.y);

			if (m_FillSidebarsVertically)
			{
				LeftSidebarSize.y = viewportSize.y;
				RightSidebarSize.y = viewportSize.y;
			}
			// Reapply viewport settings starting from a lower point of the left sidebar and the bottom bar height
			GraphicsContext::Get()->SetViewport(EditorWindowPos, EditorWindowSize);

			WindowsHaveChangedInSize = false;
			ShouldUpdateWindowSizes = true; // Tell UI to set the window size ONCE
		}

	}

	void Editor::UI_BackgroundColorModificationMenu()
	{
		static ImGuiSliderFlags backgroundSliderFlags = ImGuiSliderFlags_None;
		auto& colors = ImGui::GetStyle().Colors;
		UI::PushID();
		ImGui::BeginGroup();
		{
			ImGui::Text("Background"); 
			ImGui::SliderFloat("##x", &Renderer::BackgroundColor.x, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##y", &Renderer::BackgroundColor.y, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##z", &Renderer::BackgroundColor.z, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##w", &Renderer::BackgroundColor.w, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("UI Alpha", &colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f, " %.2f", backgroundSliderFlags);
		}
		ImGui::EndGroup();
		UI::PopID();
	}

	void Editor::UpdateLeftSidebarSize(ImGuiViewport* viewport)
	{
		ImGui::SetNextWindowPos(ImVec2(0, MenuBarSize.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(LeftSidebarSize.x, viewport->WorkSize.y), ImGuiCond_Always);
	}

	void Editor::UpdateRightSidebarSize(ImGuiViewport* viewport)
	{
		ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - RightSidebarSize.x, MenuBarSize.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(RightSidebarSize.x, RightSidebarSize.y), ImGuiCond_Always);
	}

}
