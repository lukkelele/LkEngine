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
#include "LkEngine/Core/Event/SceneEvent.h"
#include "LkEngine/Input/Mouse.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/UI/SceneManagerPanel.h"
#include "LkEngine/UI/ContentBrowser.h"
#include "LkEngine/UI/DebugPanel.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLRenderer.h"
#include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"


namespace LkEngine {

	namespace {
		LVector2 MenuBarSize = { 0.0f, 30.0f };
		LVector2 TabBarSize = { 0.0f, 34.0f };
		LVector2 BottomBarSize = { 0.0f, 240.0f };
		LVector2 LeftSidebarSize = { 340.0f, 0.0f };
		LVector2 RightSidebarSize = { 340.0f, 0.0f };
		LVector2 BottomBarPos = { 0.0f, 0.0f };
		LVector2 LeftSidebarPos = { 0.0f, 0.0f };
		LVector2 RightSidebarPos = { 0.0f, 0.0f };

		LVector2 LastSidebarLeftPos{};
		LVector2 LastSidebarLeftSize{};
		LVector2 LastSidebarRightPos{};
		LVector2 LastSidebarRightSize{};
		LVector2 LastBottomBarPos{};
		LVector2 LastBottomBarSize{};

		bool bRenderSkybox = true;
	}

	static TObjectPtr<LTexture2D> FloorTexture;
	static TObjectPtr<LTexture2D> CubeTexture;

	TObjectPtr<LDebugPanel> DebugPanel{};

	/* --- REMOVE ME --- */
	static FAssetHandle CubeAssetHandle;
	/* ----------------- */

