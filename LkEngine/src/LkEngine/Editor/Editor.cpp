#include "LKpch.h"
#include "LkEngine/Editor/Editor.h"
#include "LkEngine/Debug/DebugLayer.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/UI/SceneManagerPanel.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer.h"


namespace LkEngine {

	static bool WindowsHaveChangedInSize = true; // Run once when starting

	static glm::vec2 MenuBarSize = { 0.0f, 30.0f };
	static glm::vec2 TabBarSize = { 0.0f, 34.0f };
	static glm::vec2 BottomBarSize = { 0.0f, 340.0f };
	static glm::vec2 LeftSidebarSize = { 340.0f, 0.0f };
	static glm::vec2 RightSidebarSize = { 340.0f, 0.0f };
	static glm::vec2 BottomBarPos = { 0.0f, 0.0f };
	static glm::vec2 LeftSidebarPos = { 0.0f, 0.0f };
	static glm::vec2 RightSidebarPos = { 0.0f, 0.0f };

	static bool ShowEditorWindowSizesWindow = false;

	Editor::Editor()
		: m_Scene(nullptr)
		, m_ActiveWindowType(WindowType::None)
		, m_Enabled(true)
	{
		m_Instance = this;
		m_ShowStackTool = false;
		m_ActiveWindowType = WindowType::Viewport;

		Window& window = Application::Get()->GetWindow();
		m_Window = &window;
		LK_CORE_ASSERT(m_Window != nullptr, "Window is nullptr");

		m_ViewportBounds[0] = { 0, 0 };
		m_ViewportBounds[1] = { window.GetViewportWidth(), window.GetViewportHeight() };
		m_SecondViewportBounds[0] = { 0, 0 };
		m_SecondViewportBounds[1] = { 0, 0 };
		LeftSidebarSize.y = m_ViewportBounds[1].y;
		RightSidebarSize.y = m_ViewportBounds[1].y;
		last_bottombar_size = Utils::ConvertToImVec2(BottomBarSize);

		EditorWindowPos = { LeftSidebarSize.x, BottomBarSize.y };
		EditorWindowSize.x = m_ViewportBounds[1].x - LeftSidebarSize.x - RightSidebarSize.x;
		EditorWindowSize.y = m_ViewportBounds[1].y - BottomBarSize.y /* - topbar_height */;

		ViewportScalers.x = EditorWindowSize.x / m_ViewportBounds[1].x;
		ViewportScalers.y = EditorWindowSize.y / m_ViewportBounds[1].y;

		m_Window->SetScalers(ViewportScalers.x, ViewportScalers.y);
		m_Window->SetWidth(EditorWindowSize.x);
		m_Window->SetHeight(EditorWindowSize.y);

		m_EditorCamera = Ref<EditorCamera>::Create(60.0f, Window::Get().GetWidth(), Window::Get().GetHeight(), 0.10f, 2400.0f);
		//m_EditorCamera->SetPosition({ -650, -100, -200 });
		m_EditorCamera->SetPosition({ -10, 1, -10 });
		m_EditorCamera->m_Pitch = 0.260;
		m_EditorCamera->m_Yaw = 2.460;

		m_SceneManagerPanel = new SceneManagerPanel();
		m_ContentBrowser = new ContentBrowser();

		// Tab manager
		{
			m_TabManager = new EditorTabManager();
			m_TabManager->Init();
			auto viewportTab = m_TabManager->NewTab("Viewport", EditorTabType::Viewport, true);
		}

		// Default project
		// TODO: Parse config or cache to determine most recent project and go from there
		m_TargetProject = Ref<Project>::Create();
		m_TargetProject->CreateDefaultProject();
		SetScene(m_TargetProject->Data.TargetScene);
	}

	void Editor::OnUpdate()
	{
		// The window space is calculated from topleft corner, so remove Mouse::Pos.y to get the actual cursor placement
		{
			Mouse::Pos = Mouse::GetRawPos();
			Mouse::Pos.x -= LeftSidebarSize.x;
			//Mouse::Pos.y = viewport->WorkSize.y - BottomBarSize.y - Mouse::Pos.y;
			Mouse::Pos.y = m_ViewportBounds[1].y - BottomBarSize.y - Mouse::Pos.y;

			Mouse::ScaledPos.x = (Mouse::Pos.x) / ViewportScalers.x;
			Mouse::ScaledPos.y = (Mouse::Pos.y) / ViewportScalers.y;

			Mouse::CenterPos.x = (Mouse::Pos.x / m_Window->GetWidth()) * 2.0f - 1.0f;
			Mouse::CenterPos.y = ((Mouse::Pos.y / m_Window->GetHeight()) * 2.0f - 1.0f) * -1.0f; // was -1.0f 
		}
		RenderViewport();
	}

