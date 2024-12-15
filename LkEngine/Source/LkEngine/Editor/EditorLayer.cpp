#include "LKpch.h"
#include "EditorLayer.h"

#include "EditorTabManager.h"
#include "ComponentEditor.h"
#include "NodeEditor/NodeEditor.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Project/Project.h"

#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Input/Mouse.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/Property.h"
#include "Panels/SceneManagerPanel.h"
#include "Panels/ContentBrowser.h"
#include "Panels/ToolsPanel.h"

#if defined(LK_ENGINE_OPENGL)
#include "LkEngine/Renderer/Backend/OpenGL/OpenGLRenderer.h"
#include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"
#endif

#include <nfd.hpp>


namespace LkEngine {

	namespace 
	{
		LVector2 MenuBarSize = { 0.0f, 30.0f };
		LVector2 TopBarSize = { 0.0f, 54.0f };
		LVector2 TabBarSize = { 0.0f, 34.0f };
		LVector2 BottomBarSize = { 0.0f, 240.0f };
		LVector2 LeftSidebarSize = { 340.0f, 0.0f };
		LVector2 RightSidebarSize = { 340.0f, 0.0f };

		/* Windows. */
		bool bWindow_MetricTool = false;
		bool bWindow_StyleEditor = false;
		
		bool bRenderSkybox = true;

		float TranslationSnapValue = 0.50f;
		float RotationSnapValue = 45.0f;
		float ScaleSnapValue = 0.50f;
	}

	bool GDebug_DisplayWindowSizeOnHover = false;

	static TObjectPtr<LTexture2D> FloorTexture;
	static TObjectPtr<LTexture2D> CubeTexture;

	/* --- REMOVE ME --- */
	static FAssetHandle CubeAssetHandle;
	/* ----------------- */