	LEditorLayer::LEditorLayer()
		: LLayer("EditorLayer")
		, TabManager(LEditorTabManager::Get())
	{
		LCLASS_REGISTER();
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

		GOnObjectCreated.Add([&](const LObject* NewObject)
		{
			LK_CORE_DEBUG_TAG("Editor", "Object Class=\"{}\"  Name=\"{}\"", NewObject->GetClass()->GetName(), NewObject->ClassName());
		});

		GOnSceneSetActive.Add([&](const TObjectPtr<LScene>& NewActiveScene)
		{
			SetScene(NewActiveScene);
		});

		/* Editor UI components. */
		SceneManagerPanel = MakeShared<LSceneManagerPanel>();
		ContentBrowser = MakeUnique<LContentBrowser>();
		ComponentEditor = MakeUnique<LComponentEditor>();

		Window->OnWindowSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			SetUpdateWindowFlag(true);
		});

		Window->OnViewportSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			/* Update scalers. */
			const LVector2 EditorWindowSize = EditorViewport->GetSize();
			LVector2 WindowScalers;
			WindowScalers.X = EditorWindowSize.X / NewWidth;
			WindowScalers.Y = EditorWindowSize.Y / NewHeight;
			EditorViewport->SetScalers(WindowScalers);

			SetUpdateWindowFlag(true);
		});
	}

	void LEditorLayer::Initialize()
	{
		LObject::Initialize();
		LK_CORE_DEBUG_TAG("Editor", "Initializing");

		/* Attach to LWindow delegates. */
		LK_CORE_ASSERT(Window, "Window reference is nullptr");
		FWindowData& WindowData = Window->GetWindowData();
		LK_CORE_DEBUG_TAG("Editor", "Attaching to window delegates");

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

		/* UI components. */
		SceneManagerPanel->Initialize();
		ContentBrowser->Initialize();
		ComponentEditor->Initialize();

		LK_CORE_DEBUG_TAG("Editor", "Adding debugging panel");
		DebugPanel = TObjectPtr<LDebugPanel>::Create();

		/* TODO: Load last open project, else load an empty 'default' project. */
		if (!LProject::Current())
		{
			EmptyProject();
		}

		/* Bind delegate for GEditorOnSelectionChanged. */
		auto OnSelectionChanged = [&](const LObject& Object)
		{
			LK_CORE_DEBUG_TAG("Editor", "OnSelectionChanged: '{}', references: {}", 
							  Object.ClassName(), Object.GetReferenceCount());
		};
		GEditorOnSelectionChanged.Add(OnSelectionChanged);

		/* Force editor viewport to sync data. */
		EditorViewport->SetDirty(true);

		/* Create the 2D renderer. */
		FRenderer2DSpecification Renderer2DSpec;
		Renderer2D = TObjectPtr<LRenderer2D>::Create(Renderer2DSpec);
		Renderer2D->Initialize();

		CubeAssetHandle = LAssetManager::GetDebugCubeHandle();

		/* FIXME: Temporary debugging. */
		LOpenGL_Debug::InitializeEnvironment();

		bInitialized = true;
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
		LRenderer::Submit([&]()
		{
			LRenderer::GetViewportFramebuffer()->Bind();

			//LOpenGL_Debug::RenderMirrorTexture(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			//LOpenGL_Debug::RenderCubes(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());
			LOpenGL_Debug::RenderFloor(EditorCamera->GetViewMatrix(), EditorCamera->GetProjectionMatrix());

			LFramebuffer::TargetSwapChain();
		});

		LRenderer::Submit([&]()
		{
			LRenderer::GetViewportFramebuffer()->Bind();
			LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);

			using namespace LOpenGL_Debug;

			const glm::mat4 ProjectionMatrix = EditorCamera->GetProjectionMatrix();
			const glm::mat4 ViewMatrix = glm::mat4(glm::mat3(EditorCamera->GetViewMatrix()));

			TObjectPtr<LMesh> Cube = LAssetManager::GetAsset<LMesh>(CubeAssetHandle);
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
				LK_CORE_ASSERT(Cube->GetIndexBuffer()->GetCount() > 0, "IndexBuffer count not valid");

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

		/**
		 * Render the debug skybox.
		 * TODO: Move away from here.
		 */

		if (bRenderSkybox)
		{
			LRenderer::Submit([&]()
			{
				LRenderer::GetViewportFramebuffer()->Bind();

				LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);
				static constexpr unsigned int SkyboxModelSize = 100;

				LK_CORE_ASSERT(LOpenGL_Debug::SkyboxShader);
				LOpenGL_Debug::SkyboxShader->Bind();

				/* Make the TextureCube follow us. */
				const glm::mat4 ProjectionMatrix = EditorCamera->GetProjectionMatrix();
				const glm::mat4 ViewMatrix = glm::mat4(glm::mat3(EditorCamera->GetViewMatrix()));
				LOpenGL_Debug::SkyboxShader->Set("u_ViewProjectionMatrix", ProjectionMatrix * ViewMatrix);

				LOpenGL_Debug::SkyboxShader->Set("u_CameraPos", EditorCamera->GetViewMatrix());
				LOpenGL_Debug::SkyboxShader->Set("u_Model", glm::mat4(SkyboxModelSize));
				LOpenGL_Debug::SkyboxVertexBuffer->Bind();
				LOpenGL_Debug::SkyboxTexture->Bind(0);
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));

				LRenderer::SetDepthFunction(EDepthFunction::Less);

				LFramebuffer::TargetSwapChain();
			});
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
		ImGuiIO& IO = ImGui::GetIO();
		ImGuiStyle& Style = ImGui::GetStyle();
		auto& Colors = Style.Colors;
		IO.ConfigWindowsResizeFromEdges = IO.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		const LVector2 WindowSize = EditorViewport->GetSize();

		ImGuiViewportP* Viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		UI::BeginViewport(UI_CORE_VIEWPORT, Window.Get(), Viewport);
		UI_HandleManualWindowResize();
		UI::BeginDockSpace(LkEngine_DockSpace);

		/* Main menu. */
		UI_MainMenuBar();

		/*-----------------------------------------------------------------------------
			Sidebar1
							Default to the left.
		-----------------------------------------------------------------------------*/
		static const ImGuiID DockspaceID_Sidebar1 = ImGui::GetID("Dockspace_LeftSidebar");
		static bool ResetDockspace_LeftSidebar = true;

		PrepareForLeftSidebar();
		ImGui::Begin(UI::Sidebar1, nullptr, UI::SidebarFlags);
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

			/* Color modification. */
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Colors"))
			{
				UI_ClearColorModificationMenu();

				ImGui::TreePop(); /* Colors. */
			}

			/* UI Panels. */
			SceneManagerPanel->OnRenderUI();
			DebugPanel->OnRenderUI();

			const ImVec2 WindowSize = ImGui::GetWindowSize();
			const ImVec2 WindowPos = ImGui::GetWindowPos();
			if ((WindowSize.x != LastSidebarLeftSize.X) || (WindowSize.y != LastSidebarLeftSize.Y))
			{
				bWindowsHaveChangedInSize = true;
			}

			LastSidebarLeftPos = { WindowPos.x, WindowPos.y };
			LastSidebarLeftSize = { WindowSize.x, WindowSize.y };