	void Editor::OnRender()
	{
		RenderViewport();
	}

	void Editor::OnImGuiRender()
	{
		auto* app = Application::Get();
		auto& window = m_Window;
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		ViewportScalers.x = EditorWindowSize.x / m_ViewportBounds[1].x;
		ViewportScalers.y = EditorWindowSize.y / m_ViewportBounds[1].y;

		if (m_Scene && SELECTION::SelectedEntity != SELECTION::NULL_ENTITY)
			SELECTION::SelectedEntity = m_Scene->TryGetEntityWithUUID(SELECTION::SelectedEntity.GetUUID());


		UI::BeginViewport(UI_CORE_VIEWPORT, m_Window, viewport);
		UI_HandleManualWindowResize();
		UI::BeginDockSpace(LkEngine_DockSpace);

		auto renderer2D = Renderer2DAPI::Get().As<OpenGLRenderer2D>();
		auto framebuffer2D = renderer2D->GetFramebuffer();

		//=========================================================
		// Menubar
		//=========================================================
		ImGui::BeginMainMenuBar();
		{
			MenuBarSize.x = ImGui::GetCurrentWindow()->Size.x;
			MenuBarSize.y = ImGui::GetFrameHeight();

			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project")) 
			{ 
				// Save project
				if (ImGui::MenuItem("Save")) 
				{ 
					m_TargetProject->Save();
				}
				// New project
				if (ImGui::MenuItem("New")) 
				{ 
					m_TargetProject = Ref<Project>::Create("Project1");
				}
				// Load existing project
				if (ImGui::MenuItem("Load")) 
				{ 
				}
				if (ImGui::MenuItem("Recent")) 
				{ 
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Renderer Settings"))
					ShowRenderSettingsWindow = !ShowRenderSettingsWindow;

				if (ImGui::MenuItem("Editor Window Sizes"))
					ShowEditorWindowSizesWindow = !ShowEditorWindowSizesWindow;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("New")) 
				{ 
				}
				if (ImGui::MenuItem("Load"))
				{
					//m_Scene->Clear();
					Ref<Scene> newScene = Ref<Scene>::Create();
					//Ref<Scene> scene = Ref<Scene>::Create();
					SceneSerializer sceneSerializer(newScene);
					sceneSerializer.Deserialize("scene.lukkelele");
					newScene->CopyTo(m_Scene);
					//m_Scene = newScene;
				}
				if (ImGui::MenuItem("Save"))
				{
					SceneSerializer serializer(m_Scene);
					serializer.Serialize("scene.lukkelele");
				}
				ImGui::EndMenu();
			}

			// Horizontal space
			ImGui::Dummy(ImVec2(40, 0));
			if (ImGui::BeginMenu(std::string("Project: " + m_TargetProject->GetName()).c_str()))
			{
				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();


		//=========================================================
		// Left Sidebar
		//=========================================================
		ImGuiID DockspaceID_LeftSidebar = ImGui::GetID("Dockspace_LeftSidebar");
		static bool ResetDockspace_LeftSidebar = true;
		if (ShouldUpdateWindowSizes)
			UpdateLeftSidebarSize(viewport);
		ImGui::Begin(UI_SIDEBAR_LEFT, nullptr, UI::SidebarFlags);
		{
			static bool blendingEnabled = GraphicsContext::Get()->GetBlendingEnabled();
			if (ImGui::Checkbox("Depth Testing", &blendingEnabled))
			{
				m_Window->SetDepthEnabled(blendingEnabled);
			}

			//====================================================
			// Colors
			//====================================================
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Colors"))
			{
				UI_ClearColorModificationMenu();
				ImGui::TreePop();
			}
			//====================================================
			// Editor Camera
			//====================================================
			ImGui::Text("Editor Cam Mode: %s", (m_EditorCamera->m_CameraMode == EditorCamera::Mode::Arcball ? "Arcball" : "Flycam"));
			static const char* cameraViewTypes[] = { "Perspective", "Orthographic" };
			int selectedEditorCameraViewTypeIndex = (int)m_EditorCamera->GetProjectionType();
			ImGui::Text("Selected View Type Index: %d", selectedEditorCameraViewTypeIndex);
			// Get current camera view mode and set it to that if at init stage
			if (ImGui::BeginCombo("Editor Camera Type", cameraViewTypes[selectedEditorCameraViewTypeIndex]))
			{
				int currentIdx = 0;
				for (auto& viewType : cameraViewTypes)
				{
					//LK_CORE_DEBUG("Current viewtype: {}", viewType);
					const bool isSelected = selectedEditorCameraViewTypeIndex == currentIdx;
					if (ImGui::Selectable(cameraViewTypes[currentIdx], isSelected, ImGuiSelectableFlags_SpanAvailWidth))
					{
						selectedEditorCameraViewTypeIndex = currentIdx;
						if (viewType == "Perspective")
							m_EditorCamera->SetProjectionType(Camera::ProjectionType::Perspective);
						else if (viewType == "Orthographic")
							m_EditorCamera->SetProjectionType(Camera::ProjectionType::Orthographic);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();

					currentIdx++;
				}

				ImGui::EndCombo();
			}
			ImGui::BeginGroup();
			{
				UI::Property::PositionXYZ(m_EditorCamera->m_Position);
				ImGui::SliderFloat("Pitch", &m_EditorCamera->m_Pitch, -12.0f, 12.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("Yaw", &m_EditorCamera->m_Yaw, -12.0f, 12.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			}
			ImGui::EndGroup();

			//====================================================
			// Mode Selector
			//====================================================
			ImGui::BeginGroup();
			{
				static ImVec2 modeButtonSize = { 50.0f, 50.0f };
				static ImVec4 modeButtonBgColor = { 0, 0, 0, 0 };
				static ImVec4 modeButtonTintColor = { 1, 1, 1, 1 };
				std::string textureName = "ale";
				if (ImGui::ImageButton("##ModeButton-NormalMode", (void*)TextureLibrary::Get()->GetTexture2D(textureName)->GetRendererID(), modeButtonSize, ImVec2(1, 1), ImVec2(0, 0), modeButtonBgColor, modeButtonTintColor))
				{
					LK_CORE_DEBUG("Push tab");
					m_TabManager->NewTab(fmt::format("Node Editor-{}", m_TabManager->GetTabCount()), EditorTabType::NodeEditor);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton("##ModeButton-NodeEditor", (void*)TextureLibrary::Get()->GetTexture2D(textureName)->GetRendererID(), modeButtonSize, ImVec2(1, 1), ImVec2(0, 0), modeButtonBgColor, modeButtonTintColor))
				{
					LK_CORE_DEBUG("Pop tab");
					if (m_TabManager->GetTabCount() > 1)
						m_TabManager->PopTab();
				}
			}
			ImGui::EndGroup();

			// Creator menu
			ImGui::BeginGroup();
			{
				UI_CreateMenu();
			}
			ImGui::EndGroup();

			//m_ContentBrowser->OnImGuiRender();
			m_SceneManagerPanel->OnImGuiRender();

			auto windowSize = ImGui::GetWindowSize();
			auto windowPos = ImGui::GetWindowPos();
			if (windowSize.x != last_sidebar_left_size.x || windowSize.y != last_sidebar_left_size.y)
				WindowsHaveChangedInSize = true;
			last_sidebar_left_pos = windowPos;
			last_sidebar_left_size = windowSize;
		}
		ImGui::End();  


		if (SELECTION::SelectedEntity.m_EntityHandle != entt::null && m_GizmoType != -1)
		{
			//DrawImGuizmo(SELECTION::SelectedEntity);
			//if (SELECTION::SelectedEntity.m_Scene != nullptr) DrawImGuizmo(SELECTION::SelectedEntity);
		}

		//====================================================
		// Right Sidebar
		//====================================================
		if (ShouldUpdateWindowSizes)
			UpdateRightSidebarSize(viewport);
		ImGui::Begin(UI_SIDEBAR_RIGHT, nullptr, UI::SidebarFlags);
		{

			m_ComponentEditor.OnImGuiRender();

			// Window Information
			if (ImGui::TreeNode("Window Information"))
			{
				UI_ShowViewportAndWindowDetails();
				ImGui::TreePop();
			}
			// Mouse Information
			if (ImGui::TreeNode("Mouse Information"))
			{
				UI_ShowMouseDetails();
			}

			auto windowSize = ImGui::GetWindowSize();
			if (windowSize.x != RightSidebarSize.x || windowSize.y != RightSidebarSize.y)
				WindowsHaveChangedInSize = true;

			last_sidebar_right_pos = ImGui::GetWindowPos();
			last_sidebar_right_size = windowSize;

			ImGui::Separator();
		}
		ImGui::End();

		// Draw component BELOW the content in the right sidebar
		//if (SELECTION::SelectedEntity && SELECTION::SelectedEntity.m_Scene != nullptr && SELECTION::SelectedEntity.m_EntityHandle != entt::null)
		SceneManagerPanel::DrawComponents(SELECTION::SelectedEntity);
			

		//====================================================
		// Bottom Bar
		//====================================================
		if (ShouldUpdateWindowSizes)
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x, viewport->Size.y + MenuBarSize.y - BottomBarSize.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - (LeftSidebarSize.x + RightSidebarSize.x), BottomBarSize.y), ImGuiCond_Always);
		}
		ImGui::Begin(UI_BOTTOM_BAR, nullptr, UI::SidebarFlags);
		{
			m_ContentBrowser->OnImGuiRender();
		}
		ImGui::End();

		//====================================================
		// Tab bar
		//====================================================
		static int lastTabCount = 0;
		m_CurrentTabCount = m_TabManager->GetTabCount();

		if (m_CurrentTabCount > 1)
		{
			// Apply viewport update as the tabbar height might've changed
			ImGui::SetNextWindowPos({ LeftSidebarSize.x, MenuBarSize.y }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ viewport->WorkSize.x, TabBarSize.y }, ImGuiCond_Always);
			ImGui::Begin("##LkTabBar", NULL, UI::TabBarFlags);
			{
				if (ImGui::BeginTabBar("MainTab", ImGuiTabBarFlags_Reorderable))
				{
					for (TabEntry tabEntry: m_TabManager->m_Tabs)
					{
						Tab& tab = *tabEntry.second;
						const bool is_selected_tab = (m_TabManager->GetActiveTab()->Index == tab.Index);
						constexpr float padding = { 36.0f };
						if (is_selected_tab)
							ImGui::SetNextItemWidth(ImGui::CalcTextSize(tab.Name.c_str()).x + padding);
						if (ImGui::BeginTabItem(tabEntry.second->Name.c_str(), nullptr))
						{
							if (is_selected_tab)
							{
								auto pos = (ImGui::GetItemRectMin());
								pos.x = ImGui::GetItemRectMax().x;
								pos.x -= padding * 0.75f;
								pos.y += padding * (1.0f / 8.0f);

								if (ImGui::CloseButton(ImGui::GetID(UI::GenerateID()), pos))
								{
									if (tab.GetTabType() != EditorTabType::Viewport)
										m_TabManager->CloseTab(tabEntry.second);
								}
							}

							if (tab.Closed == false)
								m_TabManager->SetActiveTab(tabEntry.second);

							if (tab.GetTabType() == EditorTabType::Viewport || tab.Closed == true)
							{
								ImGui::EndTabItem();
								continue;
							}

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
			ImGui::End(); // TabBar
		}
		// No tabs
		else 
		{
		}

		if (lastTabCount != m_CurrentTabCount)
			WindowsHaveChangedInSize = true;

		lastTabCount = m_TabManager->GetTabCount();

		//===================================================================
		// Main Window
		//===================================================================
		static ImVec2 statsWindowSize = ImVec2(ImGui::CalcTextSize("FPS: xyz").x + 200, 400);
		ImGui::SetCursorPos(Utils::ConvertToImVec2(EditorWindowPos));
		UI::Begin(UI_CORE_VIEWPORT, UI::CoreViewportFlags);
		{
			OpenGLFramebuffer& framebuffer = *Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer();
			Ref<Image2D> viewportImage = framebuffer.GetImage(0);
			ImGui::Begin("ViewportTexture", nullptr, UI::CoreViewportFlags | ImGuiWindowFlags_NoScrollWithMouse);
			//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			//ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			UI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			UI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			UI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			UI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
			ImGui::ImageButton((ImTextureID)viewportImage->GetRendererID(), ImVec2(EditorWindowSize.x, EditorWindowSize.y + MenuBarSize.y), ImVec2(0, 1), ImVec2(1, 0));
			//UI::PopStyleStack();
			UI::PopStyleColor(3);
			UI::PopStyleVar(1);
			//ImGui::PopStyleVar(1);
			//ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget())
			{
			    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FOLDER_DATA_TYPE", ImGuiDragDropFlags_None);
			    if (payload)
			    {
			        LK_CORE_DEBUG_TAG("ContentBrowser", "Folder - AcceptDragDropPayload  Size={}", (int)payload->SourceId);
			    }

				ImGui::EndDragDropTarget();
			}
			//ImGui::Text("Viewport Image: {}", viewportImage->GetSpecification().DebugName);

			if (SELECTION::SelectedEntity)
				DrawImGuizmo(SELECTION::SelectedEntity);

			ImGui::End();
			

			//=========================================================
			// Window statistics, FPS counter etc.
			//=========================================================
			if (m_TabManager->GetTabCount() == 1)
				ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x + EditorWindowSize.x - statsWindowSize.x, MenuBarSize.y), ImGuiCond_Always);
			else
				ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x + EditorWindowSize.x - statsWindowSize.x, MenuBarSize.y + TabBarSize.y), ImGuiCond_Always);
			ImGui::BeginChild("##WindowStats", statsWindowSize, false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
			{
				float fps = 1000.0f / app->GetTimestep();
				ImGui::Text("FPS: %1.f", fps);

				// Editor Camera information
				if (m_EditorCamera->m_IsActive)
				{
					ImGui::Text("FOV: %1.f", m_EditorCamera->m_DegPerspectiveFOV);
					const glm::vec3 camPos = m_EditorCamera->GetPosition();
					ImGui::Text("Pos (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
					ImGui::Text("Pitch: %.3f  Yaw: %.3f", m_EditorCamera->GetPitch(), m_EditorCamera->GetYaw());
					ImGui::Text("Camera Zoom: %.3f", m_EditorCamera->ZoomSpeed());
					ImGui::Text("Speed: %.3f", m_EditorCamera->GetCameraSpeed());
					ImGui::Text("Distance: %.2f", m_EditorCamera->GetDistance());
					ImGui::Text("Focalpoint: (%.2f, %.2f, %.2f)", m_EditorCamera->GetFocalPoint().x, m_EditorCamera->GetFocalPoint().y, m_EditorCamera->GetFocalPoint().z);
					if (SELECTION::SelectedEntity)
						ImGui::Text("Selected ID: %llu", SELECTION::SelectedEntity.GetUUID());
					else
						ImGui::Text("Selected ID: None");
				}
				ImGui::EndChild();
			}
		}
		UI::End();


		// If the window sizes have been adjusted, set the bool member to false
		// This must be run BEFORE the 'WindowsHaveChangedInSize' if-statement
		if (ShouldUpdateWindowSizes == true)
			ShouldUpdateWindowSizes = false; 

		glm::vec2 viewportSize = { viewport->WorkSize.x, viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(viewportSize);

		// Take care of tabs here
		m_TabManager->End();

		// Temporary check
		//if (!ScreenShader)
		//	ScreenShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Screen");
		//ScreenShader->Bind();
		//glDisable(GL_DEPTH_TEST);
		//glBindVertexArray(QuadVAO);
		//glBindTexture(GL_TEXTURE_2D, viewportImage->GetRendererID());
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		ImGui::End(); // Viewport
		HandleExternalWindows();
		ImGui::End(); // Core Viewport
	}

	void Editor::RenderViewport()
	{
		auto& framebuffer2D = *Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer();
		auto viewportImage = framebuffer2D.GetImage(0);
		//LK_CORE_DEBUG("Viewport Image: {}", viewportImage->GetSpecification().DebugName);

		framebuffer2D.Bind();
		framebuffer2D.BindTexture(0);

		RenderMirrorTexture(m_EditorCamera->GetViewMatrix(), m_EditorCamera->GetProjectionMatrix());
		RenderCubes(m_EditorCamera->GetViewMatrix(), m_EditorCamera->GetProjectionMatrix());
		RenderFloor(m_EditorCamera->GetViewMatrix(), m_EditorCamera->GetProjectionMatrix());
		Renderer::SubmitQuad({ 200, 0, 640 }, { 140, 90 }, Color::RGBA::Red);

		framebuffer2D.Unbind();
	}

	void Editor::RenderViewport(Ref<Image> image)
	{
	}

	void Editor::HandleExternalWindows()
	{
		if (ShowRenderSettingsWindow)
			UI_RenderSettingsWindow();

		if (ShowEditorWindowSizesWindow)
		{
			ImGui::Begin("Editor Window Sizes", &ShowEditorWindowSizesWindow);
			UI_ShowEditorWindowsDetails();
			ImGui::End();
		}
	}

	void Editor::OnEvent(Event& e)
	{
		switch (e.GetEventType())
		{
			case EventType::MouseButtonPressed:
			{
				break;
			}
			case EventType::MouseButtonReleased:
			{
				break;
			}
			case EventType::MouseScrolled:
			{
				e.Handled = m_EditorCamera->OnMouseScroll(static_cast<MouseScrolledEvent&>(e));
				break;
			}
			case EventType::KeyPressed:
			{
				e.Handled = m_EditorCamera->OnKeyPress(static_cast<KeyPressedEvent&>(e));
				break;
			}
			case EventType::KeyReleased:
			{
				break;
			}
		}
	}

    void Editor::DrawImGuizmo(Entity entity)
    {
		if (!entity.HasComponent<TransformComponent>())
		{
			LK_CORE_ASSERT(false, "Entity doesnt have a transform component");
			return;
		}

		auto& framebuffer2D = *Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer();

		auto& tc = entity.Transform();
        glm::mat4 transform_matrix = tc.GetTransform();

		//auto& scene = *Scene::GetActiveScene();
        //EditorCamera& cam = *scene.GetCamera();
		//cam_pos.x += camPosOffset.x;
		//cam_pos.y += camPosOffset.y;
        auto& cameraPosition = m_EditorCamera->GetPosition();
        auto& viewMatrix = m_EditorCamera->GetViewMatrix();
        auto& projectionMatrix = m_EditorCamera->GetProjectionMatrix();

		SpriteComponent& sc = entity.Sprite();

		float pos_x = m_SecondViewportBounds[0].x;
		float pos_y = m_SecondViewportBounds[0].y;
		float width  = m_SecondViewportBounds[1].x - m_SecondViewportBounds[0].x;
		float height = m_SecondViewportBounds[1].y - m_SecondViewportBounds[0].y;

		//auto window = ImGui::FindWindowByName(UI_CORE_VIEWPORT);
		//auto window = ImGui::FindWindowByName("ViewportTexture");
		//ImGui::SetNextWindowViewport(window->ID);
		//ImGui::Begin(window->Name, nullptr, UI::CoreViewportFlags | ImGuiWindowFlags_NoScrollbar);
		{
			//ImGui::SetNextWindowViewport(window->ID);
			ImGuizmo::SetOrthographic((int)m_EditorCamera->GetProjectionType());
			ImGuizmo::SetDrawlist();
			if (m_TabManager->GetTabCount() == 1) // Only 1 tab, aka only the 'Viewport' tab
				ImGuizmo::SetRect(pos_x, (pos_y - BottomBarSize.y + MenuBarSize.y), EditorWindowSize.x, EditorWindowSize.y);
			else
				ImGuizmo::SetRect(pos_x, (pos_y - BottomBarSize.y + MenuBarSize.y + TabBarSize.y), EditorWindowSize.x, EditorWindowSize.y);

			ImGuizmo::Manipulate(
			    glm::value_ptr(viewMatrix), 
			    glm::value_ptr(projectionMatrix), 
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
		}
		//ImGui::End();
    }

	void Editor::UI_HandleManualWindowResize()
	{
		auto window = Application::Get()->GetWindow().GetGlfwWindow();
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
        auto& window = m_Window;
		float menu_height = window->GetHeight() - SelectedEntityMenuSize.y;
		constexpr const char* id = "##lkengine-scene-content";
		UI::PushID(id);
		ImGui::BeginGroup();
		ImGui::SeparatorText("Scene");

        //auto& scene = *Scene::GetActiveScene();
        //auto& registry = scene.GetRegistry();
        static ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;

		auto entities = m_Scene->m_Registry.view<TransformComponent>();
		ImGui::Text("Entities in scene: %d", entities.size());
        for (auto& ent : entities)
        {
            Entity entity = { ent, m_Scene.Raw() };
			bool is_selected = SELECTION::SelectedEntity == entity;
            std::string label = fmt::format("{}", entity.Name());
            if (ImGui::Selectable(label.c_str(), &is_selected, selectable_flags))
            {
                LK_CORE_DEBUG("Selecting {}", label);
				SELECTION::SelectedEntity = entity;
				//SELECTION::SelectedEntityID = entity.GetComponent<IDComponent>().ID;
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

	void Editor::UI_RenderSettingsWindow()
	{
		if (ImGui::Begin("Render Settings", &ShowRenderSettingsWindow, ImGuiWindowFlags_NoDocking))
		{
			//auto graphicsCtx = Ref<GraphicsContext>();
			//GraphicsContext* graphicsCtx = Ref<GraphicsContext>().Raw();
			Ref<GraphicsContext> graphicsCtx = GraphicsContext::Get();

			//bool& blending = GraphicsContext::Get()->GetBlending();
			bool blending = graphicsCtx->GetBlendingEnabled();
			if (ImGui::Checkbox("Blending", &blending))
			{
				if (blending == true)
					graphicsCtx->SetBlendingEnabled(true);
				else
					graphicsCtx->SetBlendingEnabled(false);
			}

			if (ImGui::BeginCombo("Drawmode", Renderer::GetDrawModeStr().c_str(), NULL))
			{
				if (ImGui::MenuItem("Triangles"))
				{
					LK_CORE_DEBUG("Selected new drawmode 'Triangles'");
					Renderer::SetDrawMode(RendererDrawMode::Triangles);
				}
				if (ImGui::MenuItem("Lines"))
				{
					LK_CORE_DEBUG("Selected new drawmode 'Lines'");
					Renderer::SetDrawMode(RendererDrawMode::Lines);
				}

				ImGui::EndCombo();
			}

			ImGui::SeparatorText("Blend Function");
			if (ImGui::BeginCombo(fmt::format("Source: {}", graphicsCtx->GetCurrentSourceBlendFunctionName()).c_str(), nullptr, ImGuiComboFlags_NoPreview))
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

			if (ImGui::BeginCombo(fmt::format("Destination: {}", graphicsCtx->GetCurrentDestinationBlendFunctionName()).c_str(), nullptr, ImGuiComboFlags_NoPreview))
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
		UI::PushID("UI_CreateMenu");
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
					LK_CORE_DEBUG("COMBO: Selected Item -> {}", geometricShapes[geometricShapeCurrentIndex]);
				}
			}
			ImGui::EndCombo();
		}

		// Selectable geometric shapes
		// Can be clicked on to select diffent shapes instead of dropdown menu
		auto textureLibrary = TextureLibrary::Get();
		ImGui::BeginGroup();
		{
			static const ImVec4 tintColor = ImVec4(1, 1, 0.90, 1);
			static const ImVec4 bgColor = ImVec4(0, 0, 0, 0);
			static const ImVec2 imageSize = ImVec2(60, 60);

			static std::string textureName = "ale";
			// Rectangle Image
			auto rectangleTexture = textureLibrary->GetTexture2D(textureName);
			if (ImGui::ImageButton("##RectangleImage", (void*)rectangleTexture->GetRendererID(), imageSize, ImVec2(1, 1), ImVec2(0, 0), bgColor, tintColor))
			{
				LK_CORE_TRACE("Clicked RectangleImage -> Selecting rectangle shape");
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
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("FOLDER_DATA_TYPE", "Wow!", sizeof("Wow!"));
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine(0, 2);

			// Circle Image
			if (ImGui::ImageButton("##CircleImage", (void*)textureLibrary->GetTexture2D(textureName)->GetRendererID(), imageSize, ImVec2(1, 1), ImVec2(0, 0), bgColor, tintColor))
			{
				LK_CORE_TRACE("Clicked CircleImage -> Selecting circle shape");
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
			if (ImGui::ImageButton("##TriangleImage", (void*)textureLibrary->GetTexture2D(textureName)->GetRendererID(), imageSize, ImVec2(1, 1), ImVec2(0, 0), bgColor, tintColor))
			{
				LK_CORE_TRACE("Clicked TriangleImage -> Selecting triangle shape");
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
			LK_CORE_TRACE("Creating: {}", nameInputBuffer);
		}

		UI::PopID();
	}

	void Editor::SetUpdateWindowFlag(bool flag) 
	{ 
		// Window width and height has been changed as this function has been called,
		// therefore need to update the viewport bounds
		ShouldUpdateWindowSizes = flag; 

		m_ViewportBounds[0] = { 0, 0 };
		m_ViewportBounds[1] = { m_Window->GetViewportWidth(), m_Window->GetViewportHeight()};

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
			ImGui::Text("Window Size: (%1.f, %1.f)", (float)m_Window->GetWidth(), (float)m_Window->GetHeight());
			ImGui::Text("Viewport Window Size: (%1.f, %1.f)", m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
			ImGui::Text("Editor Window Size: (%1.f, %1.f)", EditorWindowSize.x, EditorWindowSize.y);
			ImGui::Text("Center Window (%1.f, %1.f)", EditorWindowSize.x, EditorWindowSize.y);
			ImGui::SameLine(0, 20);
			ImGui::Text("Scaled res (%.1f, %.1f)", EditorWindowSize.x / ViewportScalers.x, EditorWindowSize.y / ViewportScalers.y);
			ImGui::Text("Centered window pos (%1.f, %1.f) - (%1.f, %1.f)", m_SecondViewportBounds[0].x, m_SecondViewportBounds[0].y, m_SecondViewportBounds[1].x, m_SecondViewportBounds[1].y);

			if (m_EditorCamera->m_IsActive)
			{
				glm::vec2 cameraPos = m_EditorCamera->GetPosition();
				ImGui::Text("EditorCamera Position: (%.1f, %.1f)", cameraPos.x, cameraPos.y);
			}

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
			ImGui::Text("Current Tab: %s", m_TabManager->GetActiveTabName().c_str());
			ImGui::Text("Tabs: %d", m_TabManager->GetTabCount());
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
				EditorWindowSize.y = viewportSize.y - BottomBarSize.y;
			else
				EditorWindowSize.y = viewportSize.y - BottomBarSize.y + TabBarSize.y;

			EditorWindowPos.y -= MenuBarSize.y;

			// Update viewport scalers as the resolution has been altered
			ViewportScalers.x = EditorWindowSize.x / m_ViewportBounds[1].x;
			ViewportScalers.y = EditorWindowSize.y / m_ViewportBounds[1].y;

			m_Window->SetScalers(ViewportScalers);
			m_Window->SetWidth(EditorWindowSize.x);
			m_Window->SetHeight(EditorWindowSize.y);

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

	void Editor::UI_ClearColorModificationMenu()
	{
		static ImGuiSliderFlags backgroundSliderFlags = ImGuiSliderFlags_None;
		auto& colors = ImGui::GetStyle().Colors;
		ImGui::BeginGroup();
		{
			UI::PushID("##ClearColorsModification");
			ImGui::Text("Background"); 
			ImGui::SliderFloat("##x", &Renderer::ClearColor.x, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##y", &Renderer::ClearColor.y, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##z", &Renderer::ClearColor.z, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##w", &Renderer::ClearColor.w, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("UI Alpha", &colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f, " %.2f", backgroundSliderFlags);
			UI::PopID();
			//UI::PopID("##ClearColors");
		}
		ImGui::EndGroup();
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

	glm::vec2 Editor::GetLeftSidebarSize() const 
	{ 
		return LeftSidebarSize; 
	}

	glm::vec2 Editor::GetRightSidebarSize() const
	{ 
		return RightSidebarSize; 
	}

	glm::vec2 Editor::GetBottomBarSize() const
	{
		return BottomBarSize;
	}

	float Editor::GetViewportScalerX() const 
	{ 
		return ViewportScalers.x; 
	}

	float Editor::GetViewportScalerY() const 
	{ 
		return ViewportScalers.y; 
	}

	glm::vec2 Editor::GetMenuBarSize() const 
	{ 
		return MenuBarSize; 
	}

	glm::vec2 Editor::GetTabBarSize() const
	{
		return TabBarSize; 
	}

	void Editor::SetScene(Ref<Scene> scene)
	{
		m_Scene = scene;
		m_SceneManagerPanel->SetScene(scene);
		Application::Get()->SetScene(scene);
		scene->m_EditorCamera->SetActive(true);
	}

}
