#include "LKpch.h"
#include "EditorLayer.h"

#include "EditorTabManager.h"
#include "ComponentEditor.h"
#include "NodeEditor/NodeEditor.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Asset/AssetTypes.h"

#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Input/Mouse.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/Property.h"
#include "Panels/EditorSettingsPanel.h"
#include "Panels/ToolsPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneManagerPanel.h"

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
		LVector2 TopBarSize = { 0.0f, 54.0f };
		LVector2 TabBarSize = { 0.0f, 34.0f };
		LVector2 BottomBarSize = { 0.0f, 240.0f }; /// REMOVE
		LVector2 LeftSidebarSize = { 340.0f, 0.0f };
		LVector2 RightSidebarSize = { 340.0f, 0.0f };
		bool bFillSidebarsVertically = true;

		struct FTopBarData
		{
			ImVec2 FramePadding{};
			ImVec2 WindowPadding{};
		} TopBarData{};

		/* Windows. */
		bool bWindow_MetricTool = false;
		bool bWindow_IdStackTool = false;
		bool bWindow_StyleEditor = false;
		bool bWindow_ImGuiLogWindow = false;
		bool bWindow_RenderSettingsWindow = false; /* TODO: REMOVE */
		bool bRenderSkybox = true;

		/// TODO: Move
		struct FMeshSubmissionMetadata
		{
			std::string Name = "Unknown";
			std::string ShaderName{};
			std::string MaterialName{};
		};
		std::unordered_map<LUUID, FMeshSubmissionMetadata> MeshSubmissions{};

		std::vector<FSelectionData> SelectionData;
	}

	/**
	 * TODO: Move lots of the initialization code to OnAttach instead.
	 */

	LEditorLayer::LEditorLayer()
		: LLayer("EditorLayer")
		, Settings(FEditorSettings::Get())
		, TabManager(LEditorTabManager::Get())
	{
		LOBJECT_REGISTER();
		Instance = this;

		SelectionData.clear();
		CurrentWindowType = EEditorWindowType::Viewport;

		Window = &LApplication::Get()->GetWindow();
		LK_CORE_VERIFY(Window);
	}

	/**
	 * TODO: Move lots of the initialization code to OnAttach instead.
	 */
	void LEditorLayer::Initialize()
	{
		//FWindowData& WindowData = Window->GetData();

		/* Viewport framebuffer. */
		FFramebufferSpecification FramebufferSpec;
		FramebufferSpec.Attachments = { EImageFormat::RGBA32F, EImageFormat::DEPTH24STENCIL8 };
		FramebufferSpec.Samples = 1;
		FramebufferSpec.ClearColorOnLoad = false;
		FramebufferSpec.ClearColor = { 0.10f, 0.50f, 0.50f, 1.0f };
		FramebufferSpec.DebugName = "EditorLayer-Framebuffer";
		FramebufferSpec.Width = Window->GetWidth();
		FramebufferSpec.Height = Window->GetHeight();
		ViewportFramebuffer = LFramebuffer::Create(FramebufferSpec);

		/* Editor Camera. */
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
		FRenderer2DSpecification Renderer2DSpec;
		Renderer2D = TObjectPtr<LRenderer2D>::Create(Renderer2DSpec);
		Renderer2D->Initialize();

		/* FIXME: Temporarily here. */
		LOpenGL_Debug::InitializeEnvironment();

		/** Bind mouse and keyboard callbacks. */
		LMouse::OnMouseEnabled.Add(this, &LEditorLayer::OnMouseEnabled);
		LMouse::OnMouseButtonPressed.Add(this, &LEditorLayer::OnMouseButtonPressed);
		LMouse::OnMouseButtonReleased.Add(this, &LEditorLayer::OnMouseButtonReleased);
		LMouse::OnMouseScrolled.Add(this, &LEditorLayer::OnMouseScrolled);
		LMouse::OnCursorModeChanged.Add(this, &LEditorLayer::OnMouseCursorModeChanged);

		LKeyboard::OnKeyPressed.Add(this, &LEditorLayer::OnKeyPressed);
		LKeyboard::OnKeyReleased.Add(this, &LEditorLayer::OnKeyReleased);

	#if 0
		LRenderer::OnMeshSubmission.Add([&](TObjectPtr<LMesh> SubmittedMesh, TObjectPtr<LShader> UsedShader, TObjectPtr<LMaterial> Material)
		{
			LK_CORE_DEBUG_TAG("Editor", "OnMeshSubmission");
			LK_CORE_ASSERT(SubmittedMesh, "OnMeshSubmission error, mesh is not valid");
			LK_CORE_ASSERT(UsedShader, "OnMeshSubmission error, shader is not valid");
			LK_CORE_ASSERT(Material, "OnMeshSubmission error, material is not valid");
			auto& Data = MeshSubmissions[SubmittedMesh->Handle];
			Data.Name = SubmittedMesh->GetName();
			Data.ShaderName = UsedShader->GetName();
			Data.MaterialName = Material->GetName();
		});
	#endif

		UI::OnMessageBoxCancelled.Add([&](const char* WindowName)
		{
			LK_CORE_DEBUG_TAG("Editor", "Message Box Cancelled: {}", WindowName);
		});
	}

	void LEditorLayer::OnAttach()
	{
		LK_CORE_VERIFY(Window);
		LK_CORE_DEBUG_TAG("Editor", "OnAttach");

		LObject::Initialize();

		/* Setup viewport bounds and sizes of the sidebars. */
		ViewportBounds[0] = { 0, 0 };
		ViewportBounds[1] = { Window->GetViewportWidth(), Window->GetViewportHeight() };
		LeftSidebarSize.Y = ViewportBounds[1].Y;
		RightSidebarSize.Y = ViewportBounds[1].Y;

		/* Editor viewport. */
		EditorViewport = TObjectPtr<LViewport>::Create();
		EditorViewport->SetViewportBounds(0, { 0.0f, 0.0f });
		EditorViewport->SetViewportBounds(1, { Window->GetWidth(), Window->GetHeight() });
		EditorViewport->SetPosition({ LeftSidebarSize.X, BottomBarSize.Y });
		EditorViewport->SetSize({ (Window->GetWidth() - LeftSidebarSize.X, RightSidebarSize.X),
								  (Window->GetHeight() - BottomBarSize.Y) });

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
		Settings.Load();

		LK_CORE_DEBUG_TAG("Editor", "Creating scene renderer");
		FSceneRendererSpecification RendererSpec{};
		SceneRenderer = TObjectPtr<LSceneRenderer>::Create(EditorScene, RendererSpec);

		GOnSceneSetActive.Add([&](const TObjectPtr<LScene>& NewActiveScene)
		{
			LK_CORE_DEBUG_TAG("Editor", "[GOnSceneSetActive] Setting active scene to: {}", NewActiveScene->GetName());
			SetScene(NewActiveScene);
		});

		Initialize();
	}

	void LEditorLayer::OnDetach()
	{
		LK_CORE_TRACE_TAG("Editor", "OnDetach");
		PanelManager->Serialize();

		Settings.Save();

		SceneRenderer->SetScene(nullptr);
	}

	void LEditorLayer::OnUpdate(const float InDeltaTime)
	{
		LK_PROFILE_FUNC();
		DeltaTime = InDeltaTime;

		EditorViewport->Update();

		/* FIXME: Shortcuts should be handled in a separate function. */
		/* Shortcut: Item Picker (DEBUG). */
		ImGuiIO& IO = ImGui::GetIO();
		if (ImGui::IsKeyPressed(ImGuiKey_P) && IO.KeyCtrl && IO.KeyShift)
		{
			LK_CORE_INFO_TAG("Editor", "Shortcut: Debug Item");
			ImGui::DebugStartItemPicker();
		}
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

		EditorScene->OnRenderEditor(SceneRenderer, *EditorCamera, DeltaTime);

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

	void LEditorLayer::RenderViewport(TObjectPtr<LImage> Image) 
	{
		/* TODO: Remove this function (?) */
		LK_UNUSED(Image);
	}

	void LEditorLayer::OnRender()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		LRenderer::Submit([&]()
		{
			LOpenGL_Debug::RenderMirrorTexture(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LOpenGL_Debug::RenderCubes(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LOpenGL_Debug::RenderFloor(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
		});
	}

	void LEditorLayer::OnRenderUI()
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
		PanelManager->OnRenderUI();

		/*----------------------------------------------------------------------------
			                              Top Bar
		 ----------------------------------------------------------------------------*/
		UI_PrepareTopBar();
		UI::Begin(LK_UI_TOPBAR, nullptr, UI::SidebarFlags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		{
			//LK_UI_DEBUG_DOCKNODE(LK_UI_TOPBAR);
			UI_ToolBar();
		}
		UI::End();

		/*----------------------------------------------------------------------------
			                             Sidebar 1
		 ----------------------------------------------------------------------------*/
		UI_PrepareLeftSidebar();
		UI::Begin(LK_UI_SIDEBAR_1, nullptr, UI::SidebarFlags);
		{
			//LK_UI_DEBUG_DOCKNODE(LK_UI_SIDEBAR_1);

			/* Blending */
			static bool bBlendingEnabled = false;
			TObjectPtr<LRenderContext> RenderContext = LWindow::Get().GetRenderContext();
			bBlendingEnabled = RenderContext->GetBlendingEnabled();
			if (ImGui::Checkbox("Depth Testing", &bBlendingEnabled))
			{
				Window->SetDepthEnabled(bBlendingEnabled);
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
			//LK_UI_DEBUG_DOCKNODE(LK_UI_SIDEBAR_2);

			/* Window Information. */
			if (ImGui::TreeNode("Window Information"))
			{
				UI_ShowViewportAndWindowDetails();
				ImGui::TreePop();
			}

			/* Mouse Information. */
			if (ImGui::TreeNode("Mouse Information"))
			{
				UI_ShowMouseDetails();
				ImGui::TreePop();
			}

			if (ImGui::Checkbox("Render Skybox", &bRenderSkybox)) 
			{
			}

			if (ImGui::TreeNode("Config Directories"))
			{
				ImGui::Text("Assets Dir: %S", LFileSystem::GetAssetsDir().c_str());
				ImGui::Text("Engine Dir: %S", LFileSystem::GetEngineDir().c_str());
				ImGui::Text("Config Dir: %S", LFileSystem::GetConfigDir().c_str());
				ImGui::Text("Engine Config: %S", LFileSystem::GetEngineConfig().c_str());
				ImGui::TreePop(); /* Mouse Information */
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
				#if 1
					const LEntity SelectedEntity = EditorScene->TryGetEntityWithUUID(LSelectionContext::SelectedHandle);
					if (SelectedEntity)
					{
						DrawObjectGizmo(SelectedEntity);
					}
				#endif
				}
					
			#if 0
				/* FIXME: Fix this code. */
				if (LSelectionContext::SelectedHandle > 0)
				{
					const LEntity SelectedEntity = EditorScene->TryGetEntityWithUUID(LSelectionContext::SelectedHandle);
					if (SelectedEntity)
					{
						DrawObjectGizmo(SelectedEntity);
					}
				}
			#endif

				UI_WindowStatistics();

				UI_HandleDragAndDrop();
			}
			UI::End(); /* LK_UI_EDITOR_VIEWPORT */
		}
		UI::End();
		ImGui::PopStyleVar(2); /* FramePadding, WindowPadding. */

		/* Render tabs. */
		UI_TabManager();

		const LVector2 ViewportSize = { Viewport->WorkSize.x, Viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(ViewportSize);

		TabManager.End(); /// FIXME

		UI::RenderMessageBoxes();
		UI_RenderExternalWindows();

		ImGui::End(); /* Core Viewport. */
	}

	void LEditorLayer::UI_ViewportTexture()
	{
		TObjectPtr<LImage2D> ViewportImage = ViewportFramebuffer->GetImage(0);
	#if 0
		const LVector2 WindowSize(ImGui::GetWindowSize());
	#else
		const LVector2 WindowSize = EditorViewport->GetSize();
	#endif

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
			const float SnapValue = Settings.TranslationSnapValue;
			const float SnapValues[3] = { SnapValue, SnapValue, SnapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(ViewMatrix),
				glm::value_ptr(ProjectionMatrix),
				static_cast<ImGuizmo::OPERATION>(Gizmo),
				ImGuizmo::LOCAL, //ImGuizmo::WORLD, 
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

			ImGui::End(); /* Window->Name */
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

	void LEditorLayer::NewScene(const std::string& SceneName)
	{
		EditorScene = TObjectPtr<LScene>::Create(SceneName, true);
		EditorScene->SetActive(true);
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
		LK_CORE_INFO_TAG("Editor", "Loading starter project");
		if (TObjectPtr<LProject> CurrentProject = LProject::Current(); CurrentProject != nullptr)
		{
			/* TODO: Close already opened project. */
		}

		static std::string StarterProjectName = "Starter-2024";
		NewScene(StarterProjectName);

		/* 1180 */
	#if 0
		LEntity Entity_Cube1 = EditorScene->CreateEntity("Cube-1");
		auto& MeshComp1 = Entity_Cube1.AddComponent<LMeshComponent>();

		LEntity Entity_Cube2 = EditorScene->CreateEntity("Cube-2");
		auto& MeshComp2 = Entity_Cube2.AddComponent<LMeshComponent>();

		LEntity Entity_Cube3 = EditorScene->CreateEntity("Cube-3");
		auto& MeshComp3 = Entity_Cube3.AddComponent<LMeshComponent>();
	#endif

	#if 0
		LEntity Entity_StaticCube1 = EditorScene->CreateEntity("StaticCube-1");
		LStaticMeshComponent& StaticMeshComp1 = Entity_StaticCube1.AddComponent<LStaticMeshComponent>();

		LEntity Entity_StaticCube2 = EditorScene->CreateEntity("StaticCube-2");
		LStaticMeshComponent& StaticMeshComp2 = Entity_StaticCube2.AddComponent<LStaticMeshComponent>();

		LEntity Entity_StaticCube3 = EditorScene->CreateEntity("StaticCube-3");
		LStaticMeshComponent& StaticMeshComp3 = Entity_StaticCube3.AddComponent<LStaticMeshComponent>();
	#endif

		Project = TObjectPtr<LProject>::Create();
		//Project->SetName(StarterProjectName);
		Project->Load(std::format("Projects/{}", StarterProjectName)); /* TODO: Just use project name, don't pass the directory. */
		LProject::SetActive(Project);
	}

	void LEditorLayer::SaveProjectAs()
	{
	}

	void LEditorLayer::SaveSceneAs()
	{
		namespace fs = std::filesystem;
		LK_INFO("Saving Current scene");
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

	#if 0
		fs::path Path = Filepath;
		UpdateWindowTitle(Path.filename().string());
		SceneFilePath = Filepath.string();
		std::replace(SceneFilePath.begin(), SceneFilePath.end(), '\\', '/');
	#endif
	}

	void LEditorLayer::InitializePanelManager()
	{
		/* Editor UI components. */
		PanelManager = TObjectPtr<LPanelManager>::Create();

		/* Panel: Editor Settings. */
		PanelManager->AddPanel<LEditorSettingsPanel>(
			EPanelCategory::View, 
			PanelID::EditorSettings,
			EPanelInitState::Closed
		);

		/* Panel: Tools. */
		PanelManager->AddPanel<LToolsPanel>(
			EPanelCategory::View, 
			PanelID::Tools,
			EPanelInitState::Closed
		);

		/* Panel: Scene Manager. */
		PanelManager->AddPanel<LSceneManagerPanel>(
			EPanelCategory::View, 
			PanelID::SceneManager, 
			"Scene Manager", 
			EPanelInitState::Open, 
			EditorScene
		);

		/* TODO: Add callbacks/delegates to content browser instance. */
		TObjectPtr<LContentBrowserPanel> ContentBrowser = PanelManager->AddPanel<LContentBrowserPanel>(
			EPanelCategory::View,
			PanelID::ContentBrowser,
			"Content Browser",
			EPanelInitState::Open
		);

		PanelManager->Deserialize();

		PanelManager->Initialize();
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

		ViewportBounds[0] = { 0, 0 };
		ViewportBounds[1] = { NewWidth, NewHeight };
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
						/// TODO
						LK_CORE_DEBUG_TAG("Editor", "Focusing on selected entity");
						break;
				}
			}

			switch (KeyData.Key)
			{
				case EKey::Escape:
					LK_CORE_DEBUG_TAG("Editor", "Pressed Escape - Deselecting");
					LSelectionContext::DeselectAll();
					break;
			}
		}
	}

	void LEditorLayer::OnKeyReleased(const FKeyData& KeyData)
	{
		LK_CORE_TRACE_TAG("Editor", "OnKeyReleased: {}", Enum::ToString(KeyData.Key));
	}

	void LEditorLayer::OnMouseEnabled(const bool Enabled)
	{
		LK_CORE_DEBUG_TAG("Editor", "OnMouseEnabled: {}", Enabled);

		static ImGuiID FocusedWindowID = 0;

		if (Enabled)
		{
			if (FocusedWindowID > 0)
			{
				if (ImGuiWindow* FocusedWindow = ImGui::FindWindowByID(FocusedWindowID); FocusedWindow != nullptr)
				{
					LK_CORE_DEBUG_TAG("Editor", "Focusing: {}", FocusedWindow->ID);
					ImGui::FocusWindow(FocusedWindow);
				}
			}
		}
		/* Cache the window focus. */
		else
		{
			ImGuiWindow* CurrentNavWindow = GImGui->NavWindow;

		#if 0
			/* Find the root window. */
			ImGuiWindow* LastWindow = nullptr;
			while (LastWindow != CurrentNavWindow)
			{
				LastWindow = CurrentNavWindow;
				CurrentNavWindow = CurrentNavWindow->RootWindow;
			}
		#endif

			if (CurrentNavWindow)
			{
				LK_CORE_DEBUG_TAG("Editor", "Caching focused window ID: {}", CurrentNavWindow->ID);
				FocusedWindowID = CurrentNavWindow->ID;
			}
			else
			{
				FocusedWindowID = 0;
			}
		}

		LK_CORE_DEBUG_TAG("Editor", "OnMouseEnabled  FocusedWindowID: {}", FocusedWindowID);
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
			LK_CORE_DEBUG_TAG("Editor", "MouseButtonPressed: '{}', ImGuizmo is active, returning", Enum::ToString(ButtonData.Button));
			return;
		}

		RaycastScene(SelectionData);

		const bool CtrlDown = LInput::IsKeyDown(EKey::LeftControl) || LInput::IsKeyDown(EKey::RightControl);
		const bool ShiftDown = LInput::IsKeyDown(EKey::LeftShift) || LInput::IsKeyDown(EKey::RightShift);

		if (!CtrlDown)
		{
			LK_CORE_DEBUG_TAG("Editor", "OnMouseButtonPressed, deselecting entities");
			LSelectionContext::DeselectAll();
		}

		if (!SelectionData.empty())
		{
			LEntity Entity = SelectionData.front().Entity;
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
				LK_CORE_DEBUG_TAG("Editor", "Selecting {} ({})", Entity.Name(), Entity.GetUUID());
				LSelectionContext::Select(ESelectionContext::Scene, Entity.GetUUID());
			}
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
		LK_CORE_DEBUG_TAG("Editor", "Cursor Mode: {}", Enum::ToString(CursorMode));
	}

	void LEditorLayer::UI_PrepareTopBar()
	{
		if (ImGuiWindow* TopBar = ImGui::FindWindowByName(LK_UI_TOPBAR); TopBar != nullptr)
		{
			TopBar->Flags |= ImGuiWindowFlags_NoTitleBar;
			if (ImGuiDockNode* DockNode = TopBar->DockNode; DockNode != nullptr)
			{
				TopBarSize.X = DockNode->Size.x;
				DockNode->Size.y = TopBarSize.Y;
				DockNode->SizeRef.y = DockNode->Size.y;

				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoResize;
			}

			ImGuiStyle& Style = ImGui::GetStyle();
			TopBarData.FramePadding = Style.FramePadding;
			TopBarData.WindowPadding = TopBar->WindowPadding;
		}
	}

	void LEditorLayer::UI_PrepareLeftSidebar() const
	{
		if (ImGuiWindow* SidebarWindow = ImGui::FindWindowByName(LK_UI_SIDEBAR_1); SidebarWindow != nullptr)
		{
			if (ImGuiDockNode* DockNode = SidebarWindow->DockNode; DockNode != nullptr)
			{
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
				ImGuiViewport* Viewport = ImGui::GetWindowViewport();
				ImGuiStyle& Style = ImGui::GetStyle();

				DockNode->Size = ImVec2(DockNode->Size.x,
										DockNode->Size.y - 2 * Style.DockingSeparatorSize + TopBarData.FramePadding.y);

				EditorViewport->SetSize(DockNode->Size.x, DockNode->Size.y);

				/* TODO: Add Style.DockingSeparatorSize here for the position? */
				EditorViewport->SetPositionX(EditorViewport->GetSize().X);

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

			ImGui::Text("Viewport Window Size: (%1.f, %1.f)", ViewportBounds[1].X - ViewportBounds[0].X,
						ViewportBounds[1].Y - ViewportBounds[0].Y);

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

	void LEditorLayer::UI_ShowMouseDetails()
	{
		const LVector2 WindowSize = EditorViewport->GetSize();
		const LVector2 WindowScalers = EditorViewport->GetScalers();

		ImGui::BeginGroup();
		{
			ImGui::SeparatorText("Mouse Info");
			ImGui::Text("Raw Pos (%1.f, %1.f)", LMouse::Pos.x, LMouse::Pos.y);
			ImGui::Text("Scaled Pos (%.1f, %.1f)", LMouse::ScaledPos.x, LMouse::ScaledPos.y);
			ImGui::Text("Center Normalized (%.2f, %.2f)", LMouse::CenterPos.x, LMouse::CenterPos.y);
			ImGui::Text("Center Scaled (%.2f, %.2f)", ((LMouse::CenterPos.x * WindowSize.X * 0.50f) / ViewportScalers.x),
						((LMouse::CenterPos.y * WindowSize.Y * 0.50f) / ViewportScalers.y));
			ImGui::Text("Mouse Scalers (%.2f, %.2f)", ViewportScalers.x, ViewportScalers.y);
			ImGui::Separator();
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
			ImGui::TextWrapped("Game and rendering engine written in C++20");
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

	void LEditorLayer::UI_SyncEditorWindowSizes(const LVector2& InViewportSize)
	{
		/* Only take the size of the TabBar into account if any tabs exist. */
		if (TabManager.GetTabCount() == 1)
		{
			EditorViewport->SetSizeY(InViewportSize.Y - BottomBarSize.Y);
		}
		else
		{
			EditorViewport->SetSizeY(InViewportSize.Y - BottomBarSize.Y + TabBarSize.Y);
		}

		if (bFillSidebarsVertically)
		{
			LeftSidebarSize.Y = InViewportSize.Y;
			RightSidebarSize.Y = InViewportSize.Y;
		}
	}

	/// TODO: Move elsewhere.
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

	void LEditorLayer::SetScene(TObjectPtr<LScene> InScene)
	{
		LK_CORE_VERIFY(InScene, "Invalid scene reference");
		if (InScene && InScene->bIsEditorScene)
		{
			EditorScene = InScene;
		}
	}

	void LEditorLayer::UI_TabManager()
	{
		static int LastTabCount = 0;
		const int CurrentTabCount = TabManager.GetTabCount();

		ImGuiViewport* Viewport = ImGui::GetWindowViewport();
		const LVector2 WindowSize = EditorViewport->GetSize();

		if (CurrentTabCount > 1)
		{
			/* Update the tab bar height as it might have changed. */
			ImGui::SetNextWindowPos({ LeftSidebarSize.X, MenuBarSize.Y }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ Viewport->WorkSize.x, TabBarSize.Y }, ImGuiCond_Always);
			ImGui::Begin("##LkTabBar", nullptr, UI::TabBarFlags);
			{
				if (ImGui::BeginTabBar("MainTab", ImGuiTabBarFlags_Reorderable))
				{
					for (const TabEntry& Entry : TabManager.Tabs)
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
									if (Tab.GetTabType() != ETabType::Viewport)
									{
										TabManager.CloseTab(Entry.second);
									}
								}
							}

							if (Tab.Closed == false)
							{
								TabManager.SetActiveTab(Entry.second);
							}

							if ((Tab.GetTabType() == ETabType::Viewport) || (Tab.Closed == true))
							{
								ImGui::EndTabItem();
								continue;
							}

							ImGui::SetNextWindowSize({ WindowSize.X, WindowSize.Y }, ImGuiCond_Always);
							ImGui::Begin("##TabWindow", NULL,
										 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
											 | ImGuiWindowFlags_NoResize);
							/// FIXME
							Entry.second->OnImGuiRender();
							ImGui::End();

							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}
			}
			ImGui::End(); /* TabBar. */
		}
		/* No tabs. */
		else
		{
		}

		LastTabCount = TabManager.GetTabCount();
	}

	void LEditorLayer::UI_HandleDragAndDrop()
	{
		/* TODO: Handle drag'n'drop (e.g asset loading) */
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
					LK_CORE_INFO_TAG("Editor", "Creating new project");
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

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
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
					
					// Re-open all essential editor windows.
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

				ImGui::EndMenu(); // View.
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
					LK_CORE_DEBUG_TAG("Editor", "Open -> UI Tools");
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
		static constexpr float WindowHeight = 32.0f; // ImGui pixel limitation.
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

		TObjectPtr<LTexture2D> ButtonImage = LTextureLibrary::Get().GetWhiteTexture();
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x * 0.50f) - ButtonSize * 0.50f);
		if (ToolbarButton(ButtonImage, Color))
		{
			ButtonClicked = !ButtonClicked;
			LK_CORE_INFO("Clicked Button: {}", ButtonClicked ? "1" : "0");
		}
	}

	void LEditorLayer::CastRay(FRayCast& RayCast, const LEditorCamera& Camera, const float MousePosX, const float MousePosY)
	{
		const glm::vec4 MouseClipPos = { MousePosX, MousePosY, -1.0f, 1.0f };
		const glm::mat4 InverseProj = glm::inverse(Camera.GetProjectionMatrix());
		const glm::vec4 Ray = InverseProj * MouseClipPos;
		const glm::mat3 InverseView = glm::inverse(glm::mat3(Camera.GetViewMatrix()));

	#if 0
		RayCast.Pos = LVector(Camera.GetPosition());
		RayCast.Dir = LVector(InverseView * glm::vec3(Ray));
	#else
		RayCast.Pos = Camera.GetPosition();
		RayCast.Dir = InverseView * glm::vec3(Ray);
	#endif
	}

	/// REMOVE?
	LEntity LEditorLayer::CreateCube()
	{
		LUUID CubeHandle = LAssetManager::GetAssetHandleFromFilePath("Assets/Meshes/Source/Cube.gltf");
		TObjectPtr<LMesh> CubeMesh = LAssetManager::GetAsset<LMesh>(CubeHandle);

		LEntity NewCubeEntity = EditorScene->CreateEntity();

		/// TODO: Just use a search function instead of iterating through like this
		std::unordered_set<LUUID> AssetList = EditorScene->GetAssetList();
		for (const LUUID AssetHandle : AssetList)
		{
			TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(AssetHandle);
			if (Mesh == CubeMesh)
			{
				/* The cube is in the scene. */
				std::vector<LEntity> SceneEntities = EditorScene->GetEntities();
				for (LEntity& Entity : SceneEntities)
				{
					if (Entity.HasComponent<LMeshComponent>())
					{
						LMeshComponent& EntityMesh = Entity.GetMesh();
						if (EntityMesh.Mesh == AssetHandle)
						{
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

	std::pair<float, float> LEditorLayer::GetMouseViewportSpace(const bool IsPrimaryViewport)
	{
		LK_CORE_ASSERT(EditorViewport);
		auto [MouseX, MouseY] = ImGui::GetMousePos();

		const auto& ViewportBoundsRef = IsPrimaryViewport ? ViewportBounds : EditorViewport->GetViewportBounds();
		MouseX -= ViewportBoundsRef[0].X;
		MouseY -= ViewportBoundsRef[0].Y;

		const float ViewportWidth = ViewportBoundsRef[1].X - ViewportBoundsRef[0].X;
		const float ViewportHeight = ViewportBoundsRef[1].Y - ViewportBoundsRef[0].Y;

		return { (MouseX / ViewportWidth) * 2.0f - 1.0f, ((MouseY / ViewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

	void LEditorLayer::UI_RenderSettingsWindow()
	{
		if (ImGui::Begin("Render Settings", &bWindow_RenderSettingsWindow, ImGuiWindowFlags_None))
		{
			TObjectPtr<LRenderContext> RenderContext = LWindow::Get().GetRenderContext();
			bool bBlendingEnabled = RenderContext->GetBlendingEnabled();
			if (ImGui::Checkbox("Blending", &bBlendingEnabled))
			{
			#if 0
				if (bBlendingEnabled)
				{
					RenderContext->SetBlendingEnabled(true);
				}
				else
				{
					RenderContext->SetBlendingEnabled(false);
				}
			#endif
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

	void LEditorLayer::RaycastScene(std::vector<FSelectionData>& SelectionData)
	{
		static FRayCast RayCast;

		auto [MouseX, MouseY] = GetMouseViewportSpace(bEditorViewportHovered);
		if ((MouseX > -1.0f) && (MouseX < 1.0f) && (MouseY > -1.0f) && (MouseY < 1.0f))
		{
			const LEditorCamera& Camera = *EditorCamera;

			CastRay(RayCast, Camera, MouseX, MouseY);

			/* Meshes. */
			auto MeshView = EditorScene->GetAllEntitiesWith<LMeshComponent>();
			for (auto EntityID : MeshView)
			{
				LEntity Entity = { EntityID, EditorScene };
				auto& MeshComp = Entity.GetComponent<LMeshComponent>();
				TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(MeshComp.Mesh);
				if (!Mesh || Mesh->IsFlagSet(EAssetFlag::Missing))
				{
					continue;
				}

				std::vector<LSubmesh>& Submeshes = Mesh->GetMeshSource()->GetSubmeshes();
				LSubmesh& Submesh = Submeshes[MeshComp.SubmeshIndex];
				const glm::mat4 Transform = EditorScene->GetWorldSpaceTransform(Entity);
				const FRay Ray = {
					glm::inverse(Transform) * glm::vec4(RayCast.Pos.As<glm::vec3>(), 1.0f),
					glm::inverse(glm::mat3(Transform)) * RayCast.Dir.As<glm::vec3>()
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
							SelectionData.push_back({ Entity, &Submesh, t });
							break;
						}
					}
				}
			}

			/* Static Meshes. */
			auto StaticMeshView = EditorScene->GetAllEntitiesWith<LStaticMeshComponent>();
			for (auto EntityID : StaticMeshView)
			{
				LEntity Entity = { EntityID, EditorScene };
				auto& StaticMeshComp = Entity.GetComponent<LStaticMeshComponent>();
				auto StaticMesh = LAssetManager::GetAsset<LStaticMesh>(StaticMeshComp.StaticMesh);
				if (!StaticMesh || StaticMesh->IsFlagSet(EAssetFlag::Missing))
				{
					continue;
				}

				std::vector<LSubmesh>& Submeshes = StaticMesh->GetMeshSource()->GetSubmeshes();
				for (uint32_t Idx = 0; Idx < Submeshes.size(); Idx++)
				{
					LSubmesh& Submesh = Submeshes[Idx];

					const glm::mat4 Transform = EditorScene->GetWorldSpaceTransform(Entity);
					const FRay Ray = {
						glm::inverse(Transform) * glm::vec4(RayCast.Pos.As<glm::vec3>(), 1.0f),
						glm::inverse(glm::mat3(Transform)) * RayCast.Dir.As<glm::vec3>()
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
								SelectionData.push_back({ Entity, &Submesh, t });
								break;
							}
						}
					}
				}
			}

			std::sort(SelectionData.begin(), SelectionData.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
		}
	}

	// TODO: Right alignment in the child window
	void LEditorLayer::UI_WindowStatistics()
	{
		const LVector2 WindowSize = EditorViewport->GetSize();

	#if 0
		/* No tabs are present. */
		if (TabManager.GetTabCount() == 1)
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X + WindowSize.X - StatisticsWindowSize.x * 1.0f, MenuBarSize.Y), ImGuiCond_Always);
		}
		/* Multiple tabs. */
		else
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X + WindowSize.X - StatisticsWindowSize.x, MenuBarSize.Y + TabBarSize.Y), ImGuiCond_Always);
		}
	#endif
		ImGui::BeginChild("##WindowStats", ImVec2(400, 500), false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
		{
			const float FPS = 1000.0f / LApplication::Get()->GetTimestep();
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