#endif
		}
		ImGui::End(); /* Left Sidebar. */

		/*-----------------------------------------------------------------------------
			Sidebar2
							Default to the right.
		-----------------------------------------------------------------------------*/
		PrepareForRightSidebar();
		ImGui::Begin(UI::Sidebar2, nullptr, UI::SidebarFlags);
		{
			ComponentEditor->OnRenderUI();

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

			/* Poll window information to detect changes window position and size. */

			const ImVec2 WindowSize = ImGui::GetWindowSize();
			if ((WindowSize.x != RightSidebarSize.X) || (WindowSize.x != RightSidebarSize.Y))
			{
				bWindowsHaveChangedInSize = true;
			}

			const ImVec2 WindowPos = ImGui::GetWindowPos();
			LastSidebarRightPos = { WindowPos.x, WindowPos.y };
			LastSidebarRightSize = { WindowSize.x, WindowSize.y };

			ImGui::Separator();
		}
		ImGui::End(); /* UI_SIDEBAR_RIGHT */

#if 0
		LSceneManagerPanel::DrawComponents(SelectionContext::SelectedEntity);
#endif

		/*-----------------------------------------------------------------------------
			Main Window
									Editor Viewport
		-----------------------------------------------------------------------------*/
		UI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		UI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		UI::Begin(UI_CORE_VIEWPORT, UI::CoreViewportFlags);
		{
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X, MenuBarSize.Y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(WindowSize.X, Viewport->Size.y - (BottomBarSize.Y + MenuBarSize.Y)),
									 ImGuiCond_Always);
			UI::Begin(UI::VIEWPORT_TEXTURE, UI::ViewportTextureFlags);
			{
				/* Viewport Texture. */
				UI_ViewportTexture();

				if (GSelectedObject)
				{
					DrawObjectGizmo(GSelectedObject);
				}

				/* Statistics, FPS counter... */
				UI_WindowStatistics();
			}
			UI::End(); /* Viewport Texture. */
		}
		UI::End();
		UI::PopStyleVar(2);


		/* Bottom Bar. */
		/* TODO: Change name of UI_BOTTOM_BAR. */
		PrepareForBottomBar();
		ImGui::Begin(UI_BOTTOM_BAR, nullptr, UI::SidebarFlags);
		{
			ContentBrowser->OnRenderUI();

			const ImVec2 WindowSize = ImGui::GetWindowSize();
			if (WindowSize.x != BottomBarSize.X || WindowSize.y != BottomBarSize.Y)
			{
				bWindowsHaveChangedInSize = true;
			}

			const ImVec2 WindowPos = ImGui::GetWindowPos();
			LastBottomBarPos = { WindowPos.x, WindowPos.y };
			LastBottomBarSize = { WindowSize.x, WindowSize.y };
		}
		ImGui::End(); /* Bottom Bar. */

		/* Render tabs. */
		UI_TabManager();

		/*
		 * If the window sizes have been updated, set to false.
		 * This must be run BEFORE the 'bWindowsHaveChangedInSize'.
		 */
		if (bShouldUpdateWindowSizes)
		{
			bShouldUpdateWindowSizes = false;
		}

		LVector2 ViewportSize = { Viewport->WorkSize.x, Viewport->WorkSize.y };
		UI_SyncEditorWindowSizes(ViewportSize);

		/// FIXME: What does this even do?
		// Take care of tabs here.
		TabManager.End();

		ImGui::End(); // Viewport
		HandleExternalWindows();

		ImGui::End(); // Core Viewport
	}

	void LEditorLayer::UI_ViewportTexture()
	{
		TObjectPtr<LImage2D> ViewportImage = ViewportFramebuffer->GetImage(0);
		const LVector2 WindowSize = EditorViewport->GetSize();

		UI::Image(ViewportImage, ImVec2(WindowSize.X - 2, WindowSize.Y + MenuBarSize.Y), ImVec2(0, 1), ImVec2(1, 0));

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
			{
				UI_ShowEditorWindowsDetails();
			}
			ImGui::End();
		}
	}

	/// FIXME:
	void LEditorLayer::DrawObjectGizmo(const TObjectPtr<LObject>& InObject)
	{
		if (InObject->IsA<LEntity>())
		{
			LEntity Entity = InObject->As<LEntity>();
			// LEntity Entity = const_cast<TObjectPtr<LObject>&>(InObject)->As<LEntity>();

			LTransformComponent& TransformComponent = Entity.Transform();
			glm::mat4 TransformMatrix = TransformComponent.GetTransform();
			const glm::vec3& CameraPos = EditorCamera->GetPosition();
			const glm::mat4& ViewMatrix = EditorCamera->GetViewMatrix();
			const glm::mat4& ProjectionMatrix = EditorCamera->GetProjectionMatrix();

			/* Viewport bounds can be indexed from 0 to 1. */
			const LVector2* ViewportBounds = EditorViewport->GetViewportBounds();
			const float PosX = ViewportBounds[0].X;
			const float PosY = ViewportBounds[0].Y;
			// const float Width  = SecondViewportBounds[1].X - SecondViewportBounds[0].X;
			// const float Height = SecondViewportBounds[1].Y - SecondViewportBounds[0].Y;

			ImGuiWindow* Window = ImGui::FindWindowByName(UI::VIEWPORT_TEXTURE);
			ImGui::Begin(Window->Name, nullptr, UI::CoreViewportFlags | ImGuiWindowFlags_NoScrollbar);
			{
#if 0
				ImGuizmo::SetOrthographic(static_cast<int>(EditorCamera->GetProjectionType()));
				ImGuizmo::SetDrawlist();

				if (TabManager.GetTabCount() == 1)
				{
					ImGuizmo::SetRect(PosX, (PosY - BottomBarSize.Y + MenuBarSize.Y), 
									  EditorWindowSize.X, EditorWindowSize.Y);
				}
				else
				{
					ImGuizmo::SetRect(PosX, (PosY - BottomBarSize.Y + MenuBarSize.Y + TabBarSize.Y), 
									  EditorWindowSize.X, EditorWindowSize.Y);
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
#endif
			}
			ImGui::End(); // Window->Name
		}
	}

	// TODO: Right alignment in the child window
	void LEditorLayer::UI_WindowStatistics()
	{
		const LVector2 WindowSize = EditorViewport->GetSize();

		/* Window statistics, FPS counter etc. */
		static ImVec2 StatisticsWindowSize = ImVec2(ImGui::CalcTextSize("Forward Direction: { N1, N2, N3 }").x + 200, 500);

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
				// ImGui::Text("Forward Direction: %s", EditorCamera->GetForwardDirection<LVector>().ToString().CStr());

				ImGui::Text("Distance: %.2f", EditorCamera->GetDistance());
				ImGui::Text("Focalpoint: (%.2f, %.2f, %.2f)", EditorCamera->GetFocalPoint().x,
							EditorCamera->GetFocalPoint().y, EditorCamera->GetFocalPoint().z);

				// Mouse data.
				ImGui::Text("Mouse Button: %s", Enum::ToString(LInput::GetLastMouseButton()));
				ImGui::Text("Editor Window: %s", EditorViewport->GetSize().ToString().c_str());
				ImGui::Text("Editor Window Scalers: %s", EditorViewport->GetScalers().ToString().c_str());
			}
		}
		ImGui::EndChild();
	}

	void LEditorLayer::UI_HandleManualWindowResize()
	{
#if 0
		GLFWwindow* window = LApplication::Get()->GetWindow().GetGlfwWindow();
		const bool maximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
		ImVec2 newSize, newPosition;
		if (!maximized && UI::UpdateWindowManualResize(ImGui::GetCurrentWindow(), newSize, newPosition))
		{
			glfwSetWindowPos(window, static_cast<int>(newPosition.x), static_cast<int>(newPosition.y));
			glfwSetWindowSize(window, static_cast<int>(newSize.X), static_cast<int>(newSize.Y));
		}
#endif
	}

	void LEditorLayer::UI_SceneContent()
	{
		static ImVec2 SelectedEntityMenuSize = { 0, 400 };
		static constexpr const char* UI_RenderID = "##LKENGINE-SCENE-CONTENT";
		const float MenuHeight = Window->GetHeight() - SelectedEntityMenuSize.y;

		UI::PushID(UI_RenderID);
		ImGui::BeginGroup();
		ImGui::SeparatorText("Scene");

		static ImGuiSelectableFlags SelectableFlags = ImGuiSelectableFlags_SpanAllColumns
			| ImGuiSelectableFlags_AllowDoubleClick;

		auto EntitiesView = EditorScene->m_Registry.view<LTransformComponent>();
		ImGui::Text("Entities in scene: %d", EntitiesView.size());
		for (const entt::entity& EntityRef : EntitiesView)
		{
			LEntity Entity = { EntityRef, EditorScene };

			bool bIsSelected = false;
			std::string label = LK_FORMAT_STRING("{}", Entity.Name());
			if (ImGui::Selectable(label.c_str(), &bIsSelected, SelectableFlags))
			{
			}
		}
		ImGui::EndGroup();

		UI::PopID(UI_RenderID);
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
			if (ImGui::BeginCombo(LK_FORMAT_STRING("Source: {}", Enum::ToString(RenderContext->GetSourceBlendFunction()))
									  .c_str(),
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
			if (ImGui::BeginCombo(LK_FORMAT_STRING("Destination: {}",
												   Enum::ToString(RenderContext->GetDestinationBlendFunction()))
									  .c_str(),
								  nullptr, ImGuiComboFlags_NoPreview))
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

	void LEditorLayer::UI_CreateMenu()
	{
		UI::PushID("UI_CreateMenu");
		ImGui::SeparatorText("Create Menu");

		static char nameInputBuffer[140] = "item";
		ImGui::Text("Name");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(140);
		ImGui::InputText("##LkEngine-CreateMenu-InputBuffer", nameInputBuffer, LK_ARRAYSIZE(nameInputBuffer),
						 ImGuiInputFlags_RepeatRateDefault);

		const char* geometricShapes[] = { "Rectangle", "Circle", "Triangle" };
		static int geometricShapeCurrentIndex = 0; // Here we store our selection data as an index
		const char* geometricPreviewValue =
			geometricShapes[geometricShapeCurrentIndex]; // Pass in the preview value visible before opening the combo (it
														 // could be anything)
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

#if 0 /// DISABLED
	  // Selectable geometric shapes
	  // Can be clicked on to select diffent shapes instead of dropdown menu
		ImGui::BeginGroup();
		{
			static const ImVec4 tintColor = ImVec4(1, 1, 0.90f, 1);
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
#endif

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
			ImGui::DragFloat("##Width", &rectangle_Width, 0.10f, 0.010f, 0.0f, "%.2f");

			/* Height. */
			ImGui::Text("Height");
			ImGui::SameLine(0, rectangle_ColumnPadding - ImGui::CalcTextSize("Height").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Height", &rectangle_Height, 0.10f, 0.010f, 0.0f, "%.2f");
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
			ImGui::DragFloat("##Radius", &circle_Radius, 0.010f, 0.010f, 0.0f, "%.2f");

			// Thickness
			ImGui::Text("Thickness");
			ImGui::SameLine(0, circle_ColumnPadding - ImGui::CalcTextSize("Thickness").x);
			ImGui::SetNextItemWidth(shapeSizeColumnSize);
			ImGui::DragFloat("##Thickness", &circle_Thickness, 0.10f, 0.10f, 0.0f, "%.2f");
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
		bShouldUpdateWindowSizes = flag;

		ViewportBounds[0] = { 0, 0 };
		ViewportBounds[1] = { Window->GetViewportWidth(), Window->GetViewportHeight() };
	}

	void LEditorLayer::NewScene(const std::string& SceneName)
	{
		EditorScene = TObjectPtr<LScene>::Create(SceneName, true);
		EditorScene->SetActive(true);
	}

	void LEditorLayer::EmptyProject()
	{
		if (LProject::Current())
		{
			/* TODO: Close current project. */
		}

		NewScene();

		LK_CORE_INFO_TAG("Editor", "Creating empty project");
		Project = TObjectPtr<LProject>::Create();
		Project->SetName("EmptyProject");
		LProject::SetActive(Project);
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
			ImGui::Text("Last Right Sidebar Size: (%1.f, %1.f)", LastSidebarRightSize.X, LastSidebarRightSize.Y);
		}
		ImGui::EndGroup();
	}

	void LEditorLayer::UI_ShowEditorWindowsDetails()
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("MenuBarSize: (%1.f, %1.f)", MenuBarSize.X, MenuBarSize.Y);
			ImGui::Text("TabBarSize: (%1.f, %1.f)", TabBarSize.X, TabBarSize.Y);
			ImGui::Text("RightSidebarSize: (%1.f, %1.f)", RightSidebarSize.X, RightSidebarSize.Y);
			ImGui::Text("BottomBarSize: (%1.f, %1.f)", BottomBarSize.X, BottomBarSize.Y);
			ImGui::Text("Last Bottombar Size: (%1.f, %1.f)", LastBottomBarSize.X, LastBottomBarSize.Y);
			ImGui::Text("Current Tab: %s", TabManager.GetActiveTabName().c_str());
			ImGui::Text("Tabs: %d", TabManager.GetTabCount());
		}
		ImGui::EndGroup();
	}

	void LEditorLayer::UI_SyncEditorWindowSizes(const LVector2& InViewportSize)
	{
		const LVector2 WindowSize = EditorViewport->GetSize();

		// Check to see if any of the editor windows have changed in size and if they have
		// then readjust the Viewport
		if (bWindowsHaveChangedInSize)
		{
			LeftSidebarSize = { LastSidebarLeftSize.X, LastSidebarLeftSize.Y };
			RightSidebarSize = { LastSidebarRightSize.X, LastSidebarRightSize.Y };
			BottomBarSize = { LastBottomBarSize.X, LastBottomBarSize.Y };

			EditorViewport->SetPosition({ LeftSidebarSize.X, BottomBarSize.Y });
			EditorViewport->SetSizeX(InViewportSize.X - LeftSidebarSize.X - RightSidebarSize.X);

			/* Only take the size of the TabBar into account if any tabs exist. */
			if (TabManager.GetTabCount() == 1)
			{
				EditorViewport->SetSizeY(InViewportSize.Y - BottomBarSize.Y);
			}
			else
			{
				EditorViewport->SetSizeY(InViewportSize.Y - BottomBarSize.Y + TabBarSize.Y);
			}

			/* Take the menubar size into account.*/
			// EditorWindowPos.y -= MenuBarSize.Y;

			if (bFillSidebarsVertically)
			{
				LeftSidebarSize.Y = InViewportSize.Y;
				RightSidebarSize.Y = InViewportSize.Y;
			}

			bWindowsHaveChangedInSize = false;
			bShouldUpdateWindowSizes = true;
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

	void LEditorLayer::PrepareForLeftSidebar() const
	{
		if (bShouldUpdateWindowSizes)
		{
			/* FIXME */
			ImGuiViewport* Viewport = ImGui::GetWindowViewport();
			ImGui::SetNextWindowPos(ImVec2(0, MenuBarSize.Y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(LeftSidebarSize.X, Viewport->WorkSize.y), ImGuiCond_Always);
		}
	}

	void LEditorLayer::PrepareForRightSidebar() const
	{
		if (bShouldUpdateWindowSizes)
		{
			ImGuiViewport* Viewport = ImGui::GetWindowViewport();
			ImGui::SetNextWindowPos(ImVec2(Viewport->Size.x - RightSidebarSize.X, MenuBarSize.Y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(RightSidebarSize.X, RightSidebarSize.Y), ImGuiCond_Always);
		}
	}

	void LEditorLayer::PrepareForBottomBar() const
	{
		if (bShouldUpdateWindowSizes)
		{
			ImGuiViewport* Viewport = ImGui::GetWindowViewport();
			ImGui::SetNextWindowPos(ImVec2(LeftSidebarSize.X, Viewport->Size.y - BottomBarSize.Y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(Viewport->Size.x - (LeftSidebarSize.X + RightSidebarSize.X), BottomBarSize.Y),
									 ImGuiCond_Always);
		}
	}

	void LEditorLayer::SetScene(TObjectPtr<LScene> InScene)
	{
		LK_CORE_ASSERT(false, "Remove this function...");
		LK_CORE_VERIFY(InScene, "Scene is nullptr");
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
			MenuBarSize.X = ImGui::GetCurrentWindow()->Size.x;
			MenuBarSize.Y = ImGui::GetFrameHeight();

			if (ImGui::BeginMenu("File"))
			{

				ImGui::EndMenu(); /* File. */
			}

			if (ImGui::BeginMenu("Project"))
			{
				/* Save project. */
				if (ImGui::MenuItem("Save"))
				{
					LK_CORE_VERIFY(Project);
					Project->Save();
				}

				/* New project. */
				if (ImGui::MenuItem("New"))
				{
				}

				/* Load existing project. */
				if (ImGui::MenuItem("Load"))
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

				ImGui::EndMenu(); /* View. */
			}

			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Live Objects"))
				{
					// bWindow_LiveObjects = true;
					DebugPanel->bWindow_ObjectReferences = true;
				}

				ImGui::EndMenu(); /* Debug. */
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("New"))
				{
				}
				if (ImGui::MenuItem("Load"))
				{
					static constexpr bool IsEditorScene = true;
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

			/* Horizontal space. */
			ImGui::Dummy(ImVec2(40, 0));
			if (ImGui::BeginMenu(std::string("Project: " + Project->GetName()).c_str()))
			{
				ImGui::EndMenu(); /* Project + Name. */
			}
		}
		ImGui::EndMainMenuBar();
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
							EditorScene->CopyComponentIfExists<LMeshComponent>(NewCubeEntity, EditorScene->m_Registry, Entity);
							EditorScene->CopyComponentIfExists<LTagComponent>(NewCubeEntity, EditorScene->m_Registry, Entity);
							LK_CORE_VERIFY((NewCubeEntity.HasComponent<LMeshComponent>()) && (NewCubeEntity.HasComponent<LTagComponent>()));
						}
					}
				}
			}
		}

		return NewCubeEntity;
	}

}