	LEditorLayer::LEditorLayer()
		: LLayer("EditorLayer")
		, TabManager(LEditorTabManager::Get())
	{
		LOBJECT_REGISTER();
		Instance = this;

		m_ShowStackTool = false;
		CurrentWindowType = EEditorWindowType::Viewport;

		Window = &LApplication::Get()->GetWindow();
		LK_CORE_ASSERT(Window, "Window is nullptr");

		/* Viewport bounds. */
		ViewportBounds[0] = { 0, 0 };
		ViewportBounds[1] = { Window->GetViewportWidth(), Window->GetViewportHeight() };

		LeftSidebarSize.Y = ViewportBounds[1].Y;
		RightSidebarSize.Y = ViewportBounds[1].Y;

		/* Editor Viewport. */
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

		GOnSceneSetActive.Add([&](const TObjectPtr<LScene>& NewActiveScene)
		{
			SetScene(NewActiveScene);
		});

		/* Editor UI components. */
		PanelManager = TObjectPtr<LPanelManager>::Create();

	#if 1
		PanelManager->AddPanel<LToolsPanel>(
			EPanelCategory::View, 
			PanelID::Tools,
			EPanelInitState::NotOpen
		);
	#endif

	#if 1
		PanelManager->AddPanel<LSceneManagerPanel>(
			EPanelCategory::View, 
			PanelID::SceneManager, 
			"Scene Manager", 
			EPanelInitState::OpenByDefault, 
			EditorScene
		);
	#endif

	#if 0
		PanelManager->AddPanel<LContentBrowser>(
			EPanelCategory::View,
			PanelID::ContentBrowser,
			"Content Browser",
			EPanelInitState::OpenByDefault
		);
	#endif

	#if 0
		PanelManager->AddPanel<LComponentEditor>(
			EPanelCategory::View, 
			PanelID::ComponentEditor, 
			"Component Editor"
		);
	#endif

		/* TODO: Add static check in delegate implementation so no empty functions are able to get passed. */
		Window->OnWindowSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
		});

		Window->OnViewportSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			/* Update scalers. */
			const LVector2 EditorWindowSize = EditorViewport->GetSize();
			LVector2 WindowScalers;
			WindowScalers.X = EditorWindowSize.X / NewWidth;
			WindowScalers.Y = EditorWindowSize.Y / NewHeight;
			EditorViewport->SetScalers(WindowScalers);

			ViewportBounds[0] = { 0, 0 };
			ViewportBounds[1] = { NewWidth, NewHeight };
		});
	}

	void LEditorLayer::Initialize()
	{
		LObject::Initialize();

		/* Attach to LWindow delegates. */
		LK_CORE_ASSERT(Window, "Window reference is nullptr");
		FWindowData& WindowData = Window->GetWindowData();

		/* Mouse button pressed. */
		WindowData.OnMouseButtonPressed.Add([&](const FMouseButtonData& MouseButtonData)
		{
			LK_CORE_TRACE_TAG("Editor", "MouseButtonPressed: {}", static_cast<int>(MouseButtonData.Button));
		});

		/* Mouse button released. */
		WindowData.OnMouseButtonReleased.Add([&](const FMouseButtonData& MouseButtonData)
		{
			LK_CORE_TRACE_TAG("Editor", "MouseButtonReleased: {}", static_cast<int>(MouseButtonData.Button));
		});

		/* Mouse scroll. */
		WindowData.OnMouseScrolled.Add([&](const EMouseScroll MouseScroll)
		{
			LK_CORE_TRACE_TAG("Editor", "Mouse Scroll: {}", Enum::ToString(MouseScroll));
			if (EditorCamera)
			{
				if (MouseScroll == EMouseScroll::Up)
				{
					EditorCamera->MouseZoom(-0.01f);
				}
				else if (MouseScroll == EMouseScroll::Down)
				{
					EditorCamera->MouseZoom(0.01f);
				}
			}
		});

		/* Viewport Framebuffer. */
		LK_CORE_TRACE_TAG("Editor", "Creating viewport framebuffer");
		FFramebufferSpecification FramebufferSpec;
		FramebufferSpec.Attachments = { EImageFormat::RGBA32F, EImageFormat::DEPTH24STENCIL8 };
		FramebufferSpec.Samples = 1;
		FramebufferSpec.ClearColorOnLoad = false;
		FramebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		FramebufferSpec.DebugName = "EditorLayer-Framebuffer";
		FramebufferSpec.Width = Window->GetWidth();
		FramebufferSpec.Height = Window->GetHeight();
		ViewportFramebuffer = LFramebuffer::Create(FramebufferSpec);

		/* Editor Camera. */
		EditorCamera = TObjectPtr<LEditorCamera>::Create(60.0f,				  /* FOV    */
														 Window->GetWidth(),  /* Width  */
														 Window->GetHeight(), /* Height */
														 0.10f,				  /* ZNear  */
														 2400.0f);			  /* ZFar   */
		EditorCamera->Initialize();
		EditorCamera->SetPosition({ -10, 4, -10 });

		/* Attach editor viewport delegates to the editor camera. */
		EditorViewport->OnSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			EditorCamera->SetViewportSize(NewWidth, NewHeight);

			/* Set scalers for the editor viewport. */
			LVector2 EditorWindowScalers;
			EditorWindowScalers.X = static_cast<float>(NewWidth) / Window->GetWidth();
			EditorWindowScalers.Y = static_cast<float>(NewHeight) / Window->GetHeight();
			EditorViewport->SetScalers(EditorWindowScalers);
		});

		PanelManager->Initialize();

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

		/* FIXME: Temporary debugging. */
		LOpenGL_Debug::InitializeEnvironment();

		/* Keyboard events. */
		LKeyboard::OnKeyPressed.Add([&](const FKeyData& PressedKeyData)
		{
			LK_CORE_DEBUG_TAG("Editor", "Pressed: {} ({})", Enum::ToString(PressedKeyData.Key), Enum::ToString(PressedKeyData.State));
		});

		LKeyboard::OnKeyReleased.Add([&](const FKeyData& ReleasedKeyData)
		{
			LK_CORE_DEBUG_TAG("Editor", "Released: {} ({})", Enum::ToString(ReleasedKeyData.Key), Enum::ToString(ReleasedKeyData.State));
		});

		bInitialized = true;
	}

	void LEditorLayer::OnAttach()
	{
		LK_CORE_WARN_TAG("Editor", "OnAttach");
		PanelManager->Deserialize();
	}

	void LEditorLayer::OnDetach()
	{
		LK_CORE_WARN_TAG("Editor", "OnDetach");
		PanelManager->Serialize();
	}

	void LEditorLayer::OnUpdate(const float DeltaTime)
	{
		LK_PROFILE_FUNC();

		EditorViewport->Update();

		if (EditorCamera)
		{
			EditorCamera->OnUpdate(DeltaTime);
		}
	}

	void LEditorLayer::RenderViewport()
	{
		/* Render floor. */
		LRenderer::Submit([&]()
		{
			LRenderer::GetViewportFramebuffer()->Bind();
			LOpenGL_Debug::RenderFloor(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LFramebuffer::TargetSwapChain();
		});

		LRenderer::Submit([&]()
		{
			LRenderer::GetViewportFramebuffer()->Bind();
			LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);

			const glm::mat4 ProjectionMatrix = EditorCamera->GetProjectionMatrix();
			const glm::mat4 ViewMatrix = glm::mat4(glm::mat3(EditorCamera->GetViewMatrix()));

			if (EditorScene)
			{
				for (LEntity Entity : EditorScene->GetEntities())
				{
					if (!Entity.HasComponent<LMeshComponent>())
					{
						continue;
					}

					auto& MeshComp = Entity.GetComponent<LMeshComponent>();
					//TObjectPtr<LMesh> Cube = LAssetManager::GetAsset<LMesh>(CubeAssetHandle);
					TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(MeshComp.Mesh);
					/// FIXME
					if (!Mesh)
					{
						continue;
					}
					LK_CORE_ASSERT(Mesh, "Failed to retrieve mesh from asset ID '{}'", MeshComp.Mesh);

					TObjectPtr<LMaterialTable> MeshMaterials = Mesh->GetMaterials();
					if (MeshMaterials->HasMaterial(0))
					{
						const FAssetHandle MaterialHandle = MeshMaterials->GetMaterialHandle(0);
						TObjectPtr<LMaterial> Material = Mesh->GetMaterial(0);
					}
				}
			}

			LFramebuffer::TargetSwapChain();
		});

	#if 0
		LRenderer::Submit([&]()
		{
			LRenderer::GetViewportFramebuffer()->Bind();
			LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);

			using namespace LOpenGL_Debug;

			const glm::mat4 ProjectionMatrix = EditorCamera->GetProjectionMatrix();
			const glm::mat4 ViewMatrix = glm::mat4(glm::mat3(EditorCamera->GetViewMatrix()));

			TObjectPtr<LMesh> Cube = LAssetManager::GetAsset<LMesh>(CubeAssetHandle);
			GSelectedObject = Cube;
			TObjectPtr<LMaterialTable> MeshMaterials = Cube->GetMaterials();
			if (MeshMaterials->HasMaterial(0))
			{
				const FAssetHandle MaterialHandle = MeshMaterials->GetMaterialHandle(0);
				TObjectPtr<LMaterial> Material = Cube->GetMaterial(0);

				TObjectPtr<LTexture2D> Texture = Material->GetTexture();
				TObjectPtr<LShader> Shader = Material->GetShader();
				Shader->Bind();

				glm::mat4 ModelPosition = glm::translate(glm::mat4(1.0f), glm::vec3(4, 0.50, 1));
				Shader->Set("u_Model", ModelPosition);
				Shader->Set("u_ViewProjectionMatrix", GetEditorCamera()->GetViewProjectionMatrix());
				Shader->Set("u_Texture0", 0);
				Texture->Bind(0);
				LK_CORE_ASSERT((Cube->GetIndexBuffer()->GetCount() > 0), "IndexBuffer count not valid");

				Cube->GetVertexBuffer()->Bind();
				LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, Cube->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));

				ModelPosition = glm::translate(ModelPosition, glm::vec3(-5, 0, -4));
				Shader->Set("u_Model", glm::scale(ModelPosition, glm::vec3(0.80f, 1.00f, 0.80f)));
				LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, Cube->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));

				ModelPosition = glm::translate(ModelPosition, glm::vec3(1, 0, 5));
				Shader->Set("u_Model", glm::scale(ModelPosition, glm::vec3(0.95f, 1.00f, 0.90f)));
				LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, Cube->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));

				LFramebuffer::TargetSwapChain();
			}
		});
	#endif

		/**
		 * Render the debug skybox.
		 * TODO: Move away from here.
		 */

		if (bRenderSkybox)
		{
			LRenderer::Submit([&]()
			{
				LOpenGL_Debug::RenderSkybox(EditorCamera);
			});
		}

		if (EditorScene)
		{
			EditorScene->OnRenderEditor(*EditorCamera, 0.0f /* FIXME: DeltaTime */);
		}

		Render2D();
	}

	void LEditorLayer::RenderViewport(TObjectPtr<LImage> Image) 
	{
		LK_UNUSED(Image);
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

	void LEditorLayer::OnRender()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED("IS USED");
		LRenderer::Submit([&]()
		{
			LOpenGL_Debug::RenderMirrorTexture(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LOpenGL_Debug::RenderCubes(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LOpenGL_Debug::RenderFloor(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
		});
	}

	void LEditorLayer::OnRenderUI()
	{
		LApplication* Application = LApplication::Get();

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
		-----------------------------------------------------------------------------*/
		UI_PrepareTopBar();
		UI::Begin(LK_UI_TOPBAR, nullptr, UI::SidebarFlags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		{
			//LK_UI_DEBUG_DOCKNODE(LK_UI_TOPBAR);
			UI_ToolBar();
		}
		UI::End();

		/*----------------------------------------------------------------------------
			                             Sidebar 1
		-----------------------------------------------------------------------------*/
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
		}
		UI::End(); /* LK_UI_SIDEBAR_1 */

		/*----------------------------------------------------------------------------
			                             Sidebar 2
		-----------------------------------------------------------------------------*/
		UI_PrepareRightSidebar();
		UI::Begin(LK_UI_SIDEBAR_2, nullptr, UI::SidebarFlags);
		{
			//LK_UI_DEBUG_DOCKNODE(LK_UI_SIDEBAR_2);

			/* Window Information. */
			if (ImGui::TreeNode("Window Information"))
			{
				UI_ShowViewportAndWindowDetails();
				ImGui::TreePop(); /* Window Information */
			}

			/* Mouse Information. */
			if (ImGui::TreeNode("Mouse Information"))
			{
				UI_ShowMouseDetails();
				ImGui::TreePop(); /* Mouse Information */
			}

			if (ImGui::Checkbox("Render Skybox", &bRenderSkybox)) 
			{
			}

			ImGui::Text("Assets Dir: %S", LFileSystem::GetAssetsDir().c_str());
		}
		UI::End(); /* LK_UI_SIDEBAR_2 */

		/*----------------------------------------------------------------------------
			                          Bottom Bar
		-----------------------------------------------------------------------------*/
		UI_PrepareBottomBar();
		UI::Begin(LK_UI_BOTTOMBAR, nullptr, UI::SidebarFlags);
		{
			//LK_UI_DEBUG_DOCKNODE(LK_UI_DOCK_BOTTOMBAR);

			static std::vector<EKey> PressedKeys;
			const std::size_t PressedKeyCount = LInput::GetPressedKeys(PressedKeys);
			ImGui::Text("Pressed Keys: %d", (int)PressedKeyCount);
			if (PressedKeyCount > 0)
			{
				if (ImGui::BeginTable("PressedKeysTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
				{
					/* Set up the columns for the table. */
					ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Repeat Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
					ImGui::TableSetupColumn("Time Held (ms)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
					ImGui::TableHeadersRow();

					for (const EKey PressedKey : PressedKeys)
					{
						using namespace std::chrono;

						const FKeyData& KeyData = LInput::GetKeyData(PressedKey);
						const auto TimeHeld = LInput::GetKeyHeldTime<milliseconds>(PressedKey);

						ImGui::TableNextRow();

						/* Column 1: Key Name */
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s", Enum::ToString(PressedKey));

						/* Column 2: Repeat Count */
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%d", KeyData.RepeatCount);

						/* Column 3: Time Held in milliseconds */
						ImGui::TableSetColumnIndex(2);
						ImGui::Text("%d", static_cast<int>(TimeHeld.count()));
					}

					ImGui::EndTable();
				}

			#if 0
				int Index = 0;
				for (const EKey PressedKey : PressedKeys)
				{
					using namespace std::chrono;
					const FKeyData& KeyData = LInput::GetKeyData(PressedKey);
					const auto TimeHeld = LInput::GetKeyHeldTime<milliseconds>(PressedKey);
					ImGui::Text("%s  Repeat: %d  (%d)", Enum::ToString(PressedKey), KeyData.RepeatCount, (int)TimeHeld.count());
					Index++;
				}
			#endif
			}
		}
		UI::End(); /* Bottom Bar. */

		/*----------------------------------------------------------------------------
									 Editor Viewport
		-----------------------------------------------------------------------------*/
		UI::Begin(LK_UI_CORE_VIEWPORT, nullptr, UI::CoreViewportFlags);
		{
			UI::FScopedStyle FramePadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			UI::FScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

			/* Multiply by the docking separator size twice to cover the editor viewport 
			 * on both the left and the right sidebar. */
			ImGui::SetNextWindowSize(ImVec2((EditorViewport->GetSize().X + 2 * Style.DockingSeparatorSize), 
											(Viewport->Size.y - (BottomBarSize.Y + MenuBarSize.Y + TopBarSize.Y))), ImGuiCond_Always);
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X, MenuBarSize.Y + TopBarSize.Y), ImGuiCond_Always);
			UI::Begin(LK_UI_EDITOR_VIEWPORT, nullptr, UI::EditorViewportFlags);
			{
				UI_ViewportTexture();

				if (LSelectionContext::SelectedEntityID > 0)
				{
					const LEntity SelectedEntity = EditorScene->TryGetEntityWithUUID(LSelectionContext::SelectedEntityID);
					if (SelectedEntity)
					{
						DrawObjectGizmo(SelectedEntity);
					}
				}

				UI_WindowStatistics();
			}
			UI::End(); /* LK_UI_EDITOR_VIEWPORT. */
		}
		UI::End();

		/* Render tabs. */
		UI_TabManager();

		const LVector2 ViewportSize = { Viewport->WorkSize.x, Viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(ViewportSize);

		/// FIXME: What does this even do?
		TabManager.End();

		UI::RenderMessageBoxes();
		UI_RenderExternalWindows();

		ImGui::End(); /* Core Viewport. */
	}

	void LEditorLayer::UI_ViewportTexture()
	{
		TObjectPtr<LImage2D> ViewportImage = ViewportFramebuffer->GetImage(0);
		const LVector2 WindowSize(ImGui::GetWindowSize());

		UI::Image(
			ViewportImage, 
			ImVec2(WindowSize.X, WindowSize.Y + MenuBarSize.Y), 
			ImVec2(0, 1), 
			ImVec2(1, 0)
		);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("CB_PAYLOAD", ImGuiDragDropFlags_None);
			if (Payload)
			{
				const std::string PayloadDataString = std::string(static_cast<const char*>(Payload->Data));
				if (PayloadDataString == "CUBE")
				{
					CreateCube();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	void LEditorLayer::DrawObjectGizmo(LEntity Entity)
	{
		LTransformComponent& TransformComponent = Entity.Transform();
		glm::mat4 TransformMatrix = TransformComponent.GetTransform();
		const glm::vec3& CameraPos = EditorCamera->GetPosition();
		const glm::mat4& ViewMatrix = EditorCamera->GetViewMatrix();
		const glm::mat4& ProjectionMatrix = EditorCamera->GetProjectionMatrix();

		/* Viewport bounds can be indexed from 0 to 1. */
		const LVector2* ViewportBounds = EditorViewport->GetViewportBounds();
		const float PosX = ViewportBounds[0].X;
		const float PosY = ViewportBounds[0].Y;

		if (ImGuiWindow* Window = ImGui::FindWindowByName(LK_UI_EDITOR_VIEWPORT))
		{
			const LVector2 EditorWindowSize = EditorViewport->GetSize();
			ImGui::Begin(Window->Name, nullptr, UI::CoreViewportFlags | ImGuiWindowFlags_NoScrollbar);
		#if 1
			ImGuizmo::SetOrthographic(static_cast<int>(EditorCamera->GetProjectionType()));
			ImGuizmo::SetDrawlist();

			/// TODO: Do a getter here instead of calculating inline.
			if (TabManager.GetTabCount() == 1)
			{
				ImGuizmo::SetRect(PosX, (PosY - BottomBarSize.Y + MenuBarSize.Y), EditorWindowSize.X, EditorWindowSize.Y);
			}
			else
			{
				ImGuizmo::SetRect(PosX, (PosY - BottomBarSize.Y + MenuBarSize.Y + TabBarSize.Y), EditorWindowSize.X, EditorWindowSize.Y);
			}

			const bool bSnapValues = LInput::IsKeyDown(EKey::LeftControl);
			const float SnapValue = TranslationSnapValue;
			const float SnapValues[3] = { SnapValue, SnapValue, SnapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(ViewMatrix),
				glm::value_ptr(ProjectionMatrix),
				static_cast<ImGuizmo::OPERATION>(EditorCamera->GetGizmoMode()),
				ImGuizmo::LOCAL, //ImGuizmo::WORLD, 
				glm::value_ptr(TransformMatrix),
				nullptr,
				bSnapValues ? SnapValues : nullptr
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

		if (bWindow_StyleEditor)
		{
			ImGuiStyle& Style = ImGui::GetStyle();
			ImGui::Begin("Style Editor", &bWindow_StyleEditor);
			ImGui::ShowStyleEditor(&Style);
			ImGui::End();
		}

	#if 0
		if (bWindow_RegisteredFonts)
		{
			static std::vector<FFontEntry> RegisteredFonts;
			const std::size_t RegisteredFontsCount = UI::Font::GetRegistered(RegisteredFonts);

			/* Place the default font named 'Default' first. */
			std::sort(RegisteredFonts.begin(), RegisteredFonts.end(), [](const FFontEntry& A, const FFontEntry& B)
			{
				return (A.Name == "Default") || (B.Name != "Default" && A.Name < B.Name);
			});

			/* Determine the maximum width based on the longest font name. */
			auto FontEntryComparitor = [](const FFontEntry& A, const FFontEntry& B)
			{
				return A.Name.length() < B.Name.length();
			};

			/* Calculate the width dynamically based on the longest name, with some padding. */
			static constexpr float NamePadding = 12.0f;
			const auto LongestNameIt = std::max_element(RegisteredFonts.begin(), RegisteredFonts.end(), FontEntryComparitor);
			const float NameColumnWidth = ImGui::CalcTextSize(LongestNameIt->Name.c_str()).x + NamePadding;

			const std::string WindowName = std::format("Registered Fonts: {}", RegisteredFontsCount);
			ImGui::Begin(WindowName.c_str(), &bWindow_RegisteredFonts);
			if (ImGui::BeginTable("RegisteredFontsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, NameColumnWidth);
				ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 34.0f);
				ImGui::TableSetupColumn("Filepath", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();
				for (const auto& Font : RegisteredFonts)
				{
					ImGui::TableNextRow();
					/* Column 1: Font Name */
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", Font.Name.c_str());

					/* Column 2: Font Size */
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%.1f", Font.Size);

					/* Column 3: Filepath */
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("\"%s\"", Font.FilePath.c_str());
					ImGui::Text("%s (%.1f)  \"%s\"", Font.Name.c_str(), Font.Size, Font.FilePath.c_str());
				}

				ImGui::EndTable();
			}

			ImGui::End();
		}
	#endif
	}

	// TODO: Right alignment in the child window
	void LEditorLayer::UI_WindowStatistics()
	{
		const LVector2 WindowSize = EditorViewport->GetSize();

		/* Window statistics, FPS counter etc. */
		static ImVec2 StatisticsWindowSize = ImVec2(ImGui::CalcTextSize("Forward Direction: { N1, N2, N3 }").x + 200, 500);

	#if 0
		/* No tabs are present. */
		if (TabManager.GetTabCount() == 1)
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X + WindowSize.X - StatisticsWindowSize.x * 1.0f, MenuBarSize.Y),
									ImGuiCond_Always);
		}
		/* Multiple tabs. */
		else
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X + WindowSize.X - StatisticsWindowSize.x,
										   MenuBarSize.Y + TabBarSize.Y),
									ImGuiCond_Always);
		}
	#endif
		ImGui::BeginChild("##WindowStats", StatisticsWindowSize, false,
						  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
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
				// ImGui::Text("Forward Direction: %s", EditorCamera->GetForwardDirection<LVector>().ToString().c_str());

				ImGui::Text("Distance: %.2f", EditorCamera->GetDistance());
				ImGui::Text("Focalpoint: (%.2f, %.2f, %.2f)", EditorCamera->GetFocalPoint().x,
							EditorCamera->GetFocalPoint().y, EditorCamera->GetFocalPoint().z);

				// Mouse data.
				ImGui::Text("Mouse Button: %s", Enum::ToString(LInput::GetLastMouseButton()));
				ImGui::Text("Editor Window: %s", EditorViewport->GetSize().ToString().c_str());
				ImGui::Text("Editor Window Scalers: %s", EditorViewport->GetScalers().ToString<const char*>());
			}
		}
		ImGui::EndChild();
	}

	void LEditorLayer::UI_HandleManualWindowResize()
	{
	#if 0
		GLFWwindow* GlfwWindow = LApplication::Get()->GetWindow().GetGlfwWindow();
		const bool WindowMaximized = (bool)glfwGetWindowAttrib(GlfwWindow, GLFW_MAXIMIZED);
		ImVec2 NewSize, NewPosition;
		if (!WindowMaximized && UI::UpdateWindowManualResize(ImGui::GetCurrentWindow(), NewSize, NewPosition))
		{
			glfwSetWindowPos(GlfwWindow, (int)(NewPosition.x), (int)(NewPosition.y));
			glfwSetWindowSize(GlfwWindow, (int)(NewSize.X), (int)(NewSize.Y));
		}
	#endif
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

		Project = TObjectPtr<LProject>::Create();
		//Project->SetName(StarterProjectName);
		Project->Load(LK_FORMAT_STRING("Projects/{}", StarterProjectName)); /* TODO: Just use project name, don't pass the directory. */
		LProject::SetActive(Project);

	#if 0
		MeshComp1.Mesh = CubeAssetHandle;
		MeshComp2.Mesh = CubeAssetHandle;
		MeshComp3.Mesh = CubeAssetHandle;
	#endif
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

	void LEditorLayer::PollInput()
	{
		if (LInput::IsKeyPressed(EKey::LeftControl))
		{
			LK_CORE_INFO("KeyPressed: LeftControl");
		}
	}

	void LEditorLayer::UI_PrepareTopBar()
	{
		if (ImGuiWindow* TopBar = ImGui::FindWindowByName(LK_UI_TOPBAR); TopBar != nullptr)
		{
			TopBar->Flags |= ImGuiWindowFlags_NoTitleBar;
			if (ImGuiDockNode* DockNode = TopBar->DockNode; DockNode != nullptr)
			{
				TopBarSize.X = DockNode->Size.x;
				DockNode->SizeRef.y = TopBarSize.Y;
				DockNode->Size.y = TopBarSize.Y;

				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoResize;
			}
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
					DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoWindowMenuButton;
					SidebarWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;

					if (DockNode->VisibleWindow)
					{
						DockNode->VisibleWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;
					}
				}
			}
		}
	}

	void LEditorLayer::UI_PrepareBottomBar()
	{
		if (ImGuiWindow* BottomBarWindow = ImGui::FindWindowByName(LK_UI_BOTTOMBAR); BottomBarWindow != nullptr)
		{
			if (ImGuiDockNode* DockNode = BottomBarWindow->DockNode; DockNode != nullptr)
			{
				ImGuiViewport* Viewport = ImGui::GetWindowViewport();

				/** Add the docking separator size here to make the alignment linear for both sidebars. */
				ImGuiStyle& Style = ImGui::GetStyle();
				
				/* TODO: Check if any other windows are docked. */
				//BottomBarSize = { DockNode->Size.x + Style.DockingSeparatorSize, DockNode->Size.y };

				EditorViewport->SetSizeX(BottomBarSize.X - Style.DockingSeparatorSize);
				EditorViewport->SetPositionX(EditorViewport->GetPosition().X - Style.DockingSeparatorSize);

				BottomBarWindow->Pos = ImVec2(LeftSidebarSize.X, Viewport->Size.y - BottomBarSize.Y);
				BottomBarWindow->Size = ImVec2(Viewport->Size.x - (LeftSidebarSize.X + RightSidebarSize.X), BottomBarSize.Y);

				/* Remove the window titlebar if no other windows are docked in the sidebar. */
				if (DockNode->Windows.Size <= 1)
				{
					DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
					DockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
					BottomBarWindow->Flags &= ~ImGuiWindowFlags_NoTitleBar;

					//BottomBarSize = { DockNode->Size.x + Style.DockingSeparatorSize, DockNode->Size.y };
					if (DockNode->ParentNode->ChildNodes[0]->VisibleWindow && DockNode->ParentNode->ChildNodes[1]->VisibleWindow)
					{
						BottomBarSize = { DockNode->ParentNode->Size.x + Style.DockingSeparatorSize, DockNode->ParentNode->Size.y };
					}
					else
					{
						BottomBarSize = { DockNode->Size.x + Style.DockingSeparatorSize, DockNode->Size.y };
					}
				}
				else if (DockNode->Windows.Size > 1)
				{
					DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
					DockNode->LocalFlags &= ~ImGuiDockNodeFlags_NoWindowMenuButton;
					LK_CORE_VERIFY(DockNode->ParentNode, "BottomBar has no parent node");

					//BottomBarSize = { DockNode->ParentNode->Size.x + Style.DockingSeparatorSize, DockNode->ParentNode->Size.y };
					if (DockNode->ParentNode->ChildNodes[0]->VisibleWindow && DockNode->ParentNode->ChildNodes[1]->VisibleWindow)
					{
						BottomBarSize = { DockNode->ParentNode->Size.x + Style.DockingSeparatorSize, DockNode->ParentNode->Size.y };
					}
					else
					{
						BottomBarSize = { DockNode->Size.x + Style.DockingSeparatorSize, DockNode->Size.y };
					}
				}
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
			ImGui::Text("Scaled res (%.1f, %.1f)", (WindowSize.X / WindowScalers.X), (WindowSize.Y / WindowScalers.X));

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
			ImGui::TextWrapped("Game and rendering engine written in C++20.");
			ImGui::Separator();

			/* Section: Written By */
			UI::Font::Push("Bold");
			ImGui::Text("Developed by");
			UI::Font::Pop();
			ImGui::BulletText("Lukas Gunnarsson (Lukkelele)");
			ImGui::Separator();

			/* Section: Dependencies */
			UI::Font::Push("Italic");
			{
				ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.50f, 1.0f), "Contains source code provided by");
				ImGui::BulletText("GLFW: %d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
			#if defined(LK_ENGINE_OPENGL)
				ImGui::BulletText("OpenGL: %d.%d", LOpenGL::GetMajorVersion(), LOpenGL::GetMinorVersion());
			#endif
				ImGui::BulletText("ImGui: %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
				ImGui::BulletText("ENTT: %s", ENTT_VERSION);
				ImGui::BulletText("nativefiledialog: %s", "1.2.1"); /* TODO: Get version from library, currently no way to do that(?). */
				ImGui::BulletText("yaml-cpp: %s", "0.6.0");         /* TODO: Get version from library, currently no way to do that(?). */
			}
			UI::Font::Pop();

			ImGui::Separator();
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
		});
	}

	void LEditorLayer::UI_ShowEditorWindowsDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("MenuBarSize: (%1.f, %1.f)", MenuBarSize.X, MenuBarSize.Y);
			ImGui::Text("TabBarSize: (%1.f, %1.f)", TabBarSize.X, TabBarSize.Y);
			ImGui::Text("RightSidebarSize: (%1.f, %1.f)", RightSidebarSize.X, RightSidebarSize.Y);
			ImGui::Text("BottomBarSize: (%1.f, %1.f)", BottomBarSize.X, BottomBarSize.Y);
			ImGui::Text("Current Tab: %s", TabManager.GetActiveTabName().c_str());
			ImGui::Text("Tabs: %d", TabManager.GetTabCount());
		}
		ImGui::EndGroup();
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

	void LEditorLayer::UI_ClearColorModificationMenu()
	{
		static ImGuiSliderFlags BackgroundSliderFlags = ImGuiSliderFlags_None;
		auto& Colors = ImGui::GetStyle().Colors;
		ImGui::BeginGroup();
		{
			UI::PushID("##ClearColorsModification");
			ImGui::Text("Background");
			ImGui::SliderFloat("##x", &LRenderer::ClearColor.x, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("##y", &LRenderer::ClearColor.y, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("##z", &LRenderer::ClearColor.z, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("##w", &LRenderer::ClearColor.w, 0.0f, 1.0f, " %.3f", BackgroundSliderFlags);
			ImGui::SliderFloat("UI Alpha", &Colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f, " %.2f", BackgroundSliderFlags);
			UI::PopID();
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

#if 0
							ImGui::SetNextWindowPos({ SecondViewportBounds[0].X, MenuBarSize.Y + TabBarSize.Y }, 
													ImGuiCond_Always);
#endif
							ImGui::SetNextWindowSize({ WindowSize.X, WindowSize.Y }, ImGuiCond_Always);
							ImGui::Begin("##TabWindow", NULL,
										 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
											 | ImGuiWindowFlags_NoResize);
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
				if (ImGui::MenuItem("Renderer Settings"))
				{
					ShowRenderSettingsWindow = !ShowRenderSettingsWindow;
				}
				if (ImGui::MenuItem("EditorLayer Window Sizes"))
				{
					bShowEditorWindowSizesWindow = !bShowEditorWindowSizesWindow;
				}

				ImGui::EndMenu(); /* View. */
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

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Metric Tool"))
				{
					bWindow_MetricTool = true;
				}

				if (ImGui::MenuItem("Style Editor"))
				{
					bWindow_StyleEditor = true;
				}

				if (ImGui::MenuItem("Live Objects"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Live Objects");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->bWindow_ObjectReferences = true;
					}
				}

				if (ImGui::MenuItem("View Asset Registry"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Asset Registry");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->bWindow_AssetRegistry = true;
					}
				}

				if (ImGui::MenuItem("Registered Fonts"))
				{
					LK_CORE_DEBUG_TAG("Editor", "Open -> Registered Fonts");
					if (FPanelData* PanelData = PanelManager->GetPanelData(PanelID::Tools))
					{
						PanelData->bIsOpen = true;
						PanelData->Panel.As<LToolsPanel>()->bWindow_RegisteredFonts = true;
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
					ImGui::Checkbox("Show window sizes on hover", &GDebug_DisplayWindowSizeOnHover);

					ImGui::EndMenu();
				}

				ImGui::EndMenu(); /* Debug. */
			}


			if (ImGui::MenuItem("About"))
			{
				UI_AboutPopup();
			}

			/* Horizontal space. */
			ImGui::Dummy(ImVec2(240, 0));

			if (ImGui::BeginMenu(std::string("Project: " + Project->GetName()).c_str()))
			{
				ImGui::EndMenu(); /* Project + Name. */
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

		TObjectPtr<LTexture2D> ButtonImage = LTextureLibrary::Get().GetWhiteTexture();
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x * 0.50f) - ButtonSize * 0.50f);
		if (ToolbarButton(ButtonImage, Color))
		{
			ButtonClicked = !ButtonClicked;
			LK_CORE_INFO("Clicked Button: {}", ButtonClicked ? "1" : "0");
		}
	}

	LEntity LEditorLayer::CreateCube()
	{
		FAssetHandle CubeHandle = LAssetManager::GetAssetHandleFromFilePath("Assets/Meshes/Cube.gltf");
		TObjectPtr<LMesh> CubeMesh = LAssetManager::GetAsset<LMesh>(CubeHandle);

		LEntity NewCubeEntity = EditorScene->CreateEntity();

		/// TODO: Just use a search function instead of iterating through like this
		std::unordered_set<FAssetHandle> AssetList = EditorScene->GetAssetList();
		for (const FAssetHandle AssetHandle : AssetList)
		{
			TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(AssetHandle);
			if (Mesh == CubeMesh)
			{
				// The cube is in the scene
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
			if (ImGui::BeginCombo(
					LK_FORMAT_STRING("Source: {}", Enum::ToString(RenderContext->GetSourceBlendFunction())).c_str(), 
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
					LK_FORMAT_STRING("Destination: {}", Enum::ToString(RenderContext->GetDestinationBlendFunction())).c_str(), 
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


}
