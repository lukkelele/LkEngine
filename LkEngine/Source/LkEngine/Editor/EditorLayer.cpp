#include "LKpch.h"
#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Debug/DebugLayer.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Event/SceneEvent.h"
#include "LkEngine/Input/Mouse.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/UI/SceneManagerPanel.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer.h"
#include "EditorLayer.h"


namespace LkEngine {

	static bool bInit = false;

	static TObjectPtr<LTexture2D> FloorTexture;
	static TObjectPtr<LTexture2D> CubeTexture;

	LEditorLayer::LEditorLayer()
		: LLayer("EditorLayer")
		, TabManager(LEditorTabManager::Get())
	    , m_Scene(nullptr)
		, m_Enabled(true)
	{
		Instance = this;

		m_ShowStackTool = false;
		CurrentWindowType = EEditorWindowType::Viewport;

		Window = &LApplication::Get()->GetWindow();
		LK_CORE_ASSERT(Window, "Window is nullptr");

		/* Viewport bounds. */
		ViewportBounds[0] = { 0, 0 };
		//ViewportBounds[1] = { Window->GetViewportWidth(), Window->GetViewportHeight() };
		ViewportBounds[1] = { Window->GetViewportWidth(), Window->GetViewportHeight() };

		SecondViewportBounds[0] = { 0, 0 };
		SecondViewportBounds[1] = { 0, 0 };

		LeftSidebarSize.y = ViewportBounds[1].Y;
		RightSidebarSize.y = ViewportBounds[1].Y;
		LastBottomBarSize = Utils::ConvertToImVec2(BottomBarSize);

		EditorWindowPos = { LeftSidebarSize.x, BottomBarSize.y };
		EditorWindowSize.x = ViewportBounds[1].X - LeftSidebarSize.x - RightSidebarSize.x;
		EditorWindowSize.y = ViewportBounds[1].Y - BottomBarSize.y /* - topbar_height */;

		ViewportScalers.x = EditorWindowSize.x / ViewportBounds[1].X;
		ViewportScalers.y = EditorWindowSize.y / ViewportBounds[1].Y;

		GOnObjectCreated.Add([&](const LObject* NewObject)
		{
			LK_CORE_DEBUG_TAG("Editor", "New Object Created: {}, Name=\"{}\"", NewObject->StaticClass(), NewObject->GetName());
		});

		/* Editor UI components. */
		SceneManagerPanel = MakeShared<LSceneManagerPanel>();
		ContentBrowser = MakeUnique<LContentBrowser>();
		ComponentEditor = MakeUnique<LComponentEditor>();

		LEditorLayer& Editor = *this;
		auto EventCallback = [&Editor](LEvent& Event) { Editor.OnEvent(Event); };
		LK_CORE_TRACE_TAG("Editor", "Setting event callback to OnEvent");
		SetEventCallback(EventCallback);

		Window->OnWindowSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			LK_CORE_DEBUG_TAG("Editor", "Window size updated to ({}, {})", NewWidth, NewHeight);
			SetUpdateWindowFlag(true);
		});
	}

	void LEditorLayer::Initialize()
	{
		LObject::Initialize();
		LK_ASSERT(bInit == false, "EditorLayer already initialized");

		LK_CORE_DEBUG_TAG("Editor", "Initializing layer");
	#if 0 /// DISABLED, NEEDS TO BE RE-EVALUATED
		Window->SetScalers(ViewportScalers.x, ViewportScalers.y);
		Window->SetWidth(static_cast<uint32_t>(EditorWindowSize.x));
		Window->SetHeight(static_cast<uint32_t>(EditorWindowSize.y));
	#endif
		/* Attach to LWindow delegates. */
		LK_CORE_ASSERT(Window, "Window reference is nullptr");
		if (Window)
		{
			FWindowData& WindowData = Window->GetWindowData();
			LK_CORE_DEBUG_TAG("Editor", "Attaching to window delegates");

			/* Mouse button pressed. */
			WindowData.OnMouseButtonPressed.Add([&](const FMouseButtonData& MouseButtonData)
			{
				LK_CORE_DEBUG_TAG("Editor", "MouseButtonPressed: {}", static_cast<int>(MouseButtonData.Button));
			});

			/* Mouse button released. */
			WindowData.OnMouseButtonReleased.Add([&](const FMouseButtonData& MouseButtonData)
			{
				LK_CORE_DEBUG_TAG("Editor", "MouseButtonReleased: {}", static_cast<int>(MouseButtonData.Button));
			});

			/* Mouse scroll. */
			WindowData.OnMouseScrolled.Add([&](const EMouseScroll MouseScroll)
			{
				LK_CORE_DEBUG_TAG("Editor", "Mouse Scroll: {}", Enum::ToString(MouseScroll));
				if (EditorCamera)
				{
					if (MouseScroll == EMouseScroll::Up)
					{
						EditorCamera->MouseZoom(0.01f);
					}
					else if (MouseScroll == EMouseScroll::Down)
					{
						EditorCamera->MouseZoom(-0.01f);
					}
				}
			});

		}

        /* Viewport Framebuffer. */
		LK_CORE_TRACE_TAG("Editor", "Creating viewport framebuffer");
		FFramebufferSpecification FramebufferSpec;
		FramebufferSpec.Attachments = { 
			EImageFormat::RGBA32F, 
			EImageFormat::DEPTH24STENCIL8 
		};
		FramebufferSpec.Samples = 1;
		FramebufferSpec.ClearColorOnLoad = false;
		FramebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		FramebufferSpec.DebugName = "EditorLayer-Framebuffer";
        FramebufferSpec.Width = LWindow::Get().GetWidth();
        FramebufferSpec.Height = LWindow::Get().GetHeight();
		m_ViewportFramebuffer = LFramebuffer::Create(FramebufferSpec);

		/* Editor Camera. */
		EditorCamera = TObjectPtr<LEditorCamera>::Create(60.0f,                /* FOV    */
														 Window->GetWidth(),   /* Width  */
														 Window->GetHeight(),  /* Height */
														 0.10f,                /* ZNear  */
														 2400.0f);             /* ZFar   */

		/// FIXME:
		EditorCamera->SetPosition({ -10, 4, -10 });
		EditorCamera->m_Pitch = 0.0f;
		EditorCamera->m_Yaw = glm::pi<float>();

		/* UI components. */
		SceneManagerPanel->Initialize();
		ContentBrowser->Initialize();
		ComponentEditor->Initialize();

		// TODO: Parse config or cache to determine most recent project and go from there
		// New project and/or setting the project to active triggers an event that 
		// handles the redirection of input and physics 
		m_Project = LProject::CreateEmptyProject("Editor", true);

		m_ViewportFramebuffer->Bind();

		/* Bind delegate for GEditorOnSelectionChanged. */
		auto OnSelectionChanged = [&](const LObject& Object)
		{
			LK_CORE_DEBUG_TAG("Editor", "OnSelectionChanged Callback: {}, ReferenceCount: {}", 
							  Object.GetName(), Object.GetReferenceCount());
		};
		GEditorOnSelectionChanged.Add(OnSelectionChanged);

		bInit = true;
	}

	void LEditorLayer::OnUpdate(const float DeltaTime)
	{
		// The window space is calculated from topleft corner, so remove LMouse::Pos.y to get the actual cursor placement
		{
			/// MOVE THIS
			LMouse::Pos = LMouse::GetRawPos();
			LMouse::Pos.x -= LeftSidebarSize.x;
			LMouse::Pos.y = ViewportBounds[1].Y - BottomBarSize.y - LMouse::Pos.y;

			LMouse::ScaledPos.x = (LMouse::Pos.x) / ViewportScalers.x;
			LMouse::ScaledPos.y = (LMouse::Pos.y) / ViewportScalers.y;

			LMouse::CenterPos.x = (LMouse::Pos.x / Window->GetWidth()) * 2.0f - 1.0f;
			LMouse::CenterPos.y = ((LMouse::Pos.y / Window->GetHeight()) * 2.0f - 1.0f) * -1.0f;
		}

		if (EditorCamera)
		{
			EditorCamera->SetViewportSize(EditorWindowSize.x, EditorWindowSize.y);
			EditorCamera->OnUpdate(DeltaTime);
		}

		RenderViewport();
	}

	void LEditorLayer::OnRender()
	{
		RenderViewport();
	}

	void LEditorLayer::OnImGuiRender()
	{
		LApplication* Application = LApplication::Get();
		auto& window = Window;
		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		ImGuiViewportP* Viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		ViewportScalers.x = EditorWindowSize.x / ViewportBounds[1].X;
		ViewportScalers.y = EditorWindowSize.y / ViewportBounds[1].Y;

#if 0
		if (m_Scene && SELECTION::SelectedEntity != SELECTION::NULL_ENTITY)
		{
			SELECTION::SelectedEntity = m_Scene->TryGetEntityWithUUID(SELECTION::SelectedEntity.GetUUID());
		}
#endif

		UI::BeginViewport(UI_CORE_VIEWPORT, Window, Viewport);
		UI_HandleManualWindowResize();
		UI::BeginDockSpace(LkEngine_DockSpace);

		UI_MainMenuBar();

		/* Left Sidebar. */
		const ImGuiID DockspaceID_LeftSidebar = ImGui::GetID("Dockspace_LeftSidebar");
		static bool ResetDockspace_LeftSidebar = true;

		CheckLeftSidebarSize();
		ImGui::Begin(UI_SIDEBAR_LEFT, nullptr, UI::SidebarFlags);
		{
	#if LK_UI_ENABLE_LEFT_SIDEBAR_CONTENT

			/* Blending */
			static bool bBlendingEnabled = false;
			TObjectPtr<LRenderContext> RenderContext = LWindow::Get().GetRenderContext();
			bBlendingEnabled = RenderContext->GetBlendingEnabled();
			if (ImGui::Checkbox("Depth Testing", &bBlendingEnabled))
			{
				Window->SetDepthEnabled(bBlendingEnabled);
			}

			SceneManagerPanel->UI_CameraSettings();

			/* Retrieve the cached textures. */
		#if LK_UI_RENDER_GEOMETRIC_SHAPES_MENU
			const std::vector<TTexture2DPair>& textures2D = LAssetManager::GetTextures2D();
			if (CubeTexture)
			{
				std::string_view TextureName = CubeTexture->GetName();
				if (ImGui::BeginCombo("Cubes", TextureName.data(), ImGuiComboFlags_HeightLargest | ImGuiComboFlags_NoPreview))
				{
					for (const std::pair<std::string, TObjectPtr<LTexture2D>>& TextureEntry : textures2D)
					{
						if (ImGui::Selectable(TextureEntry.first.c_str()))
						{
							CubeTexture = TextureEntry.second;
						}
					}

					ImGui::EndCombo(); // Cubes
				}

				if (ImGui::BeginCombo("Floor", TextureName.data(), ImGuiComboFlags_HeightLargest | ImGuiComboFlags_NoPreview))
				{
					for (auto& tex : textures2D)
					{
						if (ImGui::Selectable(tex.first.c_str()))
						{
							//mc.SetTexture(tex.second);
							PlaneTexture = tex.second;
						}
					}

					ImGui::EndCombo(); // Floor
				}
			}
		#endif


			/* COLORS */
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Colors"))
			{
				UI_ClearColorModificationMenu();

				ImGui::TreePop(); /* Colors. */
			}

		#if LK_UI_USE_EDITORLAYER_CAMERA
			//----------------------------------------------------
			// EditorLayer Camera
			//----------------------------------------------------
			ImGui::Text("EditorLayer Cam Mode: %s", (EditorCamera->m_CameraMode == LEditorCamera::Mode::Arcball ? "Arcball" : "Flycam"));
			static const char* cameraViewTypes[] = { "Perspective", "Orthographic" };
			int selectedEditorCameraViewTypeIndex = (int)EditorCamera->GetProjectionType();
			ImGui::Text("Selected View Type Index: %d", selectedEditorCameraViewTypeIndex);
			// Get current camera view mode and set it to that if at init stage
			if (ImGui::BeginCombo("EditorLayer Camera Type", cameraViewTypes[selectedEditorCameraViewTypeIndex]))
			{
				int CurrentIndex = 0;
				for (const char*& viewType : cameraViewTypes)
				{
					const bool isSelected = selectedEditorCameraViewTypeIndex == CurrentIndex;
					if (ImGui::Selectable(cameraViewTypes[CurrentIndex], isSelected, ImGuiSelectableFlags_SpanAvailWidth))
					{
						selectedEditorCameraViewTypeIndex = CurrentIndex;
						if (viewType == "Perspective")
						{
							EditorCamera->SetProjectionType(LCamera::ProjectionType::Perspective);
						}
						else if (viewType == "Orthographic")
						{
							EditorCamera->SetProjectionType(LCamera::ProjectionType::Orthographic);
						}
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}

					CurrentIndex++;
				}

				ImGui::EndCombo();
			}
			ImGui::BeginGroup();
			{
				UI::Property::PositionXYZ(EditorCamera->m_Position);
				ImGui::SliderFloat("Pitch", &EditorCamera->m_Pitch, -12.0f, 12.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("Yaw", &EditorCamera->m_Yaw, -12.0f, 12.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			}
			ImGui::EndGroup();
		#endif

			/* DISABLE FOR NOW */
	#if LK_UI_USE_MODE_SELECTOR
			//----------------------------------------------------
			// Mode Selector
			//----------------------------------------------------
			ImGui::BeginGroup();
			{
				static ImVec2 modeButtonSize = { 50.0f, 50.0f };
				static ImVec4 modeButtonBgColor = { 0, 0, 0, 0 };
				static ImVec4 modeButtonTintColor = { 1, 1, 1, 1 };
				constexpr const char* TextureName = "ale1024";
				if (ImGui::ImageButton("##ModeButton-NormalMode", 
					(void*)LTextureLibrary::GetTexture(TextureName)->GetRendererID(), 
					modeButtonSize, 
					ImVec2(1, 1), 
					ImVec2(0, 0), 
					modeButtonBgColor, 
					modeButtonTintColor))
				{
					LK_CORE_DEBUG("Push tab");
					//TabManager::NewTab(
					TabManager.NewTab(
						fmt::format("Node EditorLayer-{}", TabManager.GetTabCount()), 
						EditorTabType::NodeEditor
					);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton("##ModeButton-NodeEditorLayer", 
					(void*)LTextureLibrary::GetTexture(TextureName)->GetRendererID(), 
					modeButtonSize, 
					ImVec2(1, 1), 
					ImVec2(0, 0), 
					modeButtonBgColor, 
					modeButtonTintColor))
				{
					if (TabManager.GetTabCount() > 1)
					{
						TabManager.PopTab();
					}
				}
			}
			ImGui::EndGroup();
	#endif

	#if LK_USE_CREATOR_MENU
			//----------------------------------------------------
			// Creator menu
			//----------------------------------------------------
			ImGui::BeginGroup();
			{
				UI_CreateMenu();
			}
			ImGui::EndGroup();
	#endif

			SceneManagerPanel->OnRenderUI();

			const ImVec2 WindowSize = ImGui::GetWindowSize();
			const ImVec2 windowPos = ImGui::GetWindowPos();
			if (WindowSize.x != LastSidebarLeftSize.x || WindowSize.y != LastSidebarLeftSize.y)
			{
				//bWindowsHaveChangedInSize = true;
			}

			LastSidebarLeftPos = windowPos;
			LastSidebarLeftSize = WindowSize;
	#endif
		}
		ImGui::End(); /* Left Sidebar. */


		//---------------------------------------------------------
		// Right Sidebar
		//---------------------------------------------------------
		CheckRightSidebarSize();
		ImGui::Begin(UI_SIDEBAR_RIGHT, nullptr, UI::SidebarFlags);
		{
			ComponentEditor->OnRenderUI();

			/* Window Information. */
			if (ImGui::TreeNode("Window Information"))
			{
				UI_ShowViewportAndWindowDetails();

				ImGui::TreePop(); /* ~Window Information */
			}

			/* Mouse Information. */
			if (ImGui::TreeNode("Mouse Information"))
			{
				UI_ShowMouseDetails();
			}

			const ImVec2 WindowSize = ImGui::GetWindowSize();
			if (WindowSize.x != RightSidebarSize.x || WindowSize.y != RightSidebarSize.y)
			{
				//bWindowsHaveChangedInSize = true;
			}
			LastSidebarRightPos = ImGui::GetWindowPos();
			LastSidebarRightSize = WindowSize;

			ImGui::Separator();

		#if 0 
			// Testing: Create new cube in scene
			if (ImGui::Button("Create Cube"))
			{
				CreateCube();
			}
		#endif
		}
		ImGui::End(); // UI_SIDEBAR_RIGHT

		// Draw component below the content in the right sidebar
	#if 0
		LSceneManagerPanel::DrawComponents(SelectionContext::SelectedEntity);
	#endif

		/* Main Window. */
		UI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		UI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		UI::Begin(UI_CORE_VIEWPORT, UI::CoreViewportFlags);
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x, MenuBarSize.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(EditorWindowSize.x, Viewport->Size.y - (BottomBarSize.y + MenuBarSize.y)), ImGuiCond_Always);
			UI::Begin(UI::VIEWPORT_TEXTURE, UI::ViewportTextureFlags);
			{
				UI_ViewportTexture(); // Actual scene image

				if (GSelectedObject)
				{
					DrawObjectGizmo(GSelectedObject);
				}

				/* Statistics, FPS counter... */
				UI_WindowStatistics();
			}
			UI::End(); // UI::VIEWPORT_TEXTURE
		}
		UI::End();
		UI::PopStyleVar(2);


		//---------------------------------------------------------
		// Bottom Bar
		//---------------------------------------------------------
		CheckBottomBarSize();
		ImGui::Begin(UI_BOTTOM_BAR, nullptr, UI::SidebarFlags);
		{
			ContentBrowser->OnRenderUI();

			const ImVec2 WindowSize = ImGui::GetWindowSize();
			if (WindowSize.x != BottomBarSize.x || WindowSize.y != BottomBarSize.y)
			{
				bWindowsHaveChangedInSize = true;
			}
			LastBottomBarPos = ImGui::GetWindowPos();
			LastBottomBarSize = WindowSize;

		}
		ImGui::End();

		/* Render tabs. */
		UI_TabManager();

		// If the window sizes have been adjusted, set the member to false.
		// This must be run BEFORE the 'bWindowsHaveChangedInSize'.
		if (ShouldUpdateWindowSizes)
		{
			ShouldUpdateWindowSizes = false; 
		}

		glm::vec2 ViewportSize = { Viewport->WorkSize.x, Viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(ViewportSize);

		// FIXME: What does this even do?...
		// Take care of tabs here.
		TabManager.End();

		ImGui::End(); // Viewport
		HandleExternalWindows();

		ImGui::End(); // Core Viewport
	}

	void LEditorLayer::RenderViewport()
	{
	#if 0 
		LRenderer::Submit([&]()
		{
			RenderMirrorTexture(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			RenderCubes(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			RenderFloor(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
		});
	#endif
	}

	void LEditorLayer::RenderViewport(TObjectPtr<LImage> image)
	{
	}

	void LEditorLayer::UI_ViewportTexture()
	{
		TObjectPtr<LImage2D> ViewportImage = m_ViewportFramebuffer->GetImage(0);

		UI::Image(ViewportImage, ImVec2(EditorWindowSize.x - 2, EditorWindowSize.y + MenuBarSize.y), ImVec2(0, 1), ImVec2(1, 0));

		// Drag'n'drop feature, used to drag items from content browser to the scene
		if (ImGui::BeginDragDropTarget())
		{
			// Accept Content Browser payloads
		    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CB_PAYLOAD", ImGuiDragDropFlags_None);
			if (payload)
		    {
				// Payload data is sent in string format
				std::string data = std::string(static_cast<const char*>(payload->Data));

				if (data == "CUBE")
				{
					CreateCube();
				}
				else if (data == "CIRCLE")
				{
				}
				else if (data == "CYLINDER")
				{
				}

		    }
		 	ImGui::EndDragDropTarget();
		}
	}

	void LEditorLayer::HandleExternalWindows()
	{
		if (ShowRenderSettingsWindow)
		{
			UI_RenderSettingsWindow();
		}

		if (bShowEditorWindowSizesWindow)
		{
			ImGui::Begin("EditorLayer Window Sizes", &bShowEditorWindowSizesWindow);
			UI_ShowEditorWindowsDetails();
			ImGui::End();
		}
	}

	void LEditorLayer::RegisterEvent(LEvent& e)
	{
		m_EventCallback(e);
	}

	void LEditorLayer::OnEvent(LEvent& Event)
	{
		switch (Event.GetEventType())
		{
			case EEventType::MouseButtonPressed:
			{
				LMouseButtonPressedEvent& MouseButtonPressedEvent = static_cast<LMouseButtonPressedEvent&>(Event);

				if (TObjectPtr<LEditorCamera> EditorCamera = GetEditorCamera())
				{
					LWindow& Window = LWindow::Get();

					const glm::vec2 ScreenCoordinates = MouseButtonPressedEvent.GetCoordinates();
					//const float Depth = 0.50f;
					const float Depth = GLUtils::SampleDepth(0, 0, Window.GetWidth(), Window.GetHeight());
					const glm::vec4 Viewport = { 0, 0, LWindow::Get().GetWidth(), LWindow::Get().GetHeight() };
					const glm::vec2 WorldCoordinates = Math::ConvertScreenToWorldCoordinates(ScreenCoordinates, 
																							 Depth, 
																							 EditorCamera->GetViewMatrix(), 
																							 EditorCamera->GetProjectionMatrix(), 
																							 Viewport);
					LK_CORE_DEBUG_TAG("Editor", "{}  World Coordinates: ({}, {})", 
									  MouseButtonPressedEvent.ToString(), WorldCoordinates.x, WorldCoordinates.y);
				}

				break;
			}

			case EEventType::MouseButtonReleased:
			{
				break;
			}

			case EEventType::MouseScrolled:
			{
				Event.Handled = EditorCamera->OnMouseScroll(static_cast<MouseScrolledEvent&>(Event));
				break;
			}

			case EEventType::KeyPressed:
			{
				Event.Handled = EditorCamera->OnKeyPress(static_cast<KeyPressedEvent&>(Event));
				break;
			}

			case EEventType::KeyReleased:
			{
				break;
			}

			case EEventType::SceneCreated:
			{
				LK_CORE_DEBUG_TAG("Editor", "Handling SceneCreated event");
				SceneCreatedEvent& sceneEvent = static_cast<SceneCreatedEvent&>(Event);

				TObjectPtr<LScene> newScene = sceneEvent.GetScene();
				m_Scene = newScene;
				SceneManagerPanel->SetScene(newScene);
				newScene->EditorCamera->SetActive(true);

				break;
			}
		}
	}

	/// FIXME:
	void LEditorLayer::DrawObjectGizmo(const TObjectPtr<LObject>& InObject)
    {
		if (InObject->IsA<LEntity>())
		{
			LEntity Entity = InObject->As<LEntity>();
			//LEntity Entity = const_cast<TObjectPtr<LObject>&>(InObject)->As<LEntity>();

			LTransformComponent& TransformComponent = Entity.Transform();
			glm::mat4 TransformMatrix = TransformComponent.GetTransform();
			const glm::vec3& CameraPos = EditorCamera->GetPosition();
			const glm::mat4& ViewMatrix = EditorCamera->GetViewMatrix();
			const glm::mat4& ProjectionMatrix = EditorCamera->GetProjectionMatrix();

			const float PosX = SecondViewportBounds[0].X;
			const float PosY = SecondViewportBounds[0].Y;
			//const float Width  = SecondViewportBounds[1].X - SecondViewportBounds[0].X;
			//const float Height = SecondViewportBounds[1].Y - SecondViewportBounds[0].Y;

			ImGuiWindow* Window = ImGui::FindWindowByName(UI::VIEWPORT_TEXTURE);
			ImGui::Begin(Window->Name, nullptr, UI::CoreViewportFlags | ImGuiWindowFlags_NoScrollbar);
			{
				ImGuizmo::SetOrthographic(static_cast<int>(EditorCamera->GetProjectionType()));
				ImGuizmo::SetDrawlist();

				if (TabManager.GetTabCount() == 1)
				{
					ImGuizmo::SetRect(PosX, (PosY - BottomBarSize.y + MenuBarSize.y), 
									  EditorWindowSize.x, EditorWindowSize.y);
				}
				else
				{
					ImGuizmo::SetRect(PosX, (PosY - BottomBarSize.y + MenuBarSize.y + TabBarSize.y), 
									  EditorWindowSize.x, EditorWindowSize.y);
				}

				ImGuizmo::Manipulate(
					glm::value_ptr(ViewMatrix),
					glm::value_ptr(ProjectionMatrix),
					static_cast<ImGuizmo::OPERATION>(EditorCamera->GetGizmoMode()),
					ImGuizmo::LOCAL, 
					glm::value_ptr(TransformMatrix)
				);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 Translation;
					glm::vec3 Scale;
					glm::quat Rotation;
					Math::DecomposeTransform(TransformMatrix, Translation, Rotation, Scale);

					TransformComponent.Translation = Translation;
					TransformComponent.Scale = Scale;
					TransformComponent.SetRotation(Rotation);
				}
			}
			ImGui::End(); // Window->Name
		}
    }

	// TODO: Right alignment in the child window
	void LEditorLayer::UI_WindowStatistics()
	{
		//---------------------------------------------------------
		// Window statistics, FPS counter etc.
		//---------------------------------------------------------
		static ImVec2 StatisticsWindowSize = ImVec2(ImGui::CalcTextSize("Forward Direction: { N1, N2, N3 }").x + 120, 500);

		/* No tabs are present. */
		if (TabManager.GetTabCount() == 1)
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x + EditorWindowSize.x - StatisticsWindowSize.x * 1.0f, MenuBarSize.y), 
				ImGuiCond_Always
			);
		}
		/* Multiple tabs. */
		else
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x + EditorWindowSize.x - StatisticsWindowSize.x, MenuBarSize.y + TabBarSize.y), 
									ImGuiCond_Always);
		}
		ImGui::BeginChild("##WindowStats", StatisticsWindowSize, false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
		{
			const float FPS = 1000.0f / LApplication::Get()->GetTimestep();
			ImGui::Text("FPS: %1.f", FPS);

			if (EditorCamera->bIsActive)
			{
				ImGui::Text("FOV: %1.f", EditorCamera->m_DegPerspectiveFOV);
				const glm::vec3& camPos = EditorCamera->GetPosition();

				ImGui::Text("Pos (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
				ImGui::Text("Camera Zoom: %.3f", EditorCamera->ZoomSpeed());
				ImGui::Text("Speed: %.3f", EditorCamera->GetCameraSpeed());
				ImGui::Text("Forward Direction: %s", EditorCamera->GetForwardDirection<LVector>().ToString().CStr());

				ImGui::Text("Distance: %.2f", EditorCamera->GetDistance());
				ImGui::Text("Focalpoint: (%.2f, %.2f, %.2f)", EditorCamera->GetFocalPoint().x, EditorCamera->GetFocalPoint().y, EditorCamera->GetFocalPoint().z);

				// Mouse data.
				ImGui::Text("Mouse Button: %s", Enum::ToString(LInput::GetLastMouseButton()));
			}
		}
		ImGui::EndChild();
	}

	void LEditorLayer::UI_HandleManualWindowResize()
	{
		auto window = LApplication::Get()->GetWindow().GetGlfwWindow();
		const bool maximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
		ImVec2 newSize, newPosition;
		if (!maximized && UI::UpdateWindowManualResize(ImGui::GetCurrentWindow(), newSize, newPosition))
		{
			glfwSetWindowPos(window, 
							 static_cast<int>(newPosition.x), 
							 static_cast<int>(newPosition.y));
			glfwSetWindowSize(window, 
							  static_cast<int>(newSize.x), 
							  static_cast<int>(newSize.y));
		}
	}

	void LEditorLayer::UI_SceneContent()
	{
		static constexpr const char* UI_RenderID = "##lkengine-scene-content";

		const float menu_height = Window->GetHeight() - SelectedEntityMenuSize.y;

		UI::PushID(UI_RenderID);
		ImGui::BeginGroup();
		ImGui::SeparatorText("Scene");

        static ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;

		auto entities = m_Scene->m_Registry.view<LTransformComponent>();
		ImGui::Text("Entities in scene: %d", entities.size());
        for (const entt::entity& ent : entities)
        {
            LEntity entity = { ent, m_Scene };

			bool bIsSelected = false;
            std::string label = fmt::format("{}", entity.Name());
            if (ImGui::Selectable(label.c_str(), &bIsSelected, selectable_flags))
            {
				//SELECTION::SelectedEntityID = entity.GetComponent<IDComponent>().ID;
            }
        }
		ImGui::EndGroup();

		UI::PopID(UI_RenderID);
	}

	glm::vec2 LEditorLayer::GetEditorWindowSize() const
	{
		glm::vec2 size = { 
			SecondViewportBounds[1].X - SecondViewportBounds[0].X, 
			SecondViewportBounds[1].Y - SecondViewportBounds[0].Y 
		};

		return size;
	}

	void LEditorLayer::UI_RenderSettingsWindow()
	{
		if (ImGui::Begin("Render Settings", &ShowRenderSettingsWindow, ImGuiWindowFlags_NoDocking))
		{
			TObjectPtr<LRenderContext> RenderContext = LWindow::Get().GetRenderContext();

			bool bBlendingEnabled = RenderContext->GetBlendingEnabled();
			if (ImGui::Checkbox("Blending", &bBlendingEnabled))
			{
				if (bBlendingEnabled)
				{
					RenderContext->SetBlendingEnabled(true);
				}
				else
				{
					RenderContext->SetBlendingEnabled(false);
				}
			}

			const std::string CurrentRenderTopology = Enum::ToString(LRenderer::GetPrimitiveTopology());
			if (ImGui::BeginCombo("Topology", CurrentRenderTopology.c_str(), NULL))
			{
				if (ImGui::MenuItem("Triangles"))
				{
					LK_CORE_DEBUG("Selected new drawmode 'Triangles'");
					LRenderer::SetPrimitiveTopology(ERenderTopology::Triangles);
				}
				if (ImGui::MenuItem("Lines"))
				{
					LK_CORE_DEBUG("Selected new drawmode 'Lines'");
					LRenderer::SetPrimitiveTopology(ERenderTopology::Lines);
				}

				ImGui::EndCombo();
			}

			ImGui::SeparatorText("Blend Function");
			if (ImGui::BeginCombo(fmt::format("Source: {}", RenderContext->GetCurrentSourceBlendFunctionName()).c_str(), 
				                  nullptr, ImGuiComboFlags_NoPreview))
			{
				if (ImGui::MenuItem("Zero"))
				{
					RenderContext->SetSourceBlendFunction(ESourceBlendFunction::Zero);
				}
				else if (ImGui::MenuItem("One"))
				{
					RenderContext->SetSourceBlendFunction(ESourceBlendFunction::One);
				}
				else if (ImGui::MenuItem("Alpha"))
				{
					RenderContext->SetSourceBlendFunction(ESourceBlendFunction::Alpha);
				}
				else if (ImGui::MenuItem("Color"))
				{
					RenderContext->SetSourceBlendFunction(ESourceBlendFunction::Color);
				}
				else if (ImGui::MenuItem("One_Minus_DestinationAlpha"))
				{
					RenderContext->SetSourceBlendFunction(ESourceBlendFunction::One_Minus_DestinationAlpha);
				}
				ImGui::EndCombo();
			}

			/* Destination Blend. */
			if (ImGui::BeginCombo(
					fmt::format("Destination: {}", RenderContext->GetCurrentDestinationBlendFunctionName()).c_str(), 
					nullptr, ImGuiComboFlags_NoPreview)
				)
			{
				if (ImGui::MenuItem("Zero"))
				{
					RenderContext->SetDestinationBlendFunction(EDestinationBlendFunction::Zero);
				}
				else if (ImGui::MenuItem("One"))
				{
					RenderContext->SetDestinationBlendFunction(EDestinationBlendFunction::One);
				}
				else if (ImGui::MenuItem("Alpha"))
				{
					RenderContext->SetDestinationBlendFunction(EDestinationBlendFunction::Alpha);
				}
				else if (ImGui::MenuItem("Color"))
				{
					RenderContext->SetDestinationBlendFunction(EDestinationBlendFunction::Color);
				}
				else if (ImGui::MenuItem("One_Minus_SourceAlpha"))
				{
					RenderContext->SetDestinationBlendFunction(EDestinationBlendFunction::One_Minus_SourceAlpha);
				}

				ImGui::EndCombo(); /* Destination Blend. */
			}

			ImGui::End();
		}
	}

	float LEditorLayer::GetEditorWindowWidth() const
	{
		auto WindowSize = GetEditorWindowSize();
		return WindowSize.x;
	}

	float LEditorLayer::GetEditorWindowHeight() const
	{
		auto WindowSize = GetEditorWindowSize();
		return WindowSize.y;
	}

	void LEditorLayer::UI_CreateMenu()
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
				const bool bIsSelected = (geometricShapeCurrentIndex == n);
				if (ImGui::Selectable(geometricShapes[n], bIsSelected))
				{
					geometricShapeCurrentIndex = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (bIsSelected)
				{
					ImGui::SetItemDefaultFocus();
					LK_CORE_DEBUG("COMBO: Selected Item -> {}", geometricShapes[geometricShapeCurrentIndex]);
				}
			}
			ImGui::EndCombo(); /* CreateMenu */
		}

		// Selectable geometric shapes
		// Can be clicked on to select diffent shapes instead of dropdown menu
		ImGui::BeginGroup();
		{
			static const ImVec4 tintColor = ImVec4(1, 1, 0.90, 1);
			static const ImVec4 bgColor = ImVec4(0, 0, 0, 0);
			static const ImVec2 imageSize = ImVec2(60, 60);

			static std::string TextureName = "ale1024";

			/* Rectangular Image. */
			TObjectPtr<LTexture> RectangleTexture = LTextureLibrary::Get().GetTexture(TextureName);
			if (ImGui::ImageButton(
					"##RectangleImage", 
					reinterpret_cast<ImTextureID>(RectangleTexture->GetRendererID()),
					imageSize, 
					ImVec2(1, 1), 
					ImVec2(0, 0), 
					bgColor, 
					tintColor)
				)
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
				ImGui::SetDragDropPayload("FOLDER_DATA_TYPE", "Wow", sizeof("Wow"));

				ImGui::EndDragDropSource();
			}

			ImGui::SameLine(0, 2);

			// Circle Image
			if (ImGui::ImageButton(
					"##CircleImage", 
					reinterpret_cast<ImTextureID>(LTextureLibrary::Get().GetTexture(TextureName)->GetRendererID()),
					imageSize, 
					ImVec2(1, 1), 
					ImVec2(0, 0), 
					bgColor, 
					tintColor)
				)
			{
				LK_CORE_TRACE_TAG("Editor", "Clicked CircleImage, selecting circle shape");
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
			if (ImGui::ImageButton(
					"##TriangleImage", 
					reinterpret_cast<ImTextureID>(LTextureLibrary::Get().GetTexture(TextureName)->GetRendererID()),
					imageSize, 
					ImVec2(1, 1), 
					ImVec2(0, 0), 
					bgColor, 
					tintColor)
				)
			{
				LK_CORE_TRACE_TAG("Editor", "Clicked TriangleImage, selecting triangle shape");
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

		static constexpr uint32_t shapeSizeColumnSize = 50;
		if (geometricShapes[geometricShapeCurrentIndex] == "Rectangle")
		{
			constexpr unsigned int rectangle_ColumnPadding = 60;
			currentGeometricShape = Math::Shape::Rectangle;
			static float rectangle_Width = 1.0f;
			static float rectangle_Height = 1.0f;

			/* Width. */
			ImGui::Text("Width");
			ImGui::SameLine(0, rectangle_ColumnPadding - ImGui::CalcTextSize("Width").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Width", &rectangle_Width, 0.10f ,0.010f, 0.0f, "%.2f");

			/* Height. */
			ImGui::Text("Height");
			ImGui::SameLine(0, rectangle_ColumnPadding - ImGui::CalcTextSize("Height").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Height", &rectangle_Height, 0.10f ,0.010f, 0.0f, "%.2f");
		}
		/* Shape: Circle */
		else if (geometricShapes[geometricShapeCurrentIndex] == "Circle")
		{
			static constexpr unsigned int circle_ColumnPadding = 84;
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
		/* Shape: Triangle */
		else if (geometricShapes[geometricShapeCurrentIndex] == "Triangle")
		{
			currentGeometricShape = Math::Shape::Triangle;
		}
		
		if (ImGui::Button("Add"))
		{
			/* Gather characteristics for creating object. */
			LK_CORE_TRACE("Creating: {}", nameInputBuffer);
			LK_CORE_WARN("NOT IMPLEMENTED");
		}

		UI::PopID();
	}

	void LEditorLayer::SetUpdateWindowFlag(const bool flag) 
	{ 
		// Window width and height has been changed as this function has been called,
		// therefore need to update the Viewport bounds

		ShouldUpdateWindowSizes = flag; 

		ViewportBounds[0] = { 0, 0 };
		ViewportBounds[1] = { Window->GetViewportWidth(), Window->GetViewportHeight()};
	}

	void LEditorLayer::UI_ShowViewportAndWindowDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("Window Info");
			ImGui::Text("Second Viewport Bounds[0]: (%1.f, %1.f)", SecondViewportBounds[0].X, SecondViewportBounds[0].Y);
			ImGui::Text("Second Viewport Bounds[1]: (%1.f, %1.f)", SecondViewportBounds[1].X, SecondViewportBounds[1].Y);
			ImGui::Text("Second Viewport Size: (%1.f, %1.f)", 
				SecondViewportBounds[1].X - SecondViewportBounds[0].X, 
				SecondViewportBounds[1].Y - SecondViewportBounds[0].Y
			);
			ImGui::Text("Window Size: (%1.f, %1.f)", (float)Window->GetWidth(), (float)Window->GetHeight());

			ImGui::Text("Viewport Window Size: (%1.f, %1.f)", 
						ViewportBounds[1].X - ViewportBounds[0].X, 
						ViewportBounds[1].Y - ViewportBounds[0].Y);

			ImGui::Text("EditorLayer Window Size: (%1.f, %1.f)", EditorWindowSize.x, EditorWindowSize.y);
			ImGui::Text("Center Window (%1.f, %1.f)", EditorWindowSize.x, EditorWindowSize.y);
			ImGui::SameLine(0, 20);
			ImGui::Text("Scaled res (%.1f, %.1f)", EditorWindowSize.x / ViewportScalers.x, EditorWindowSize.y / ViewportScalers.y);
			ImGui::Text("Centered window pos (%1.f, %1.f) - (%1.f, %1.f)", SecondViewportBounds[0].X, SecondViewportBounds[0].Y, SecondViewportBounds[1].X, SecondViewportBounds[1].Y);

			if (EditorCamera->bIsActive)
			{
				const glm::vec2 CameraPos = EditorCamera->GetPosition();
				ImGui::Text("EditorCamera Position: (%.1f, %.1f)", CameraPos.x, CameraPos.y);
			}

		}
		ImGui::EndGroup();
	}

	void LEditorLayer::UI_ShowMouseDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::SeparatorText("Mouse Info");
			ImGui::Text("Raw Pos (%1.f, %1.f)", LMouse::Pos.x, LMouse::Pos.y);
			ImGui::Text("Scaled Pos (%.1f, %.1f)", LMouse::ScaledPos.x, LMouse::ScaledPos.y);
			ImGui::Text("Center Normalized (%.2f, %.2f)", LMouse::CenterPos.x, LMouse::CenterPos.y);
			ImGui::Text("Center Scaled (%.2f, %.2f)", (LMouse::CenterPos.x * EditorWindowSize.x * 0.50f) / ViewportScalers.x, LMouse::CenterPos.y * EditorWindowSize.y * 0.50f / ViewportScalers.y) ;
			ImGui::Text("Mouse Scalers (%.2f, %.2f)", ViewportScalers.x, ViewportScalers.y);
			ImGui::Separator();
			ImGui::Text("Last Right Sidebar Size: (%1.f, %1.f)", LastSidebarRightSize.x, LastSidebarRightSize.y);
		}
		ImGui::EndGroup();
	}

	void LEditorLayer::UI_ShowEditorWindowsDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("MenuBarSize: (%1.f, %1.f)", MenuBarSize.x, MenuBarSize.y);
			ImGui::Text("TabBarSize: (%1.f, %1.f)", TabBarSize.x, TabBarSize.y);
			ImGui::Text("RightSidebarSize: (%1.f, %1.f)", RightSidebarSize.x, RightSidebarSize.y);
			ImGui::Text("BottomBarSize: (%1.f, %1.f)", BottomBarSize.x, BottomBarSize.y);
			ImGui::Text("Last Bottombar Size: (%1.f, %1.f)", LastBottomBarSize.x, LastBottomBarSize.y);
			ImGui::Text("Current Tab: %s", TabManager.GetActiveTabName().c_str());
			ImGui::Text("Tabs: %d", TabManager.GetTabCount());
		}
		ImGui::EndGroup();
	}

	void LEditorLayer::UI_SyncEditorWindowSizes(const glm::vec2& ViewportSize)
	{
		// Update editor window size
		EditorWindowSize.x = ViewportSize.x - (LeftSidebarSize.x + RightSidebarSize.x);
		EditorWindowSize.y = ViewportSize.y - (BottomBarSize.y + MenuBarSize.y);

		// Top right point/bound
		SecondViewportBounds[0] = { LeftSidebarSize.x, BottomBarSize.y };
		SecondViewportBounds[1] = { 
			(SecondViewportBounds[0].X + EditorWindowSize.x),
			(SecondViewportBounds[0].Y + EditorWindowSize.y)
		};

		// Check to see if any of the editor windows have changed in size and if they have
		// then readjust the Viewport
		if (bWindowsHaveChangedInSize)
		{
			LeftSidebarSize = { LastSidebarLeftSize.x, LastSidebarLeftSize.y };
			RightSidebarSize = { LastSidebarRightSize.x, LastSidebarRightSize.y };
			BottomBarSize = { LastBottomBarSize.x, LastBottomBarSize.y };

			EditorWindowPos = { LeftSidebarSize.x, BottomBarSize.y };
			EditorWindowSize.x = ViewportSize.x - LeftSidebarSize.x - RightSidebarSize.x;

			// Only take the size of the TabBar into account if any tabs exist
			if (TabManager.GetTabCount() > 1)
			{
				EditorWindowSize.y = ViewportSize.y - BottomBarSize.y;
			}
			else
			{
				EditorWindowSize.y = ViewportSize.y - BottomBarSize.y + TabBarSize.y;
			}

			EditorWindowPos.y -= MenuBarSize.y;

			// Update Viewport scalers as the resolution has been altered
			ViewportScalers.x = EditorWindowSize.x / ViewportBounds[1].X;
			ViewportScalers.y = EditorWindowSize.y / ViewportBounds[1].Y;

			Window->SetScalers(ViewportScalers);
			//Window->SetWidth(EditorWindowSize.x);
			//Window->SetHeight(EditorWindowSize.y);

			if (m_FillSidebarsVertically)
			{
				LeftSidebarSize.y = ViewportSize.y;
				RightSidebarSize.y = ViewportSize.y;
			}

			// Reapply Viewport settings starting from a lower point of the left sidebar and the bottom bar height
			LWindow::Get().GetRenderContext()->SetViewport(EditorWindowPos, EditorWindowSize);

			bWindowsHaveChangedInSize = false;
			ShouldUpdateWindowSizes = true; // Tell UI to set the window size ONCE
		}
	}

	void LEditorLayer::UI_ClearColorModificationMenu()
	{
		static ImGuiSliderFlags backgroundSliderFlags = ImGuiSliderFlags_None;
		auto& colors = ImGui::GetStyle().Colors;
		ImGui::BeginGroup();
		{
			UI::PushID("##ClearColorsModification");
			ImGui::Text("Background"); 
			ImGui::SliderFloat("##x", &LRenderer::ClearColor.x, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##y", &LRenderer::ClearColor.y, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##z", &LRenderer::ClearColor.z, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("##w", &LRenderer::ClearColor.w, 0.0f, 1.0f, " %.3f", backgroundSliderFlags);
			ImGui::SliderFloat("UI Alpha", &colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f, " %.2f", backgroundSliderFlags);
			UI::PopID();
		}
		ImGui::EndGroup();
	}

	void LEditorLayer::CheckLeftSidebarSize()
	{
		if (ShouldUpdateWindowSizes)
		{
			ImGuiViewport* Viewport = ImGui::GetWindowViewport();
			ImGui::SetNextWindowPos(ImVec2(0, MenuBarSize.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(LeftSidebarSize.x, Viewport->WorkSize.y), ImGuiCond_Always);
		}
	}

	void LEditorLayer::CheckRightSidebarSize()
	{
		if (ShouldUpdateWindowSizes)
		{
			ImGuiViewport* Viewport = ImGui::GetWindowViewport();
			ImGui::SetNextWindowPos(ImVec2(Viewport->Size.x - RightSidebarSize.x, MenuBarSize.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(RightSidebarSize.x, RightSidebarSize.y), ImGuiCond_Always);
		}
	}

	void LEditorLayer::SetScene(TObjectPtr<LScene> InScene)
	{
		m_Scene = InScene;

		GOnSceneSetActive.Broadcast(m_Scene);
		//SceneManagerPanel->SetScene(InScene);

		if (InScene->EditorCamera)
		{
			InScene->EditorCamera->SetActive(true);
		}
	}

	void LEditorLayer::CheckBottomBarSize()
	{
		if (ShouldUpdateWindowSizes)
		{
			ImGuiViewport* Viewport = ImGui::GetWindowViewport();
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.x, Viewport->Size.y - BottomBarSize.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(Viewport->Size.x - (LeftSidebarSize.x + RightSidebarSize.x), BottomBarSize.y), ImGuiCond_Always);
		}
	}

	void LEditorLayer::UI_TabManager()
	{
		//m_CurrentTabCount = TabManager::GetTabCount();
		static int LastTabCount = 0;
		const int CurrentTabCount = TabManager.GetTabCount();

		ImGuiViewport* Viewport = ImGui::GetWindowViewport();

		if (CurrentTabCount > 1)
		{
			/* Update the tab bar height as it might have changed. */
			ImGui::SetNextWindowPos({ LeftSidebarSize.x, MenuBarSize.y }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ Viewport->WorkSize.x, TabBarSize.y }, ImGuiCond_Always);
			ImGui::Begin("##LkTabBar", nullptr, UI::TabBarFlags);
			{
				if (ImGui::BeginTabBar("MainTab", ImGuiTabBarFlags_Reorderable))
				{
					for (const TabEntry& Entry: TabManager.Tabs)
					{
						static constexpr float Padding = { 36.0f };

						LTab& Tab = *Entry.second;
						const bool bIsTabSelected = (TabManager.GetActiveTab()->Index == Tab.Index);

						if (bIsTabSelected)
						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize(Tab.Name.c_str()).x + Padding);
						}
						if (ImGui::BeginTabItem(Entry.second->Name.c_str(), nullptr))
						{
							if (bIsTabSelected)
							{
								ImVec2 pos = (ImGui::GetItemRectMin());
								pos.x = ImGui::GetItemRectMax().x;
								pos.x -= Padding * 0.75f;
								pos.y += Padding * (1.0f / 8.0f);

								if (ImGui::CloseButton(ImGui::GetID(UI::GenerateID()), pos))
								{
									if (Tab.GetTabType() != EditorTabType::Viewport)
									{
										TabManager.CloseTab(Entry.second);
									}
								}
							}

							if (Tab.Closed == false)
							{
								TabManager.SetActiveTab(Entry.second);
							}

							if ((Tab.GetTabType() == EditorTabType::Viewport) || (Tab.Closed == true))
							{
								ImGui::EndTabItem();
								continue;
							}

							ImGui::SetNextWindowPos({ SecondViewportBounds[0].X, MenuBarSize.y + TabBarSize.y }, ImGuiCond_Always);
							ImGui::SetNextWindowSize({ EditorWindowSize.x, EditorWindowSize.y }, ImGuiCond_Always);
							ImGui::Begin("##TabWindow", NULL, 
										 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar 
										 | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
							Entry.second->OnImGuiRender();
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

		if (LastTabCount != CurrentTabCount)
		{
			bWindowsHaveChangedInSize = true;
		}

		LastTabCount = TabManager.GetTabCount();
	}


	void LEditorLayer::UI_MainMenuBar()
	{
		ImGui::BeginMainMenuBar();
		{
			MenuBarSize.x = ImGui::GetCurrentWindow()->Size.x;
			MenuBarSize.y = ImGui::GetFrameHeight();

			if (ImGui::BeginMenu("File"))
			{

				ImGui::EndMenu(); /* File. */
			}

			if (ImGui::BeginMenu("Project")) 
			{ 
				// Save project
				if (ImGui::MenuItem("Save")) 
				{ 
					m_Project->Save();
				}
				// New project
				if (ImGui::MenuItem("New")) 
				{ 
					m_Project = TObjectPtr<LProject>::Create("Project1");
				}
				// Load existing project
				if (ImGui::MenuItem("Load")) 
				{ 
				}
				if (ImGui::MenuItem("Recent")) 
				{ 
				}

				ImGui::EndMenu(); /* Project. */
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Renderer Settings"))
				{
					ShowRenderSettingsWindow = !ShowRenderSettingsWindow;
				}
				if (ImGui::MenuItem("EditorLayer Window Sizes"))
				{
					bShowEditorWindowSizesWindow = !bShowEditorWindowSizesWindow;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("New")) 
				{ 
				}
				if (ImGui::MenuItem("Load"))
				{
					TObjectPtr<LScene> newScene = TObjectPtr<LScene>::Create();
					LSceneSerializer SceneSerializer(newScene);
					SceneSerializer.Deserialize("scene.lukkelele");
					newScene->CopyTo(m_Scene);
				}
				if (ImGui::MenuItem("Save"))
				{
					LSceneSerializer Serializer(m_Scene);
					Serializer.Serialize("scene.lukkelele");
				}

				ImGui::EndMenu();
			}

			// Horizontal space
			ImGui::Dummy(ImVec2(40, 0));
			if (ImGui::BeginMenu(std::string("Project: " + m_Project->GetName()).c_str()))
			{
				ImGui::EndMenu(); /* Project + Name. */
			}

			if (ImGui::MenuItem("[Test] Throw Nullptr Exception"))
			{
				LK_CORE_WARN_TAG("Test", "Throwing nullptr exception to test LCrashHandler functionality");
				int* NumberPointer = nullptr;
				*NumberPointer = 60;
			}

			if (ImGui::MenuItem("[Test] Get count of Threads in ThreadPool"))
			{
				LK_CORE_WARN_TAG("ThreadManager", "Threads in ThreadPool: {}", LThreadManager::Instance().GetThreadPoolSize());
			}

			if (ImGui::MenuItem("[Test] Start Thread 1 (0)"))
			{
				constexpr int ThreadIndex = 0;
				LK_CORE_WARN_TAG("ThreadManager", "Starting thread -> {}", ThreadIndex);
				LThreadManager::Instance().StartThread(ThreadIndex);
			}

			if (ImGui::MenuItem("[Test] Submit Function to CommandQueue"))
			{
				constexpr int ThreadIndex = 0;
				LK_CORE_WARN_TAG("ThreadManager", "Submitting function CommandQueue");

				static uint16_t TestFunctionCounter = 0;
				auto TestFunction = []()
				{
					LK_CORE_DEBUG_TAG("TestThread2_Function", "Im a lambda function, Functions submitted: {}", TestFunctionCounter++);
					TThread::Sleep(1500ms);
					LK_CORE_DEBUG_TAG("TestThread2_Function", "Sleep  (1)");
					TThread::Sleep(1500ms);
					LK_CORE_DEBUG_TAG("TestThread2_Function", "Sleep  (2)");
					TThread::Sleep(1500ms);
					LK_CORE_DEBUG_TAG("TestThread2_Function", "Sleep  (3)");
				};
				LThreadManager::Instance().SubmitFunctionToThread(LApplication::Get()->Test_ThreadData2, TestFunction);
			}

		}
		ImGui::EndMainMenuBar();
	}

	LEntity LEditorLayer::CreateCube()
	{
		FAssetHandle CubeHandle = LAssetManager::GetAssetHandleFromFilePath("Assets/Meshes/Cube.gltf");
		TObjectPtr<LMesh> CubeMesh = LAssetManager::GetAsset<LMesh>(CubeHandle);

		LEntity NewCubeEntity = m_Scene->CreateEntity();

		/// TODO: Just use a search function instead of iterating through like this
		std::unordered_set<FAssetHandle> AssetList = m_Scene->GetAssetList();
		for (const FAssetHandle AssetHandle : AssetList)
		{
			TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(AssetHandle);
			if (Mesh == CubeMesh)
			{
				// The cube is in the scene
				std::vector<LEntity> SceneEntities = m_Scene->GetEntities();
				for (LEntity& Entity : SceneEntities)
				{
					if (Entity.HasComponent<LMeshComponent>())
					{
						LMeshComponent& EntityMesh = Entity.GetMesh();
						if (EntityMesh.Mesh == AssetHandle)
						{
							m_Scene->CopyComponentIfExists<LMeshComponent>(NewCubeEntity, m_Scene->m_Registry, Entity);
							m_Scene->CopyComponentIfExists<LTagComponent>(NewCubeEntity, m_Scene->m_Registry, Entity);
							LK_CORE_VERIFY((NewCubeEntity.HasComponent<LMeshComponent>()) 
										   && (NewCubeEntity.HasComponent<LTagComponent>()));
						}
					}
				}
			}
		}

		return NewCubeEntity;
	}

}
