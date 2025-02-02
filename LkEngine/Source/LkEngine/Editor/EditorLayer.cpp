#include "LKpch.h"
#include "EditorLayer.h"

#include "EditorTabManager.h"
#include "ComponentEditor.h"
#include "NodeEditor/NodeEditor.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Asset/AssetTypes.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Renderer/UI/UICore.h"
#include "LkEngine/Renderer/UI/Property.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Input/Mouse.h"

#include "LkEngine/Editor/AssetEditorManager.h"
#include "LkEngine/Editor/Panels/EditorSettingsPanel.h"
#include "LkEngine/Editor/Panels/EditorConsolePanel.h"
#include "LkEngine/Editor/Panels/ContentBrowserPanel.h"
#include "LkEngine/Editor/Panels/SceneManagerPanel.h"
#include "LkEngine/Editor/Panels/ToolsPanel.h"

#if defined(LK_ENGINE_OPENGL)
#	include "LkEngine/Renderer/Backend/OpenGL/OpenGLRenderer.h"
#	include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"
#endif

#include <nfd.hpp>


namespace LkEngine {

	namespace 
	{
		float DeltaTime = 0.0f; /* TODO: Make this a global variable. */
		
		bool bViewportHovered = false;
		bool bViewportFocused = false;
		bool bEditorViewportHovered = false;
		bool bEditorViewportFocused = false;

		LVector2 MenuBarSize = { 0.0f, 30.0f };
		LVector2 LeftSidebarSize = { 340.0f, 0.0f };
		LVector2 RightSidebarSize = { 340.0f, 0.0f };

		struct FTopBarData
		{
			LVector2 Size = { 0.0f, 54.0f };
			ImVec2 FramePadding{};
			ImVec2 WindowPadding{};
		} TopBar{};

		/* Windows. */
		bool bWindow_MetricTool = false;
		bool bWindow_IdStackTool = false;
		bool bWindow_StyleEditor = false;
		bool bWindow_ImGuiLogWindow = false;
		bool bWindow_RenderSettingsWindow = false; /* TODO: REMOVE */
		bool bRenderSkybox = true;

		float TimeSinceLastSave = 0.0f;

		std::vector<FSceneSelectionData> SceneSelectionData;

		constexpr int PROJECT_NAME_LENGTH_MIN = 4;
		constexpr int PROJECT_NAME_LENGTH_MAX = 255;
		constexpr int PROJECT_FILEPATH_LENGTH_MAX = 512;

		/** Input buffers used in UI contexts. */
		namespace InputBuffer 
		{
			char* ProjectName         = new char[PROJECT_NAME_LENGTH_MAX];
			char* OpenProjectFilePath = new char[PROJECT_FILEPATH_LENGTH_MAX];
			char* NewProjectFilePath  = new char[PROJECT_FILEPATH_LENGTH_MAX];
		}
	}

	namespace Cache 
	{
		ImGuiID FocusedWindowID = 0;
	}

	LEditorLayer::LEditorLayer()
		: LLayer("EditorLayer")
		, EditorSettings(FEditorSettings::Get())
	{
		LOBJECT_REGISTER();
		Instance = this;

		SceneSelectionData.clear();

		Window = &LApplication::Get().GetWindow();
		LK_CORE_VERIFY(Window);
	}

