/******************************************************************
 * EditorLayer
 *
 * Main editor.
 *******************************************************************/
#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Thread.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Viewport.h"
#include "LkEngine/Core/SelectionContext.h"
#include "LkEngine/Core/Math/Math.h"

#include "LkEngine/Editor/EditorGlobals.h"
#include "LkEngine/Editor/EditorSettings.h"
#include "LkEngine/Editor/PanelManager.h"
#include "LkEngine/Editor/Panels/ContentBrowserPanel.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Renderer/Framebuffer.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/UILayer.h"


namespace LkEngine {

	class LComponentEditor;
	class LEditorTabManager;
	class LNodeEditor;
	class LScene;
	class LSceneManagerPanel;
	class LSubmesh;
	class LRenderer2D;
	class LProject;
	class LViewport;

	/* FIXME: */
	enum class EEditorWindowType
	{
		None = 0,
		Viewport,
		NodeEditorLayer,
	};

	class LEditorLayer : public LLayer
	{
	public:
		LEditorLayer();
		~LEditorLayer() = default;

		virtual void Initialize() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(const float InDeltaTime) override;

		void RenderViewport();
		void RenderViewport(TObjectPtr<LImage> Image);
		void Render2D();

		virtual void OnRender() override;
		virtual void OnRenderUI() override;

		FORCEINLINE TObjectPtr<LScene> GetEditorScene() { return EditorScene; }
		FORCEINLINE TObjectPtr<LEditorCamera> GetEditorCamera() { return EditorCamera; }

		FORCEINLINE bool IsEnabled() const { return bEnabled; }

		FORCEINLINE static LEditorLayer* Get()
		{
			LK_CORE_ASSERT(Instance, "Editor instance not valid");
			return Instance;
		}

		void SetScene(TObjectPtr<LScene> InScene);
		void NewScene(const std::string& SceneName = "Untitled");
		void EmptyProject();
		void StarterProject();
		void SaveProjectAs();
		void SaveSceneAs();

	private:
		void UI_PrepareTopBar();
		void UI_PrepareLeftSidebar() const;
		void UI_PrepareRightSidebar() const;
		void UI_PrepareEditorViewport();

		void DrawObjectGizmo(LEntity Entity);
		void UI_RenderExternalWindows();

		void UI_MainMenuBar();
		void UI_ToolBar();
		void UI_AboutPopup();

		void UI_SyncEditorWindowSizes(const LVector2& InViewportSize);

		void UI_ShowMouseDetails();
		void UI_ShowViewportAndWindowDetails();
		void UI_ClearColorModificationMenu();
		void UI_ViewportTexture();
		void UI_WindowStatistics();
		void UI_TabManager();

		void UI_HandleDragAndDrop();
		
		void UI_OpenGLExtensions();
		void UI_RenderSettingsWindow(); /* TODO: Re-evaluate */

		struct FRayCast
		{
			LVector Pos;
			LVector Dir;

			FRayCast(const LVector& InPos, const LVector& InDir)
				: Pos(InPos)
				, Dir(InDir) 
			{
			}
		};

		FRayCast CastRay(const LEditorCamera& Camera, const float MousePosX, const float MousePosY);

		TObjectPtr<LFramebuffer>& GetViewportFramebuffer() 
		{ 
			LK_CORE_ASSERT(ViewportFramebuffer, "Invalid viewport framebuffer");
			return ViewportFramebuffer; 
		}

		LEntity CreateCube(); /* TODO: REMOVE */

		std::pair<float, float> GetMouseViewportSpace(const bool IsPrimaryViewport);

		/**
		 * FSelectionData
		 */
		struct FSelectionData
		{
			LEntity Entity;
			LSubmesh* Mesh{};
			float Distance = 0.0f;
		};

	public:
		glm::vec2 ViewportScalers = { 1.0f, 1.0f };
	private:
		FEditorSettings& Settings;
		LEditorTabManager& TabManager;
		TObjectPtr<LPanelManager> PanelManager;

		TObjectPtr<LProject> Project{};
		TObjectPtr<LScene> EditorScene{};
		TObjectPtr<LSceneRenderer> SceneRenderer{};
		bool bEnabled = true;

		TObjectPtr<LWindow> Window{};
		TObjectPtr<LRenderer2D> Renderer2D{};

		LVector2 ViewportBounds[2]; /* TODO: Change name to 'PrimaryViewportBounds' */
		TObjectPtr<LViewport> EditorViewport;
		TObjectPtr<LFramebuffer> ViewportFramebuffer;
		TObjectPtr<LSceneRenderer> ViewportRenderer{};
		TObjectPtr<LEditorCamera> EditorCamera; /* TODO: Remove pointer, just make raw member. */

		EGizmo GizmoType = EGizmo::Translate;

		FEventCallback m_EventCallback; /// UPDATE ME


		EEditorWindowType CurrentWindowType = EEditorWindowType::None; /// REMOVE

		/// REWORK ALL THESE FRIEND DECLARATIONS
		/// -> ESPECIALLY THIS TO Physics2D !!!!!!!
		friend class Physics2D; // For getting UI window size when raycasting

		friend class LNodeEditorTab;
		friend class LMaterialEditorTab;
		friend class LSceneManagerPanel;
		friend class LRenderer;

		inline static LEditorLayer* Instance = nullptr;
	};

}