	/**
	 * TODO: Move lots of the initialization code to OnAttach instead.
	 */
	void LEditorLayer::Initialize()
	{
		/* Viewport framebuffer. */
		LK_CORE_DEBUG_TAG("Editor", "Creating viewport framebuffer");
		FFramebufferSpecification FramebufferSpec;
		FramebufferSpec.Attachments = { EImageFormat::RGBA32F, EImageFormat::DEPTH24STENCIL8 };
		FramebufferSpec.Samples = 1;
		FramebufferSpec.ClearColorOnLoad = false;
		FramebufferSpec.ClearColor = { 0.10f, 0.50f, 0.50f, 1.0f };
		FramebufferSpec.DebugName = "EditorViewport-FB";
		FramebufferSpec.Width = Window->GetWidth();
		FramebufferSpec.Height = Window->GetHeight();
		ViewportFramebuffer = LFramebuffer::Create(FramebufferSpec);

		/* Editor Camera. */
		LK_CORE_DEBUG_TAG("Editor", "Creating editor camera");
		EditorCamera = TObjectPtr<LEditorCamera>::Create(
			60.0f,               /* FOV    */ 
			Window->GetWidth(),  /* Width  */
			Window->GetHeight(), /* Height */
			0.10f,				 /* ZNear  */
			2400.0f              /* ZFar   */
		);			  
		EditorCamera->Initialize();
		EditorCamera->SetPosition({ -10, 4, -10 });

		/* Attach editor viewport delegates to the editor camera. */
		LK_CORE_ASSERT(EditorViewport);
		EditorViewport->OnSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			EditorCamera->SetViewportSize(NewWidth, NewHeight);

			/* Set scalers for the editor viewport. */
			LVector2 EditorWindowScalers;
			EditorWindowScalers.X = static_cast<float>(NewWidth) / Window->GetWidth();
			EditorWindowScalers.Y = static_cast<float>(NewHeight) / Window->GetHeight();
			EditorViewport->SetScalers(EditorWindowScalers);
		});

		/* TODO: Load last open project, else load an empty 'default' project. */
		if (LProject::Current() == nullptr)
		{
			if (true /* == HasStarterProject */)
			{
				StarterProject();
			}
			else
			{
				EmptyProject();
			}
		}

		/* Force editor viewport to sync data. */
		EditorViewport->SetDirty(true);

		/* Create the 2D renderer. */
		LK_CORE_DEBUG_TAG("Editor", "Creating 2D renderer");
		FRenderer2DSpecification Renderer2DSpec;
		Renderer2D = TObjectPtr<LRenderer2D>::Create(Renderer2DSpec);
		Renderer2D->Initialize();

		/* FIXME: Temporarily here. */
		LOpenGL_Debug::SetupSkybox();

		/** Bind input callbacks. */
		LMouse::OnMouseEnabled.Add(this, &LEditorLayer::OnMouseEnabled);
		LMouse::OnMouseButtonPressed.Add(this, &LEditorLayer::OnMouseButtonPressed);
		LMouse::OnMouseButtonReleased.Add(this, &LEditorLayer::OnMouseButtonReleased);
		LMouse::OnMouseScrolled.Add(this, &LEditorLayer::OnMouseScrolled);
		LMouse::OnCursorModeChanged.Add(this, &LEditorLayer::OnMouseCursorModeChanged);

		LKeyboard::OnKeyPressed.Add(this, &LEditorLayer::OnKeyPressed);
		LKeyboard::OnKeyReleased.Add(this, &LEditorLayer::OnKeyReleased);

		UI::OnMessageBoxCancelled.Add([&](const char* WindowName)
		{
			LK_CORE_DEBUG_TAG("Editor", "Message Box Cancelled: {}", WindowName);
		});

		LProject::OnProjectChanged.Add(this, &LEditorLayer::OnProjectChanged);

		LAssetEditorManager::RegisterEditors();

		LK_CORE_DEBUG_TAG("Editor", "Initialized");
	}

	void LEditorLayer::OnAttach()
	{
		LK_CORE_TRACE_TAG("Editor", "OnAttach");
		LK_CORE_VERIFY(Window);

		LObject::Initialize();

		std::memset(InputBuffer::ProjectName, 0, PROJECT_NAME_LENGTH_MAX);
		std::memset(InputBuffer::NewProjectFilePath, 0, PROJECT_NAME_LENGTH_MAX);
		std::memset(InputBuffer::OpenProjectFilePath, 0, PROJECT_NAME_LENGTH_MAX);

		/* Setup viewport bounds and sizes of the sidebars. */
		PrimaryViewportBounds[0] = { 0, 0 };
		PrimaryViewportBounds[1] = Window->GetViewportSize();
		LeftSidebarSize.Y = PrimaryViewportBounds[1].Y;
		RightSidebarSize.Y = PrimaryViewportBounds[1].Y;

		/* Editor viewport. */
		EditorViewport = TObjectPtr<LViewport>::Create();
		EditorViewport->SetViewportBounds(0, LVector2(0.0f, 0.0f));
		EditorViewport->SetViewportBounds(1, Window->GetSize());

		EditorViewport->SetSize(
			(Window->GetWidth() - LeftSidebarSize.X, RightSidebarSize.X), 
			(float)(Window->GetHeight())
		);

		/* Set scalers for the editor viewport. */
		const LVector2 EditorWindowSize = EditorViewport->GetSize();
		LVector2 EditorWindowScalers;
		EditorWindowScalers.X = EditorWindowSize.X / Window->GetWidth();
		EditorWindowScalers.Y = EditorWindowSize.Y / Window->GetHeight();
		EditorViewport->SetScalers(EditorWindowScalers);

		LK_CORE_DEBUG_TAG("Editor", "Loading editor resources");
		FEditorResources::Initialize();

		InitializePanelManager();

		/* Load editor settings. */
		EditorSettings.Load();

		LK_CORE_DEBUG_TAG("Editor", "Creating scene renderer");
		FSceneRendererSpecification RendererSpec{};
		SceneRenderer = TObjectPtr<LSceneRenderer>::Create(EditorScene, RendererSpec);
		
		/**
		 * Attach delegates from LSceneRenderer to LSceneManagerPanel. 
		 * Provides useful debugging information.
		 */
		/* TODO: Cannot pass TObjectPtr to LDelegate without TObjectPtr::Get() */
		auto SceneManagerPanel = PanelManager->GetPanel<LSceneManagerPanel>(PanelID::SceneManager);
		SceneRenderer->OnDrawListFlush.Add(SceneManagerPanel.Get(), &LSceneManagerPanel::OnSceneRendererDrawListFlush);

		GOnSceneSetActive.Add([&](const TObjectPtr<LScene>& NewActiveScene)
		{
			LK_CORE_TRACE_TAG("Editor", "[GOnSceneSetActive] Setting active scene: {}", NewActiveScene->GetName());
			SetScene(NewActiveScene);
		});

		Initialize();
	}

	void LEditorLayer::OnDetach()
	{
		LK_CORE_TRACE_TAG("Editor", "OnDetach");
		PanelManager->Serialize();
		LAssetEditorManager::UnregisterEditors();

		EditorSettings.Save();

		SceneRenderer->SetScene(nullptr);
	}

	void LEditorLayer::InitializePanelManager()
	{
		LK_CORE_DEBUG_TAG("Editor", "Creating editor panels");
		PanelManager = TObjectPtr<LPanelManager>::Create();

		/* Panel: Editor Console */
		auto EditorConsole = PanelManager->AddPanel<LEditorConsolePanel>(
			EPanelCategory::View,
			PanelID::EditorConsole,
			"Console Log",
			EPanelInitState::Open
		);

		/* Panel: Editor Settings */
		PanelManager->AddPanel<LEditorSettingsPanel>(
			EPanelCategory::View, 
			PanelID::EditorSettings,
			EPanelInitState::Closed
		);

		/* TODO: Add callbacks/delegates to content browser instance. */
		/* Panel: Content Browser */
		auto ContentBrowser = PanelManager->AddPanel<LContentBrowser>(
			EPanelCategory::View,
			PanelID::ContentBrowser,
			"Content Browser",
			EPanelInitState::Open
		);

		/* Panel: Scene Manager */
		auto SceneManager = PanelManager->AddPanel<LSceneManagerPanel>(
			EPanelCategory::View, 
			PanelID::SceneManager, 
			"Scene Manager", 
			EPanelInitState::Open, 
			EditorScene
		);
		OnSceneSelectionUpdated.Add(SceneManager.Get(), &LSceneManagerPanel::OnSceneSelectionUpdated);

		PanelManager->AddPanel<LToolsPanel>(EPanelCategory::View, PanelID::Tools, EPanelInitState::Closed);
		//PanelManager->AddPanel<LMaterialEditorPanel>(EPanelCategory::Edit, PanelID::MaterialEditor, EPanelInitState::Closed);

		PanelManager->Deserialize();
		PanelManager->Initialize();
	}


	void LEditorLayer::Tick(const float InDeltaTime)
	{
		LK_PROFILE_FUNC();
		DeltaTime = InDeltaTime;

		EditorViewport->Update();

		switch (SceneState)
		{
			case ESceneState::Edit:
			{
				EditorCamera->SetActive(true);
				EditorScene->OnUpdateEditor(DeltaTime);
				EditorScene->OnRenderEditor(SceneRenderer, *EditorCamera, DeltaTime);

				if (bRenderSkybox)
				{
					LRenderer::Submit([&]()
					{
						LOpenGL_Debug::RenderSkybox(EditorCamera);
					});
				}

				Render2D();
				
				break;
			}

			case ESceneState::Play:
			{
				RuntimeScene->OnUpdateRuntime(DeltaTime);
				break;
			}

			case ESceneState::Pause:
			{
				EditorCamera->SetActive(true);
				EditorCamera->Tick(DeltaTime);

				RuntimeScene->OnRenderRuntime(ViewportRenderer, DeltaTime);
				break;
			}

			case ESceneState::Simulate:
			{
				/* TODO */
				break;
			}
		}

		LAssetEditorManager::Tick(DeltaTime);
	}

	void LEditorLayer::RenderUI()
	{
		ImGuiViewportP* Viewport = (ImGuiViewportP*)ImGui::GetMainViewport();
		ImGuiIO& IO = ImGui::GetIO();
		ImGuiStyle& Style = ImGui::GetStyle();

		/* TODO: Investigate the resizing */
		IO.ConfigWindowsResizeFromEdges = IO.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		UI::BeginViewport(Window);

		/* Main menu. */
		UI_MainMenuBar();

		/* Render editor panels. */
		PanelManager->RenderUI();

		/*----------------------------------------------------------------------------
			                              Top Bar
		 ----------------------------------------------------------------------------*/
		UI_PrepareTopBar();
		UI::Begin(LK_UI_TOPBAR, nullptr, UI::SidebarFlags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		{
			LK_UI_DEBUG_DOCKNODE(LK_UI_TOPBAR);
			UI_ToolBar();
		}
		UI::End();

		/*----------------------------------------------------------------------------
			                             Sidebar 1
		 ----------------------------------------------------------------------------*/
		UI_PrepareLeftSidebar();
		UI::Begin(LK_UI_SIDEBAR_1, nullptr, UI::SidebarFlags);
		{
			LK_UI_DEBUG_DOCKNODE(LK_UI_SIDEBAR_1);
			
			TObjectPtr<LRenderContext> RenderContext = LWindow::Get().GetRenderContext();

			/* Blending. */
			static bool bBlending = false;
			bBlending = RenderContext->GetBlendingEnabled();
			if (ImGui::Checkbox("Blending", &bBlending))
			{
				Window->SetBlendingEnabled(bBlending);
			}

			/* Depth testing. */
			static bool bDepthTesting = false;
			bDepthTesting = RenderContext->GetDepthEnabled();
			if (ImGui::Checkbox("Depth Testing", &bDepthTesting))
			{
				Window->SetDepthEnabled(bDepthTesting);
			}

			static bool bCulling = false;
			if (ImGui::Checkbox("Backface Culling", &bCulling))
			{
				LK_CORE_DEBUG_TAG("Editor", "{} backface culling", bCulling ? "Enabling" : "Disabling");
				if (bCulling)
				{
					LK_OpenGL_Verify(glEnable(GL_CULL_FACE));
				}
				else
				{
					LK_OpenGL_Verify(glDisable(GL_CULL_FACE));
				}
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Shader Sandbox"))
			{
				ImGui::Text("Material: WoodContainer");
				ImGui::SameLine();
				if (ImGui::Button("Toggle: u_TestBool"))
				{
					static bool UniformValue = true;
					LShader& Shader = *LEditorAssetManager::Material_WoodContainer->GetMaterial()->GetShader();
					Shader.Set("u_TestBool", UniformValue);
					LK_CORE_CONSOLE_INFO("Toggled u_TestBool -> {}", (int)UniformValue);

					UniformValue = !UniformValue;
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Editor Viewport Info"))
			{
				ImGui::Text("Editor Viewport Bounds 0: %s", EditorViewport->GetViewportBounds(0).ToString<const char*>());
				ImGui::Text("Editor Viewport Bounds 1: %s", EditorViewport->GetViewportBounds(1).ToString<const char*>());
				ImGui::Dummy(ImVec2(0, 8));
				if (ImGuiViewport* MainViewport = ImGui::GetMainViewport(); MainViewport != nullptr)
				{
					ImGui::Text("Viewport Size: (%.2f, %.2f)", MainViewport->Size.x, MainViewport->Size.y);
					ImGui::Text("Viewport WorkSize: (%.2f, %.2f)", MainViewport->WorkSize.x, MainViewport->WorkSize.y);
					ImGui::Text("Viewport Pos: (%.2f, %.2f)", MainViewport->Pos.x, MainViewport->Pos.y);
				}

				ImGui::Dummy(ImVec2(0, 8));
				ImGui::Text("TopBar: %s", TopBar.Size.ToString<const char*>());

				if (ImGuiWindow* EditorViewportWindow = ImGui::FindWindowByName(LK_UI_EDITOR_VIEWPORT))
				{
					ImGui::Dummy(ImVec2(0, 8));
					ImGui::Text("Editor Window Size: %s", LVector2(EditorViewportWindow->Size).ToString<const char*>());
					ImGui::Text("Editor Window Pos: %s", LVector2(EditorViewportWindow->Pos).ToString<const char*>());
					ImGui::Text("Editor Window ViewportPos: %s", LVector2(EditorViewportWindow->ViewportPos).ToString<const char*>());

					if (ImGuiDockNode* EditorWindowNode = EditorViewportWindow->DockNode)
					{
						ImGui::Dummy(ImVec2(0, 8));
						ImGui::Text("Editor Dock Window Size: %s", LVector2(EditorWindowNode->Size).ToString<const char*>());
						ImGui::Text("Editor Dock Window Pos: %s", LVector2(EditorWindowNode->Pos).ToString<const char*>());

						if (ImGuiDockNode* ParentNode = EditorViewportWindow->DockNode->ParentNode)
						{
							ImGui::Dummy(ImVec2(0, 8));
							if (ParentNode->VisibleWindow)
							{
								ImGui::Text("Parent Dock Window: %s", ParentNode->VisibleWindow->Name);
							}
							else
							{
								ImGui::Text("Parent Dock Window: NULL");
							}
							ImGui::Text("Parent Dock Window Size: %s", LVector2(ParentNode->Size).ToString().c_str());
							ImGui::Text("Parent Dock Window Pos: %s", LVector2(ParentNode->Pos).ToString().c_str());
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Editor Console Testing"))
			{
				if (ImGui::Button("Console Log: Spam"))
				{
					LK_CONSOLE_DEBUG("Editor panels initialized");
					LK_CONSOLE_INFO("Info message 1");
					LK_CONSOLE_INFO("Info message 2");
					LK_CONSOLE_ERROR("Error message");
					LK_CONSOLE_WARN("Warn message");
				}

				if (ImGui::Button("Console Log: DEBUG"))
				{
					LK_CONSOLE_DEBUG("Test log DEBUG message");
				}

				if (ImGui::Button("Console Log: INFO"))
				{
					LK_CONSOLE_INFO("Test log INFO message");
				}

				if (ImGui::Button("Console Log: WARN"))
				{
					LK_CONSOLE_WARN("Test log WARN message");
				}

				if (ImGui::Button("Console Log: ERROR"))
				{
					LK_CONSOLE_ERROR("Test log ERROR message");
				}

				ImGui::TreePop();
			}

			/* Color modification. */
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Colors"))
			{
				UI_ClearColorModificationMenu();

				ImGui::TreePop(); /* Colors. */
			}

			ImGui::Separator();

			ImGui::BeginGroup();
			static std::string ButtonLabel;
			if (Window->IsMaximized())
			{
				ButtonLabel = "Restore Window";
			}
			else
			{
				ButtonLabel = "Maximize Window";
			}
			if (ImGui::Button(ButtonLabel.c_str()))
			{
				Window->Maximize();
			}
			ImGui::EndGroup();

			ImGui::Text("Editor Viewport Focused: %s", UI::IsWindowFocused(LK_UI_EDITOR_VIEWPORT) ? "Yes" : "No");
			if (ImGuiWindow* EditorViewportWindow = ImGui::FindWindowByName(LK_UI_EDITOR_VIEWPORT); EditorViewportWindow != nullptr)
			{
				ImGui::Text("Editor Viewport ID: %lld", EditorViewportWindow->ID);
			}
			else
			{
				ImGui::Text("Editor Viewport ID: Unknown");
			}
		}
		UI::End(); /* LK_UI_SIDEBAR_1 */

		/*----------------------------------------------------------------------------
			                             Sidebar 2
		 ----------------------------------------------------------------------------*/
		UI_PrepareRightSidebar();
		UI::Begin(LK_UI_SIDEBAR_2, nullptr, UI::SidebarFlags);
		{
			LK_UI_DEBUG_DOCKNODE(LK_UI_SIDEBAR_2);

			/* Window Information. */
			if (ImGui::TreeNode("Window Information"))
			{
				UI_ShowViewportAndWindowDetails();
				ImGui::TreePop();
			}

			/* Mouse Information. */
			if (ImGui::TreeNode("Mouse Information"))
			{
				ImGui::Text("-- Empty for now --");
				ImGui::TreePop();
			}

			if (ImGui::Checkbox("Render Skybox", &bRenderSkybox)) 
			{
			}

			if (ImGui::TreeNode("Config Directories"))
			{
				ImGui::Text("Resources: %S", LFileSystem::GetResourcesDir().c_str());
				ImGui::Text("Assets: %S", LFileSystem::GetAssetsDir().c_str());
				ImGui::Text("Engine: %S", LFileSystem::GetEngineDir().c_str());
				ImGui::Text("Config: %S", LFileSystem::GetConfigDir().c_str());
				ImGui::Text("Engine: %S", LFileSystem::GetEngineConfig().c_str());

				ImGui::TreePop();
			}
		}
		UI::End(); /* LK_UI_SIDEBAR_2 */

		/*----------------------------------------------------------------------------
									  Editor Viewport
		-----------------------------------------------------------------------------*/
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		UI::Begin(LK_UI_CORE_VIEWPORT, nullptr, UI::CoreViewportFlags);
		{
			bViewportHovered = ImGui::IsWindowHovered();
			bViewportFocused = ImGui::IsWindowFocused();

			UI_PrepareEditorViewport();
			UI::Begin(LK_UI_EDITOR_VIEWPORT, nullptr, UI::EditorViewportFlags);
			{
				bEditorViewportHovered = ImGui::IsWindowHovered();
				bEditorViewportFocused = ImGui::IsWindowFocused();

				/* Render viewport image. */
				UI_ViewportTexture();

				const auto& Selection = LSelectionContext::GetSelected(ESelectionContext::Scene);
				for (const LUUID& EntityID : Selection)
				{
					const LEntity SelectedEntity = EditorScene->TryGetEntityWithUUID(LSelectionContext::SelectedHandle);
					if (SelectedEntity)
					{
						DrawObjectGizmo(SelectedEntity);
					}
				}
					
				UI_WindowStatistics();
				UI_HandleDragAndDrop();
			}
			UI::End(); /* LK_UI_EDITOR_VIEWPORT */
		}
		UI::End();
		ImGui::PopStyleVar(2); /* FramePadding, WindowPadding. */

		const LVector2 ViewportSize = { Viewport->WorkSize.x, Viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(ViewportSize);

		UI::RenderMessageBoxes();
		UI_RenderExternalWindows();

		LAssetEditorManager::RenderUI();

		ImGui::End(); /* Core Viewport. */
	}

	void LEditorLayer::UI_ViewportTexture()
	{
		TObjectPtr<LImage2D> ViewportImage = ViewportFramebuffer->GetImage(0);
		const LVector2 WindowSize = EditorViewport->GetSize();

		const float ImagePadding = ImGui::GetStyle().DockingSeparatorSize;
		UI::ShiftCursor(ImagePadding, ImagePadding);
		UI::Image(
			ViewportImage,
			ImVec2((WindowSize.X - ImagePadding), (WindowSize.Y - ImagePadding)),
			ImVec2(0, 1), /* UV0 */
			ImVec2(1, 0), /* UV1 */
			ImVec4(1, 1, 1, 1) /* Tint Color   */
			//ImVec4(1, 1, 0, 1)  /* Border Color */
		);
	}

	void LEditorLayer::RenderViewport()
	{
		if (!EditorScene)
		{
			return;
		}

		/* Render floor. */
		LRenderer::Submit([&]()
		{
			LRenderer::GetViewportFramebuffer()->Bind();
			LOpenGL_Debug::RenderFloor(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LFramebuffer::TargetSwapChain();
		});

		/**
		 * Render the skybox.
		 * NOTE: This is only here until cube environments are able to be submitted as an entity to the renderer.
		 */
		if (bRenderSkybox)
		{
			LRenderer::Submit([&]()
			{
				LOpenGL_Debug::RenderSkybox(EditorCamera);
			});
		}
	}

	void LEditorLayer::Render2D()
	{
		Renderer2D->BeginScene(*EditorCamera);
		{
			/**
			 * TODO:
			 *   - Draw bounding boxes.
			 *   - Draw/Mark selected entities.
			 */
		}
		Renderer2D->EndScene();
	}

	void LEditorLayer::DrawObjectGizmo(LEntity Entity)
	{
		LTransformComponent& TransformComponent = Entity.Transform();
		glm::mat4 TransformMatrix = TransformComponent.GetTransform();
		const glm::vec3& CameraPos = EditorCamera->GetPosition();
		const glm::mat4& ViewMatrix = EditorCamera->GetViewMatrix();
		const glm::mat4& ProjectionMatrix = EditorCamera->GetProjectionMatrix();

		/* Viewport bounds can be indexed from 0 to 1. */
		const LVector2* EditorViewportBounds = EditorViewport->GetViewportBounds();
		const float PosX = EditorViewportBounds[0].X;
		const float PosY = EditorViewportBounds[0].Y;

		if (ImGuiWindow* Window = ImGui::FindWindowByName(LK_UI_EDITOR_VIEWPORT))
		{
			const LVector2 EditorWindowSize = EditorViewport->GetSize();
			ImGui::Begin(Window->Name, nullptr, UI::CoreViewportFlags | ImGuiWindowFlags_NoScrollbar);
		#if 1
			ImGuizmo::SetOrthographic(static_cast<int>(EditorCamera->GetProjectionType()));
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			const bool ShouldSnapValues = LInput::IsKeyDown(EKey::LeftControl);
			const float SnapValue = EditorSettings.TranslationSnapValue;
			const float SnapValues[3] = { SnapValue, SnapValue, SnapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(ViewMatrix),
				glm::value_ptr(ProjectionMatrix),
				static_cast<ImGuizmo::OPERATION>(Gizmo),
				ImGuizmo::LOCAL,
				glm::value_ptr(TransformMatrix),
				nullptr,
				ShouldSnapValues ? SnapValues : nullptr
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
		#endif

			ImGui::End();
		}
	}

	void LEditorLayer::UI_RenderExternalWindows()
	{
		if (bWindow_MetricTool)
		{
			ImGui::ShowMetricsWindow(&bWindow_MetricTool);
		}

		if (bWindow_IdStackTool)
		{
			ImGui::ShowIDStackToolWindow(&bWindow_IdStackTool);
		}

		if (bWindow_StyleEditor)
		{
			ImGuiStyle& Style = ImGui::GetStyle();
			ImGui::Begin("Style Editor", &bWindow_StyleEditor);
			ImGui::ShowStyleEditor(&Style);
			ImGui::End();
		}

		if (bWindow_ImGuiLogWindow)
		{
			ImGui::ShowDebugLogWindow(&bWindow_ImGuiLogWindow);
		}
	}

	void LEditorLayer::NewScene(const std::string& InSceneName)
	{
		LK_CORE_DEBUG_TAG("Editor", "New scene: {}", InSceneName);
		EditorScene = TObjectPtr<LScene>::Create(InSceneName, true);

		EditorScene->SetActive(true);
		if (EditorScene->IsActive() && (CurrentScene != EditorScene))
		{
			if (CurrentScene)
			{
				/* TODO: Save current scene before re-assigning. */
			}
			CurrentScene = EditorScene;
		}
	}

	void LEditorLayer::OpenProject(const std::filesystem::path& ProjectPath)
	{
		LK_CORE_ASSERT(!ProjectPath.empty());
		LK_CORE_INFO_TAG("Editor", "Opening project: {}", ProjectPath);
	}

	void LEditorLayer::EmptyProject()
	{
		LK_CORE_INFO_TAG("Editor", "Loading empty project");
		if (TObjectPtr<LProject> CurrentProject = LProject::Current(); CurrentProject != nullptr)
		{
			/* TODO: Close already opened project. */
		}

		NewScene("Untitled");

		LK_CORE_INFO_TAG("Editor", "Creating empty project");
		Project = TObjectPtr<LProject>::Create();
		Project->SetName("EmptyProject");
		LProject::SetActive(Project);
	}

	void LEditorLayer::StarterProject()
	{
		//static const std::string StarterProjectName = "Starter-2024";
		static const std::string StarterProjectName = "Starter-2025";
		LK_CORE_INFO_TAG("Editor", "Loading starter project: {}", StarterProjectName);

		if (TObjectPtr<LProject> CurrentProject = LProject::Current(); CurrentProject != nullptr)
		{
			/* TODO: Close already opened project. */
		}

		NewScene(StarterProjectName);

		Project = TObjectPtr<LProject>::Create();
		Project->Load(std::format("Projects/{}", StarterProjectName)); /* TODO: Just use project name, don't pass the directory. */
		LProject::SetActive(Project);

		/* FIXME: Until asset creation is supported from within the editor. */
	#if 0
		LEntity Entity_Cube1 = EditorScene->CreateEntity("Cube-1");
		auto& MeshComp1 = Entity_Cube1.AddComponent<LMeshComponent>();

		LEntity Entity_Cube2 = EditorScene->CreateEntity("Cube-2");
		auto& MeshComp2 = Entity_Cube2.AddComponent<LMeshComponent>();

		LEntity Entity_Cube3 = EditorScene->CreateEntity("Cube-3");
		auto& MeshComp3 = Entity_Cube3.AddComponent<LMeshComponent>();
	#endif
		
	#if 0
		LEntity Entity_StaticCube1 = EditorScene->CreateEntity("Static-Cube-1");
		LStaticMeshComponent& StaticMeshComp1 = Entity_StaticCube1.AddComponent<LStaticMeshComponent>();

		LEntity Entity_StaticCube2 = EditorScene->CreateEntity("Static-Cube-2");
		LStaticMeshComponent& StaticMeshComp2 = Entity_StaticCube2.AddComponent<LStaticMeshComponent>();

		LEntity Entity_StaticCube3 = EditorScene->CreateEntity("Static-Cube-3");
		LStaticMeshComponent& StaticMeshComp3 = Entity_StaticCube3.AddComponent<LStaticMeshComponent>();
	#endif

	#if 0
		LEntity Entity_StaticPlane = EditorScene->CreateEntity("Static-Plane-1");
		LStaticMeshComponent& StaticMesh_Plane = Entity_StaticPlane.AddComponent<LStaticMeshComponent>();
	#endif

	#if 0
		LEntity Entity_StaticRamp1 = EditorScene->CreateEntity("Static-Ramp-1");
		LStaticMeshComponent& StaticMesh_Plane = Entity_StaticRamp1.AddComponent<LStaticMeshComponent>();
	#endif
	}

	void LEditorLayer::CreateProject(const std::filesystem::path& ProjectPath)
	{
		LK_CORE_CONSOLE_DEBUG("Creating project: {}", ProjectPath);
		if (!LFileSystem::Exists(ProjectPath))
		{
			LK_CORE_DEBUG_TAG("Editor", "Creating directory");
			if (!LFileSystem::CreateDirectory(ProjectPath))
			{
				LK_CORE_ERROR_TAG("Editor", "Directory creation failed with path: {}", ProjectPath);
				return;
			}
		}

		if (!LFileSystem::Exists("Resources/Template/NewProject"))
		{
			LK_CORE_ERROR_TAG("Editor", "Cannot create new project, the template 'New Project' does not exist");
			return;
		}

		LK_CORE_CONSOLE_DEBUG("Copying 'New Project' template to new project directory");
		std::filesystem::copy(
			"Resources/Template/NewProject", 
			ProjectPath, 
			std::filesystem::copy_options::recursive
		);

		std::string EngineRootDir = LFileSystem::GetEngineDir().string();
		LK_CORE_ASSERT(!EngineRootDir.empty(), "The engine directory is empty");
		std::replace(EngineRootDir.begin(), EngineRootDir.end(), '\\', '/');
		LK_CORE_CONSOLE_DEBUG("Clean dir: {}", EngineRootDir);

		/**
		 * Create the project file. 
		 *
		 * Copies the template project file and replaces the token project name 
		 * with the project name entered from the project creation popup.
		 */
		std::filesystem::path NewProjectPath;
		{
			const std::filesystem::path CompleteProjectPath(ProjectPath / "Project.lkproj");

			std::ifstream InputStream(CompleteProjectPath);
			LK_CORE_VERIFY(InputStream.is_open(), "InputStream failed for: '{}'", CompleteProjectPath.string());
			std::stringstream StringStream;
			StringStream << InputStream.rdbuf();
			InputStream.close();

			std::string ProjString = StringStream.str();
			LK_CORE_VERIFY(std::strlen(InputBuffer::ProjectName) > 0);
			StringUtils::ReplaceToken(ProjString, "$PROJECT_NAME$", InputBuffer::ProjectName);

			std::ofstream OStream(CompleteProjectPath);
			OStream << ProjString;
			OStream.close();

			const std::string NewProjectFileName = std::string(InputBuffer::ProjectName) + ".lkproj";
			NewProjectPath = ProjectPath / (std::string(InputBuffer::ProjectName) + ".lkproj");
			std::filesystem::rename(CompleteProjectPath, NewProjectPath);
		}

		if (NewProjectPath.empty())
		{
			LK_CORE_CONSOLE_ERROR("An error occured whilst creating a new project located at: ({})", ProjectPath);
			return;
		}

		LK_CORE_CONSOLE_DEBUG("Creating template directories for the new project");
		LFileSystem::CreateDirectory(ProjectPath / "Assets" / "Textures");
		LFileSystem::CreateDirectory(ProjectPath / "Assets" / "Meshes" / "Default");
		LFileSystem::CreateDirectory(ProjectPath / "Assets" / "Meshes" / "Default" / "Source");
		LFileSystem::CreateDirectory(ProjectPath / "Assets" / "Scenes");
		LFileSystem::CreateDirectory(ProjectPath / "Assets" / "Materials");

		OpenProject(NewProjectPath);
	}

	void LEditorLayer::SaveProjectAs()
	{
	}

	void LEditorLayer::OnProjectChanged(TObjectPtr<LProject> InProject)
	{
		LK_CORE_DEBUG_TAG("Editor", "OnProjectChanged -> {}", InProject->GetName());
		std::memset(InputBuffer::ProjectName, 0, PROJECT_NAME_LENGTH_MAX);
		std::memset(InputBuffer::NewProjectFilePath, 0, PROJECT_NAME_LENGTH_MAX);
		std::memset(InputBuffer::OpenProjectFilePath, 0, PROJECT_NAME_LENGTH_MAX);
	}

	void LEditorLayer::OnViewportSizeUpdated(const uint16_t NewWidth, const uint16_t NewHeight)
	{
		LK_CORE_ASSERT(EditorViewport);
		LK_CORE_ASSERT(NewWidth > 0 && NewHeight > 0, "Viewport size update failed, division by zero");

		/* Update scalers. */
		const LVector2 EditorWindowSize = EditorViewport->GetSize();
		LVector2 WindowScalers;
		WindowScalers.X = EditorWindowSize.X / NewWidth;
		WindowScalers.Y = EditorWindowSize.Y / NewHeight;
		EditorViewport->SetScalers(WindowScalers);

		PrimaryViewportBounds[0] = { 0, 0 };
		PrimaryViewportBounds[1] = { NewWidth, NewHeight };
	}

	void LEditorLayer::UpdateWindowTitle(const std::string& SceneName)
	{
		const std::string Title = std::format(
			"{} ({}) - LkEditor - {} ({}) (OpenGL)",
			SceneName, LProject::Current()->GetConfiguration().Name, LApplication::GetPlatformName(), LApplication::GetConfigurationName()
		);

		LApplication::Get().GetWindow().SetTitle(Title);
	}

	void LEditorLayer::OnKeyPressed(const FKeyData& KeyData)
	{
		LK_CORE_TRACE_TAG("Editor", "OnKeyPressed: {}", Enum::ToString(KeyData.Key));

		if (UI::IsWindowFocused(LK_UI_EDITOR_VIEWPORT) || UI::IsWindowFocused(LK_UI_SCENEMANAGER))
		{
			if ((bViewportHovered || bEditorViewportHovered) && !LInput::IsMouseButtonDown(EMouseButton::Right))
			{
				switch (KeyData.Key)
				{
					case EKey::Q:
						LK_CORE_DEBUG_TAG("Editor", "Gizmo: All");
						Gizmo = -1;
						break;

					case EKey::W:
						LK_CORE_DEBUG_TAG("Editor", "Gizmo: Translate");
						Gizmo = ImGuizmo::OPERATION::TRANSLATE;
						break;

					case EKey::E:
						LK_CORE_DEBUG_TAG("Editor", "Gizmo: Rotate");
						Gizmo = ImGuizmo::OPERATION::ROTATE;
						break;

					case EKey::R:
						LK_CORE_DEBUG_TAG("Editor", "Gizmo: Scale");
						Gizmo = ImGuizmo::OPERATION::SCALE;
						break;

					case EKey::F:
						LK_CORE_DEBUG_TAG("Editor", "Focusing on selected entity");
						/* TODO: Finish this impl. */
						break;
				}
			}

			switch (KeyData.Key)
			{
				case EKey::Escape:
				{
					LK_CORE_DEBUG_TAG("Editor", "Pressed Escape, deselecting");
					LSelectionContext::DeselectAll();

					SceneSelectionData.clear();
					OnSceneSelectionUpdated.Broadcast(SceneSelectionData);

					break;
				}
			}
		}
	}

	void LEditorLayer::OnKeyReleased(const FKeyData& KeyData)
	{
		LK_CORE_TRACE_TAG("Editor", "OnKeyReleased: {}", Enum::ToString(KeyData.Key));
	}

	void LEditorLayer::OnMouseEnabled(const bool Enabled)
	{
		LK_CORE_TRACE_TAG("Editor", "OnMouseEnabled: {}", Enabled);
		if (Enabled)
		{
			UI_SetWindowFocus(Cache::FocusedWindowID);
		}
		else
		{
			UI_CacheWindowFocus();
		}
	}

	void LEditorLayer::OnMouseButtonPressed(const FMouseButtonData& ButtonData)
	{
		LK_CORE_TRACE_TAG("Editor", "MouseButtonPressed: {}", Enum::ToString(ButtonData.Button));

		if (ButtonData.Button != EMouseButton::Left)
		{
			return;
		}

		if (!bViewportHovered && !bEditorViewportHovered)
		{
			return;
		}

		if (LInput::IsKeyDown(EKey::LeftAlt) || LInput::IsMouseButtonDown(EMouseButton::Right))
		{
			return;
		}

		if (ImGuizmo::IsOver())
		{
			LK_CORE_TRACE_TAG("Editor", "MouseButtonPressed: '{}', ImGuizmo is active, returning", Enum::ToString(ButtonData.Button));
			return;
		}

		if (CurrentScene)
		{
			RaycastScene(CurrentScene, SceneSelectionData);
		}

		const bool CtrlDown = LInput::IsKeyDown(EKey::LeftControl) || LInput::IsKeyDown(EKey::RightControl);
		const bool ShiftDown = LInput::IsKeyDown(EKey::LeftShift) || LInput::IsKeyDown(EKey::RightShift);

		if (!CtrlDown)
		{
			LK_CORE_TRACE_TAG("Editor", "Mouse button pressed without Ctrl, deselecting selected entities");
			LSelectionContext::DeselectAll();
		}

		if (!SceneSelectionData.empty())
		{
			FSceneSelectionData& SelectionEntry = SceneSelectionData.front();

			LEntity Entity = SelectionEntry.Entity;
			if (ShiftDown)
			{
				while (Entity.GetParent())
				{
					Entity = Entity.GetParent();
				}
			}

			/* Toggle selection if entity is already selected. */
			if (LSelectionContext::IsSelected(ESelectionContext::Scene, Entity.GetUUID()) && CtrlDown)
			{
				LK_CORE_DEBUG_TAG("Editor", "Deselecting {} ({}), Ctrl is down", Entity.Name(), Entity.GetUUID());
				LSelectionContext::Deselect(ESelectionContext::Scene, Entity.GetUUID());
			}
			else
			{
				LK_CORE_INFO_TAG("Editor", "Selecting: {} ({})", Entity.Name(), Entity.GetUUID());
				LSelectionContext::Select(ESelectionContext::Scene, Entity.GetUUID());
			}


			OnSceneSelectionUpdated.Broadcast(SceneSelectionData);
		}
	}

	void LEditorLayer::OnMouseButtonReleased(const FMouseButtonData& ButtonData)
	{
		LK_CORE_TRACE_TAG("Editor", "MouseButtonReleased: {}", Enum::ToString(ButtonData.Button));
	}

	void LEditorLayer::OnMouseScrolled(const EMouseScrollDirection ScrollDir)
	{
		LK_CORE_TRACE_TAG("Editor", "MouseScrolled: {}", Enum::ToString(ScrollDir));

		if (EditorCamera)
		{
			if (UI::IsWindowHovered(LK_UI_EDITOR_VIEWPORT))
			{
				if (ScrollDir == EMouseScrollDirection::Up)
				{
					EditorCamera->MouseZoom(-0.010f);
				}
				else if (ScrollDir == EMouseScrollDirection::Down)
				{
					EditorCamera->MouseZoom(0.010f);
				}
			}
		}
	}

	void LEditorLayer::OnMouseCursorModeChanged(const ECursorMode CursorMode)
	{
		LK_CORE_TRACE_TAG("Editor", "Cursor Mode: {}", Enum::ToString(CursorMode));
	}

	void LEditorLayer::OnScenePlay()
	{
		LK_CORE_TRACE_TAG("Editor", "OnScenePlay");
	}

	void LEditorLayer::OnSceneStop()
	{
		LK_CORE_TRACE_TAG("Editor", "OnSceneStop");
	}

	void LEditorLayer::SaveScene()
	{
		if (!SceneFilePath.empty())
		{
			LK_CORE_ASSERT(EditorScene, "SaveScene failed, EditorScene is nullptr");
			LSceneSerializer Serializer(EditorScene);
			Serializer.Serialize(SceneFilePath);

			TimeSinceLastSave = 0.0f;
		}
		else
		{
			LK_CORE_DEBUG_TAG("Editor", "SaveScene, no filepath exists, performing SaveSceneAs");
			SaveSceneAs();
		}
	}

	void LEditorLayer::SaveSceneAs()
	{
		namespace fs = std::filesystem;
		LK_CORE_INFO_TAG("Editor", "Saving current scene");
		fs::path Filepath = LFileSystem::SaveFileDialog({ { "LkEngine Scene", "lkscene" } });
		if (Filepath.empty())
		{
			return;
		}

		if (!Filepath.has_extension())
		{
			Filepath += LScene::FILE_EXTENSION;
		}

		LK_CORE_DEBUG("Scene filepath: {}", Filepath.string());
		LSceneSerializer Serializer(EditorScene);

		fs::path Path = Filepath;
		UpdateWindowTitle(Path.filename().string());
		SceneFilePath = Filepath.string();
		std::replace(SceneFilePath.begin(), SceneFilePath.end(), '\\', '/');
		LK_CORE_DEBUG_TAG("Editor", "Scene filepath: {}", SceneFilePath);
	}

	void LEditorLayer::AutoSaveScene()
	{
		LK_CORE_DEBUG_TAG("Editor", "Autosaving scene");
		if (!SceneFilePath.empty())
		{
			LSceneSerializer Serializer(EditorScene);
			Serializer.Serialize(SceneFilePath + ".auto");

			TimeSinceLastSave = 0.0f;
		}
	}

	void LEditorLayer::UI_PrepareTopBar()
	{
		if (ImGuiWindow* TopBarWindow = ImGui::FindWindowByName(LK_UI_TOPBAR); TopBarWindow != nullptr)
		{
			TopBarWindow->Flags |= ImGuiWindowFlags_NoTitleBar;
			if (ImGuiDockNode* DockNode = TopBarWindow->DockNode; DockNode != nullptr)
			{
				if ((DockNode->Size.x <= 0.0f) || (DockNode->Size.y <= 0.0f))
				{
					return;
				}
				TopBar.Size.X = DockNode->Size.x;
				DockNode->Size.y = TopBar.Size.Y;
				DockNode->SizeRef.y = DockNode->Size.y; /* TODO: Is this needed? */

				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoResize;
			}

			ImGuiStyle& Style = ImGui::GetStyle();
			TopBar.FramePadding = Style.FramePadding;
			TopBar.WindowPadding = TopBarWindow->WindowPadding;
		}
	}

	void LEditorLayer::UI_PrepareLeftSidebar() const
	{
		if (ImGuiWindow* SidebarWindow = ImGui::FindWindowByName(LK_UI_SIDEBAR_1); SidebarWindow != nullptr)
		{
			if (ImGuiDockNode* DockNode = SidebarWindow->DockNode; DockNode != nullptr)
			{
				if ((DockNode->Size.x <= 0.0f) || (DockNode->Size.y <= 0.0f))
				{
					return;
				}

				ImGuiViewport* Viewport = ImGui::GetWindowViewport();

				LeftSidebarSize = { DockNode->Size.x, DockNode->Size.y };
				SidebarWindow->Pos = ImVec2(0, MenuBarSize.Y);
				SidebarWindow->Size = ImVec2(LeftSidebarSize.X, Viewport->WorkSize.y);

				DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoDockingSplit;

				/* Dock node has no other windows docked in it. */
				if (DockNode->Windows.Size <= 1)
				{
					DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
					DockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
					SidebarWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;
					if (DockNode->VisibleWindow)
					{
						DockNode->VisibleWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;
					}
				}
				else if (DockNode->Windows.Size > 1)
				{
					DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
					DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoWindowMenuButton;
					SidebarWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar; /* REMOVE ME */

					if (DockNode->VisibleWindow)
					{
						DockNode->VisibleWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;
					}
				}
			}
		}
	}

	void LEditorLayer::UI_PrepareRightSidebar() const
	{
		if (ImGuiWindow* SidebarWindow = ImGui::FindWindowByName(LK_UI_SIDEBAR_2); SidebarWindow != nullptr)
		{
			if (ImGuiDockNode* DockNode = SidebarWindow->DockNode; DockNode != nullptr)
			{
				if ((DockNode->Size.x <= 0.0f) || (DockNode->Size.y <= 0.0f))
				{
					return;
				}

				ImGuiViewport* Viewport = ImGui::GetWindowViewport();

				RightSidebarSize = { DockNode->Size.x, DockNode->Size.y };
				SidebarWindow->Pos = ImVec2(Viewport->Size.x - RightSidebarSize.X, MenuBarSize.Y);
				SidebarWindow->Size = ImVec2(RightSidebarSize.X, RightSidebarSize.Y);

				DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoDocking;
				DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoDockingSplit;

				/* Dock node has no other windows docked in it. */
				if (DockNode->Windows.Size <= 1)
				{
					DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
					DockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
					SidebarWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;

					if (DockNode->VisibleWindow)
					{
						DockNode->VisibleWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;
					}
				}
				else if (DockNode->Windows.Size > 1)
				{
					DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
					SidebarWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;

					if (DockNode->VisibleWindow)
					{
						DockNode->VisibleWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;
					}
				}
			}
		}
	}

	void LEditorLayer::UI_PrepareEditorViewport()
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByName(LK_UI_EDITOR_VIEWPORT); Window != nullptr)
		{
			if (ImGuiDockNode* DockNode = Window->DockNode; DockNode != nullptr)
			{
				if ((DockNode->Size.x <= 0.0f) || (DockNode->Size.y <= 0.0f))
				{
					return;
				}

				ImGuiViewport* Viewport = ImGui::GetWindowViewport();
				ImGuiStyle& Style = ImGui::GetStyle();

				/* Modify the size on the y-axis to account for the docking separators. */
				DockNode->Size = ImVec2(DockNode->Size.x, (DockNode->Size.y - 2 * Style.DockingSeparatorSize + TopBar.FramePadding.y));

				EditorViewport->SetSize(DockNode->Size.x, DockNode->Size.y);

				EditorViewport->SetViewportBounds(0, { DockNode->Pos.x , DockNode->Pos.y });
				EditorViewport->SetViewportBounds(1, { DockNode->Pos.x + DockNode->Size.x, DockNode->Pos.y + DockNode->Size.y });

				Window->Flags |= ImGuiWindowFlags_NoTitleBar;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoTabBar;
				DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoDocking;
			}
		}
	}

	void LEditorLayer::UI_ShowViewportAndWindowDetails()
	{
		const LVector2 WindowSize = EditorViewport->GetSize();
		const LVector2 WindowScalers = EditorViewport->GetScalers();

		ImGui::BeginGroup();
		{
			ImGui::Text("Window Info");
			ImGui::Text("Window Size: (%1.f, %1.f)", (float)Window->GetWidth(), (float)Window->GetHeight());

			ImGui::Text("Viewport Window Size: (%1.f, %1.f)", PrimaryViewportBounds[1].X - PrimaryViewportBounds[0].X,
						PrimaryViewportBounds[1].Y - PrimaryViewportBounds[0].Y);

			ImGui::Text("EditorLayer Window Size: (%1.f, %1.f)", WindowSize.X, WindowSize.Y);
			ImGui::Text("Center Window (%1.f, %1.f)", WindowSize.X, WindowSize.Y);
			ImGui::SameLine(0, 20);
			ImGui::Text("Scaled Res (%.1f, %.1f)", (WindowSize.X / WindowScalers.X), (WindowSize.Y / WindowScalers.X));

			if (EditorCamera->bIsActive)
			{
				const glm::vec2 CameraPos = EditorCamera->GetPosition();
				ImGui::Text("EditorCamera Position: (%.1f, %.1f)", CameraPos.x, CameraPos.y);
			}
		}
		ImGui::EndGroup();
	}

	void LEditorLayer::UI_AboutPopup()
	{
		static const std::string EngineVersion = LVersion::ToString(LK_ENGINE_VERSION);
		UI::ShowMessageBox("About", []()
		{
			/* Section: Title. */
			UI::Font::Push("Large");
			ImGui::Text("LkEngine %s", EngineVersion.c_str());
			UI::Font::Pop();

			/* Section: About description */
			ImGui::Separator();
			ImGui::TextWrapped("Game and render engine written in C++20");
			ImGui::Separator();

			/* Section: Written By */
			UI::Font::Push("Bold");
			ImGui::Text("Developed by");
			UI::Font::Pop();
			ImGui::BulletText("Lukas Gunnarsson (Lukkelele)");
			ImGui::Separator();

			/* Section: Dependencies */
			UI::Font::Push("Italic");
			ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.50f, 1.0f), "Contains source code provided by");
			UI::Font::Pop();
			{
				ImGui::BulletText("GLFW: %d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
			#if defined(LK_ENGINE_OPENGL)
				ImGui::BulletText("OpenGL: %d.%d", LOpenGL::GetMajorVersion(), LOpenGL::GetMinorVersion());
			#endif
				ImGui::BulletText("ImGui: %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
				ImGui::BulletText("ENTT: %s", ENTT_VERSION);
				ImGui::BulletText("nativefiledialog: %s", "1.2.1"); /* TODO: Get version from library, currently no way to do that(?). */
				ImGui::BulletText("yaml-cpp: %s", "0.6.0");         /* TODO: Get version from library, currently no way to do that(?). */
			}

			ImGui::Separator();
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
		});
	}

	void LEditorLayer::UI_LoadAutoSavePopup()
	{
		UI::ShowMessageBox("LoadAutoSave", [this]()
		{
			ImGui::Text("There is a newer auto-saved file for the scene.");
			ImGui::Text("Do you wish to load it instead?");

			if (ImGui::Button("Yes"))
			{
				OpenScene(AutoSaveData.FilePathAuto, false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				OpenScene(AutoSaveData.FilePath, false);
				ImGui::CloseCurrentPopup();
			}

		}, 440);
	}

	void LEditorLayer::UI_NewProjectPopup()
	{
		static constexpr uint16_t PopupWidth = 520;
		UI::ShowMessageBox("New Project", [&]()
		{
			UI::FScopedStyle FramePadding(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
			UI::FScopedStyle FrameRounding(ImGuiStyleVar_FrameRounding, 3.0f);

			UI::Font::Push("Bold");
			std::string FullProjectPath;
			if (std::strlen(InputBuffer::NewProjectFilePath) > 0)
			{
				FullProjectPath = std::string(InputBuffer::NewProjectFilePath) + "/" + std::string(InputBuffer::ProjectName);
			}

			LFileSystem::ConvertToPlatformPath(FullProjectPath);
			ImGui::Text("Project: %s", FullProjectPath.c_str());
			UI::Font::Pop();

			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##NewProjectName", "Project Name", InputBuffer::ProjectName, PROJECT_NAME_LENGTH_MAX);

			ImGuiStyle& Style = ImGui::GetStyle();
			const ImVec2 LabelSize = ImGui::CalcTextSize("...", nullptr, true);
			const ImVec2 ButtonSize = ImGui::CalcItemSize(
				ImVec2(0, 0), 
				(LabelSize.x + Style.FramePadding.x * 2.0f), 
				(LabelSize.y + Style.FramePadding.y * 2.0f)
			);

			ImGui::SetNextItemWidth((PopupWidth - 10) - ButtonSize.x - Style.FramePadding.x * 2.0f - Style.ItemInnerSpacing.x - 1);
			ImGui::InputTextWithHint("##NewProjectLocation", "Location", InputBuffer::NewProjectFilePath, PROJECT_NAME_LENGTH_MAX);

			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				std::string ProjLocation = LFileSystem::OpenFolderDialog().string();
				LFileSystem::ConvertToPlatformPath(ProjLocation);
				std::memcpy(InputBuffer::NewProjectFilePath, ProjLocation.data(), ProjLocation.length());
			}

			UI::Font::Push("Bold");
			if (ImGui::Button("Create"))
			{
				CreateProject(FullProjectPath);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				LK_CORE_CONSOLE_INFO("Cancelled creation of new project");
				std::memset(InputBuffer::NewProjectFilePath, 0, PROJECT_NAME_LENGTH_MAX);
				ImGui::CloseCurrentPopup();
			}
			UI::Font::Pop();

		}, (uint32_t)EMessageBoxFlag::AutoSize, PopupWidth);
	}

	void LEditorLayer::UI_SyncEditorWindowSizes(const LVector2& InViewportSize)
	{
		//EditorViewport->SetSizeY(InViewportSize.Y - BottomBarSize.Y + TopBarSize.Y);

		/**
		 * Calculate the bottombar size by using the viewport size 
		 * and subtracting the editor viewport from it.
		 */
		//BottomBarSize.Y = InViewportSize.Y - EditorViewport->GetSize().Y;
		//LK_CORE_ASSERT(BottomBarSize.Y >= 0.0f, "BottomBarSize.Y is negative: {} ({} - {})", BottomBarSize.Y, InViewportSize.Y, EditorViewport->GetSize().Y);

		LeftSidebarSize.Y = InViewportSize.Y;
		RightSidebarSize.Y = InViewportSize.Y;
	}

	/* TODO: Move elsewhere. */
	void LEditorLayer::UI_ClearColorModificationMenu()
	{
		static constexpr ImGuiSliderFlags BackgroundSliderFlags = ImGuiSliderFlags_None;
		auto& Colors = ImGui::GetStyle().Colors;
		ImGui::BeginGroup();
		{
			ImGui::PushID("##ClearColorsModification");
			ImGui::Text("Clear Color");
			ImGui::Indent();
			ImGui::SliderFloat("##x", &LRenderer::ClearColor.x, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("##y", &LRenderer::ClearColor.y, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("##z", &LRenderer::ClearColor.z, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("##w", &LRenderer::ClearColor.w, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("UI Alpha", &Colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f, " %.2f", BackgroundSliderFlags);
			ImGui::Unindent();
			ImGui::PopID();
		}
		ImGui::EndGroup();
	}

	bool LEditorLayer::OpenScene()
	{
		static constexpr bool CheckForAutosaves = true;
		const std::filesystem::path Filepath = LFileSystem::OpenFileDialog({ { "LkEngine Scene", "lscene" } });
		if (!Filepath.empty())
		{
			return OpenScene(Filepath, CheckForAutosaves);
		}

		return false;
	}

	bool LEditorLayer::OpenScene(const std::filesystem::path& Filepath, const bool CheckForAutosaves)
	{
		LK_CORE_DEBUG_TAG("Editor", "Opening scene '{}'", Filepath.filename().string());
		if (!LFileSystem::Exists(Filepath))
		{
			LK_CORE_ERROR_TAG("Editor", "Tried loading a non-existing scene: {}", Filepath.string());
			return false;
		}

		if (CheckForAutosaves)
		{
			std::filesystem::path FilepathAuto = Filepath;
			FilepathAuto += ".auto";
			if (LFileSystem::Exists(FilepathAuto) && LFileSystem::IsNewer(FilepathAuto, Filepath))
			{
				AutoSaveData.FilePath = Filepath.string();
				AutoSaveData.FilePathAuto = FilepathAuto.string();
				UI_LoadAutoSavePopup();
				return false;
			}
		}

		if (SceneState == ESceneState::Play)
		{
			OnSceneStop();
		}

		TObjectPtr<LScene> NewScene = TObjectPtr<LScene>::Create("New Scene", true);
		LSceneSerializer Serializer(NewScene);
		Serializer.Deserialize(Filepath);

		EditorScene = NewScene;
		SceneFilePath = Filepath.string();

		std::replace(SceneFilePath.begin(), SceneFilePath.end(), '\\', '/');
		if ((SceneFilePath.size() >= 5) && (SceneFilePath.substr(SceneFilePath.size() - 5) == ".auto"))
		{
			SceneFilePath = SceneFilePath.substr(0, SceneFilePath.size() - 5);
		}

		UpdateWindowTitle(Filepath.filename().string());

		LSelectionContext::DeselectAll();

		CurrentScene = EditorScene;
		if (ViewportRenderer)
		{
			ViewportRenderer->SetScene(CurrentScene);
		}

		return true;
	}

	void LEditorLayer::SetScene(TObjectPtr<LScene> InScene)
	{
		LK_CORE_VERIFY(InScene);
		if (InScene && InScene->bEditorScene)
		{
			EditorScene = InScene;
			CurrentScene = EditorScene;
			LK_CORE_DEBUG_TAG("Editor", "Set editor scene: {}", InScene->GetName());
		}
	}

	void LEditorLayer::UI_HandleDragAndDrop()
	{
		/* TODO: Handle drag'n'drop (asset loading) */
	}

	void LEditorLayer::UI_OpenGLExtensions()
	{
		UI::ShowMessageBox("Supported Extensions", []()
		{
			/* Section: Title. */
			UI::Font::Push("Large");
			ImGui::Text("OpenGL Extensions");
			UI::Font::Pop();

			ImGui::Separator();
			ImGui::TextWrapped("Extensions");
			ImGui::Separator();

			int Index = 1;
			for (const auto& Extension : LOpenGL::GetSupportedExtensions())
			{
				ImGui::Text("%d: %s", Index++, Extension.c_str());
			}

			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
		});
	}

	void LEditorLayer::UI_MainMenuBar()
	{
		namespace fs = std::filesystem;
		if (ImGui::BeginMainMenuBar())
		{
			MenuBarSize.X = ImGui::GetCurrentWindow()->Size.x;
			MenuBarSize.Y = ImGui::GetFrameHeight();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Create project"))
				{
					LK_CORE_INFO_TAG("Editor", "Menu -> Create new project");
					UI_NewProjectPopup();
				}

				if (ImGui::MenuItem("Open project")) 
				{
					LK_CORE_INFO_TAG("Editor", "Menu -> Open project");
					LK_CORE_WARN("TODO");
				}

				ImGui::EndMenu(); /* File. */
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Settings"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Clicked -> Edit/Settings");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::EditorSettings))
					{
						/* TODO: Assert the panel class type before invoking TObjectPtr::As */
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LEditorSettingsPanel>()->bWindow_CoreSettings = true;
					}
				}

				ImGui::EndMenu(); /* Edit. */
			}

			if (ImGui::BeginMenu("Project"))
			{
				/* New project. */
				if (ImGui::MenuItem("New"))
				{
				}

				/* Save project. */
				if (ImGui::MenuItem("Save"))
				{
					if (Project)
					{
						LK_CORE_DEBUG_TAG("Editor", "Saving project");
						Project->Save();
					}
				}

				/* Load existing project. */
				if (ImGui::MenuItem("Load"))
				{
					const fs::path Filepath = LFileSystem::OpenFileDialog({ { "LkEngine Project", "lkproj" } });
				}

				ImGui::EndMenu(); /* Project. */
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					if (ImGui::MenuItem("Content Browser"))
					{
						if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::ContentBrowser))
						{
							PanelData->bIsOpen = true;
						}
					}
					if (ImGui::MenuItem("Scene Manager"))
					{
						if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::SceneManager))
						{
							PanelData->bIsOpen = true;
						}
					}
				
					/* Re-open all essential editor windows. */
					if (ImGui::MenuItem("Restore default windows"))
					{
						if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::ContentBrowser))
						{
							PanelData->bIsOpen = true;
						}
						if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::SceneManager))
						{
							PanelData->bIsOpen = true;
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Renderer Settings"))
				{
					bWindow_RenderSettingsWindow = !bWindow_RenderSettingsWindow;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("Material Editor"))
				{
					LAssetEditorManager::SetEditorOpen(EAssetType::Material, true);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("New"))
				{
					LK_CORE_DEBUG("Creating new scene");
					fs::path Filepath = LFileSystem::SaveFileDialog({ { "LkEngine Scene", "lkscene" } });
					if (Filepath.empty())
					{
						return;
					}

					if (!Filepath.has_extension())
					{
						Filepath += LScene::FILE_EXTENSION;
					}

					LK_CORE_DEBUG("New scene location: {}", Filepath.string());

				}
				if (ImGui::MenuItem("Load"))
				{
					static constexpr bool IsEditorScene = true;

					LK_CORE_DEBUG_TAG("Editor", "Loading scene");
					const fs::path Filepath = LFileSystem::OpenFileDialog({ { "LkEngine Scene", "lkscene" } });

					/* Prompt file dialog. */
					LK_CORE_ASSERT(false, "FIX LOADING OF SCENE");
					TObjectPtr<LScene> NewScene = TObjectPtr<LScene>::Create("", IsEditorScene);
					LSceneSerializer SceneSerializer(NewScene);
					SceneSerializer.Deserialize("scene.lukkelele");

					NewScene->CopyTo(EditorScene);
				}
				if (ImGui::MenuItem("Save"))
				{
					LSceneSerializer Serializer(EditorScene);
					Serializer.Serialize("scene.lukkelele");
				}

				ImGui::EndMenu(); // Scene.
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Metric Tool"))
				{
					bWindow_MetricTool = true;
				}

				if (ImGui::MenuItem("ID Stack Tool"))
				{
					bWindow_IdStackTool = true;
				}

				if (ImGui::MenuItem("Style Editor"))
				{
					bWindow_StyleEditor = true;
				}

				if (ImGui::MenuItem("ImGui Logging"))
				{
					bWindow_ImGuiLogWindow = true;
				}

				if (ImGui::MenuItem("Live Objects"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Clicked -> Tools/Live Objects");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						/* TODO: Assert the panel class type before invoking TObjectPtr::As */
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->Window_ObjectReferences.bOpen = true;
					}
				}

				if (ImGui::MenuItem("View Asset Registry"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Asset Registry");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						/* TODO: Assert the panel class type before invoking TObjectPtr::As */
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->Window_AssetRegistry.bOpen = true;
					}
				}

			#if 0
				if (ImGui::MenuItem("Material Editor"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Material Editor");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::MaterialEditor))
					{
						PanelData->bIsOpen = true;
					}
				}
			#endif

				if (ImGui::MenuItem("Input Info"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Input Info");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->Window_InputInfo.bOpen = true;
					}
				}

				if (ImGui::MenuItem("UI Tools"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Tools");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->Window_UserInterfaceTools.bOpen = true;
					}
				}

				if (ImGui::MenuItem("Registered Fonts"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Registered Fonts");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->Window_Fonts.bOpen = true;
					}
				}

			#if defined(LK_ENGINE_DEBUG) && defined(LK_ENGINE_OPENGL)
				if (ImGui::MenuItem("OpenGL Extensions"))
				{
					UI_OpenGLExtensions();
				}
			#endif

				if (ImGui::BeginMenu("Engine Debug"))
				{
					ImGui::Checkbox("Show window sizes on hover", &Debug::UI::bDisplayWindowSizeOnHover);
					ImGui::Checkbox("Bounding boxes on hover", &Debug::UI::bBoundingBoxesOnHover);

					if (ImGui::BeginMenu("Content Browser"))
					{
						ImGui::Checkbox("Outliner Borders", &Debug::UI::ContentBrowser::bDrawOutlinerBorders);
						/// TODO: Dropdown menu for the color of the outliner border color.
						ImGui::EndMenu(); // Content Browser.
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu(); // Engine Debug.
			}


			if (ImGui::MenuItem("About"))
			{
				UI_AboutPopup();
			}

			// Horizontal space.
			ImGui::Dummy(ImVec2(240, 0));

			if (ImGui::BeginMenu(std::string("Project: " + Project->GetName()).c_str()))
			{
				ImGui::EndMenu(); // Project + Name.
			}
		}
		ImGui::EndMainMenuBar();
	}

	void LEditorLayer::UI_ToolBar()
	{
		static constexpr float ButtonSize = 32.0f + 5.0f;
		static constexpr float EdgeOffset = 4.0f;
		static constexpr float WindowHeight = 32.0f; /* ImGui pixel limitation. */
		static constexpr float NumberOfButtons = 3.0f;
		static constexpr float BackgroundWidth = (EdgeOffset * 6.0f) + (ButtonSize * NumberOfButtons) + EdgeOffset * (NumberOfButtons - 1.0f) * 2.0f;

		auto ToolbarButton = [](const TObjectPtr<LTexture2D>& Icon, const ImColor& Tint, float PaddingY = 0.0f)
		{
			const float Height = std::min(static_cast<float>(Icon->GetHeight()), ButtonSize) - PaddingY * 2.0f;
			const float Width = ((float)Icon->GetWidth() / (float)Icon->GetHeight() * Height);
			const bool Clicked = ImGui::InvisibleButton(UI::GenerateID(), ImVec2(Width, Height));
			UI::DrawButtonImage(
				Icon,
				Tint,
				Tint,
				Tint,
				UI::RectOffset(UI::GetItemRect(), 0.0f, PaddingY)
			);

			return Clicked;
		};

		static bool ButtonClicked = false;

		uint32_t Color = 0;
		if (ButtonClicked)
		{
			Color = RGBA32::BrightGreen;
		}
		else
		{
			Color = RGBA32::Text::Normal;
		}

		TObjectPtr<LTexture2D> ButtonImage = LRenderer::GetWhiteTexture();
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x * 0.50f) - ButtonSize * 0.50f);
		if (ToolbarButton(ButtonImage, Color))
		{
			ButtonClicked = !ButtonClicked;
			LK_CORE_INFO("Clicked Button: {}", ButtonClicked ? "1" : "0");
		}
	}

	void LEditorLayer::CastRay(FRayCast& RayCast, const LEditorCamera& Camera, const float MousePosX, const float MousePosY)
	{
		//LK_CORE_DEBUG_TAG("Editor", "Casting ray (x={}, y={})   Origin: {}", MousePosX, MousePosY, LVector3(Camera.GetOrigin()).ToString());
		const glm::vec4 MouseClipPos = { MousePosX, MousePosY, -1.0f, 1.0f };
		const glm::mat4 InverseProj = glm::inverse(Camera.GetProjectionMatrix());
		const glm::vec4 Ray = InverseProj * MouseClipPos;
		const glm::mat3 InverseView = glm::inverse(glm::mat3(Camera.GetViewMatrix()));

		RayCast.Pos = Camera.GetPosition();
		RayCast.Dir = InverseView * glm::vec3(Ray);
	}

	/// REMOVE?
	LEntity LEditorLayer::CreateCube()
	{
		FAssetHandle CubeHandle = LAssetManager::GetAssetHandleFromFilePath("Assets/Meshes/Source/Cube.gltf");
		auto MeshSource = LAssetManager::GetAsset<LMeshSource>(CubeHandle);

		LEntity NewCubeEntity;

		/// TODO: Just use a search function instead of iterating through like this, or?
		std::unordered_set<FAssetHandle> AssetList = EditorScene->GetAssetList();
		for (const FAssetHandle Handle : AssetList)
		{
			auto StaticMesh = LAssetManager::GetAsset<LStaticMesh>(Handle);
			if (StaticMesh == MeshSource)
			{
			}

			auto Mesh = LAssetManager::GetAsset<LMesh>(Handle);
			if (Mesh == MeshSource)
			{
				/* The cube is in the scene. */
				std::vector<LEntity> SceneEntities = EditorScene->GetEntities();
				for (LEntity& Entity : SceneEntities)
				{
					if (Entity.HasComponent<LMeshComponent>())
					{
						LMeshComponent& MeshComp = Entity.GetMesh();
						if (MeshComp.Mesh == Handle)
						{
							NewCubeEntity = EditorScene->CreateEntity();
							EditorScene->CopyComponentIfExists<LMeshComponent>(NewCubeEntity, EditorScene->Registry, Entity);
							EditorScene->CopyComponentIfExists<LTagComponent>(NewCubeEntity, EditorScene->Registry, Entity);
							LK_CORE_VERIFY((NewCubeEntity.HasComponent<LMeshComponent>()) && (NewCubeEntity.HasComponent<LTagComponent>()));
						}
					}
				}
			}
		}

		return NewCubeEntity;
	}

	std::pair<float, float> LEditorLayer::GetMouseViewportSpace(const bool IsEditorViewport)
	{
		LK_CORE_ASSERT(EditorViewport);
		auto [MouseX, MouseY] = ImGui::GetMousePos();

		//const auto& ViewportBoundsRef = IsEditorViewport ? EditorViewport->GetViewportBounds() : PrimaryViewportBounds;
		const auto& ViewportBoundsRef = IsEditorViewport ? EditorViewport->GetViewportBounds() : PrimaryViewportBounds.data();
		MouseX -= ViewportBoundsRef[0].X;
		MouseY -= ViewportBoundsRef[0].Y;

		const float ViewportWidth = ViewportBoundsRef[1].X - ViewportBoundsRef[0].X;
		const float ViewportHeight = ViewportBoundsRef[1].Y - ViewportBoundsRef[0].Y;
	#if 0
		LK_CORE_TRACE("Mouse: ({}, {}) -> ({}, {})  Size: ({}, {})",
					 MouseX, MouseY, ((MouseX / ViewportWidth) * 2.0f - 1.0f), ((MouseY / ViewportHeight) * 2.0f - 1.0f),
					 ViewportWidth, ViewportHeight);
	#endif

		return { (MouseX / ViewportWidth) * 2.0f - 1.0f, ((MouseY / ViewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

	void LEditorLayer::UI_SetWindowFocus(const ImGuiID WindowID)
	{
		if (WindowID > 0)
		{
			if (ImGuiWindow* FocusedWindow = ImGui::FindWindowByID(WindowID); FocusedWindow != nullptr)
			{
				LK_CORE_TRACE_TAG("Editor", "Focusing: {}", FocusedWindow->ID);
				ImGui::FocusWindow(FocusedWindow);
			}
		}
	}

	void LEditorLayer::UI_CacheWindowFocus()
	{
		if (ImGuiWindow* CurrentNavWindow = GImGui->NavWindow; CurrentNavWindow != nullptr)
		{
			LK_CORE_TRACE_TAG("Editor", "Caching focused window ID: {}", CurrentNavWindow->ID);
			Cache::FocusedWindowID = CurrentNavWindow->ID;
		}
		else
		{
			Cache::FocusedWindowID = 0;
		}
	}

	void LEditorLayer::UI_RenderSettingsWindow()
	{
		if (ImGui::Begin("Render Settings", &bWindow_RenderSettingsWindow, ImGuiWindowFlags_None))
		{
			TObjectPtr<LRenderContext> RenderContext = LWindow::Get().GetRenderContext();
			bool bBlendingEnabled = RenderContext->GetBlendingEnabled();
			if (ImGui::Checkbox("Blending", &bBlendingEnabled))
			{
				RenderContext->SetBlendingEnabled(bBlendingEnabled);
			}

			const std::string CurrentRenderTopology = Enum::ToString(LRenderer::GetPrimitiveTopology());
			if (ImGui::BeginCombo("Topology", CurrentRenderTopology.c_str(), ImGuiComboFlags_None))
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
			if (ImGui::BeginCombo(
					std::format("Source: {}", Enum::ToString(RenderContext->GetSourceBlendFunction())).c_str(), 
					nullptr, 
					ImGuiComboFlags_NoPreview)
				)
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
					std::format("Destination: {}", Enum::ToString(RenderContext->GetDestinationBlendFunction())).c_str(), 
					nullptr, 
					ImGuiComboFlags_NoPreview)
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

	uint16_t LEditorLayer::RaycastScene(TObjectPtr<LScene>& TargetScene, std::vector<FSceneSelectionData>& SceneSelectionData)
	{
		LK_CORE_ASSERT(TargetScene);
		static FRayCast RayData;

		SceneSelectionData.clear();

		auto [MouseX, MouseY] = GetMouseViewportSpace(bEditorViewportHovered);
		if ((MouseX > -1.0f) && (MouseX < 1.0f) && (MouseY > -1.0f) && (MouseY < 1.0f))
		{
			const LEditorCamera& Camera = *EditorCamera;
			CastRay(RayData, Camera, MouseX, MouseY);

			/* Meshes. */
			auto MeshView = TargetScene->GetAllEntitiesWith<LMeshComponent>();
			for (auto EntityID : MeshView)
			{
				LEntity Entity = { EntityID, TargetScene };
				auto& MeshComp = Entity.GetComponent<LMeshComponent>();
				TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(MeshComp.Mesh);
				if (!Mesh || Mesh->HasFlag(EAssetFlag::Missing))
				{
					LK_CORE_ERROR_TAG("Editor", "Asset is missing for: {} (Mesh)", MeshComp.Mesh);
					continue;
				}

				std::vector<LSubmesh>& Submeshes = Mesh->GetMeshSource()->GetSubmeshes();
				LSubmesh& Submesh = Submeshes[MeshComp.SubmeshIndex];
				const glm::mat4 Transform = EditorScene->GetWorldSpaceTransform(Entity);
				const FRay Ray = {
					glm::inverse(Transform) * glm::vec4(RayData.Pos.As<glm::vec3>(), 1.0f),
					glm::inverse(glm::mat3(Transform)) * RayData.Dir.As<glm::vec3>()
				};

				float t;
				const bool RayIntersectsBoundingBox = Ray.IntersectsAABB(Submesh.BoundingBox, t);
				if (RayIntersectsBoundingBox)
				{
					const std::vector<FTriangle>& TriangleCache = Mesh->GetMeshSource()->GetTriangleCache(MeshComp.SubmeshIndex);
					for (const FTriangle& Triangle : TriangleCache)
					{
						if (Ray.IntersectsTriangle(Triangle.V0.Position, Triangle.V1.Position, Triangle.V2.Position, t))
						{
							SceneSelectionData.push_back({ Entity, &Submesh, t });
							LK_CORE_TRACE_TAG("Editor", "({}) Ray intersects: {}", SceneSelectionData.size(), Entity.Name());
							break;
						}
					}
				}
			}

			/* Static Meshes. */
			auto StaticMeshView = TargetScene->GetAllEntitiesWith<LStaticMeshComponent>();
			for (auto EntityID : StaticMeshView)
			{
				LEntity Entity = { EntityID, TargetScene };
				auto& StaticMeshComp = Entity.GetComponent<LStaticMeshComponent>();
				auto StaticMesh = LAssetManager::GetAsset<LStaticMesh>(StaticMeshComp.StaticMesh);
				if (!StaticMesh || StaticMesh->HasFlag(EAssetFlag::Missing))
				{
					LK_CORE_ERROR_TAG("Editor", "Asset is missing for: {} (StaticMesh)", StaticMeshComp.StaticMesh);
					continue;
				}

				std::vector<LSubmesh>& Submeshes = StaticMesh->GetMeshSource()->GetSubmeshes();
				for (uint32_t Idx = 0; Idx < Submeshes.size(); Idx++)
				{
					LSubmesh& Submesh = Submeshes[Idx];

					const glm::mat4 Transform = EditorScene->GetWorldSpaceTransform(Entity);
					const FRay Ray = {
						glm::inverse(Transform) * glm::vec4(RayData.Pos.As<glm::vec3>(), 1.0f),
						glm::inverse(glm::mat3(Transform)) * RayData.Dir.As<glm::vec3>()
					};

					float t;
					const bool RayIntersectsBoundingBox = Ray.IntersectsAABB(Submesh.BoundingBox, t);
					if (RayIntersectsBoundingBox)
					{
						const std::vector<FTriangle>& TriangleCache = StaticMesh->GetMeshSource()->GetTriangleCache(Idx);
						for (const FTriangle& Triangle : TriangleCache)
						{
							if (Ray.IntersectsTriangle(Triangle.V0.Position, Triangle.V1.Position, Triangle.V2.Position, t))
							{
								SceneSelectionData.push_back({ Entity, &Submesh, t });
								LK_CORE_DEBUG_TAG("Editor", "({}) Ray intersects: {}", SceneSelectionData.size(), Entity.Name());
								break;
							}
						}
					}
				}
			}

			std::sort(SceneSelectionData.begin(), SceneSelectionData.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
		}

		return static_cast<uint16_t>(SceneSelectionData.size());
	}

	void LEditorLayer::UI_WindowStatistics()
	{
		const LVector2 WindowSize = EditorViewport->GetSize();

		ImGui::BeginChild("##WindowStats", ImVec2(400, 500), false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
		{
			const float FPS = 1000.0f / LApplication::Get().GetDeltaTime();
			ImGui::Text("FPS: %1.f", FPS);

			if (EditorCamera->bIsActive)
			{
				ImGui::Text("FOV: %1.f", EditorCamera->DegPerspectiveFOV);
				const glm::vec3& CamPos = EditorCamera->GetPosition();

				ImGui::Text("Camera");
				ImGui::Indent();
				ImGui::Text("Pos (%.2f, %.2f, %.2f)", CamPos.x, CamPos.y, CamPos.z);
				ImGui::Text("Zoom: %.3f", EditorCamera->GetZoomSpeed());
				ImGui::Text("Speed: %.3f", EditorCamera->GetCameraSpeed());
				ImGui::Unindent();

				ImGui::Text("Distance: %.2f", EditorCamera->GetDistance());
				ImGui::Text("Focalpoint: (%.2f, %.2f, %.2f)", EditorCamera->GetFocalPoint().x,
							EditorCamera->GetFocalPoint().y, EditorCamera->GetFocalPoint().z);

				ImGui::Text("Mouse Button: %s", Enum::ToString(LInput::GetLastMouseButton()));

				ImGui::Text("Editor");
				ImGui::Indent();
				ImGui::Text("Window: %s", EditorViewport->GetSize().ToString().c_str());
				ImGui::Text("Window Scalers: %s", EditorViewport->GetScalers().ToString<const char*>());
				ImGui::Unindent();
			}
		}
		ImGui::EndChild();
	}

}
