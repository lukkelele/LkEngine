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

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Editor/EditorGlobals.h"

#include "LkEngine/Renderer/Framebuffer.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/UILayer.h"
#include "LkEngine/UI/ContentBrowser.h"

#define LK_UI_ENABLE_LEFT_SIDEBAR_CONTENT 1


namespace LkEngine {

	class LComponentEditor;
	class LEditorTabManager;
	class LNodeEditor;
	class LScene;
	class LSceneManagerPanel;
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

		virtual void OnUpdate(const float DeltaTime) override;

		void RenderViewport();
		void RenderViewport(TObjectPtr<LImage> Image);
		void Render2D();

		virtual void OnRender() override;
		virtual void OnRenderUI() override;

		FORCEINLINE TObjectPtr<LScene> GetEditorScene() { return EditorScene; }
		FORCEINLINE TObjectPtr<LEditorCamera> GetEditorCamera() { return EditorCamera; }

		void SetUpdateWindowFlag(bool flag);

		FORCEINLINE bool IsEnabled() const { return bEnabled; }

		/* FIXME: Update all of these, refactor away. */
		FORCEINLINE EEditorWindowType GetCurrentWindowType() const { return CurrentWindowType; }
		FORCEINLINE LVector2 GetEditorWindowSize() const { return EditorViewport->GetSize(); }

		FORCEINLINE static LEditorLayer* Get()
		{
			LK_CORE_ASSERT(Instance, "Editor not valid");
			return Instance;
		}

		void SetScene(TObjectPtr<LScene> InScene);
		void NewScene(const std::string& SceneName = "Untitled");
		void EmptyProject();
		void StarterProject();
		void SaveProjectAs();
		void SaveSceneAs();

	private:
		void DrawObjectGizmo(const TObjectPtr<LObject>& InObject);
		void HandleExternalWindows();

		void UI_MainMenuBar();
		void UI_HandleManualWindowResize();
		void UI_AboutPopup();

		void UI_SceneContent();
		void UI_CreateMenu();
		void UI_RenderSettingsWindow();

		void UI_SyncEditorWindowSizes(const LVector2& InViewportSize);

		void UI_ShowMouseDetails();
		void UI_ShowViewportAndWindowDetails();
		void UI_ShowEditorWindowsDetails();
		void UI_ClearColorModificationMenu();
		void UI_ViewportTexture();
		void UI_WindowStatistics();
		void UI_TabManager();
		
		void UI_OpenGLExtensions();

		void PrepareForLeftSidebar() const;
		void PrepareForRightSidebar() const;
		void PrepareForBottomBar() const;

		TObjectPtr<LFramebuffer>& GetViewportFramebuffer() { return ViewportFramebuffer; }

		LEntity CreateCube(); /* TODO: REMOVE */

	public:
		glm::vec2 ViewportScalers = { 1.0f, 1.0f };

		bool bShouldUpdateWindowSizes = true;
		bool ShowRenderSettingsWindow = false;
		bool bFillSidebarsVertically = true; // Always fill out sidebars vertically
	private:
		inline static bool bWindowsHaveChangedInSize = true;
		inline static bool bShowEditorWindowSizesWindow = false;

	private:
		LEditorTabManager& TabManager;

		TObjectPtr<LScene> EditorScene = nullptr;
		bool bEnabled = true;

		LVector2 ViewportBounds[2];
		TObjectPtr<LFramebuffer> ViewportFramebuffer;
		TObjectPtr<LSceneRenderer> ViewportRenderer{};

		TObjectPtr<LEditorCamera> EditorCamera; /* TODO: Remove pointer, just make raw member. */

		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditorLayer = false;

		EGizmo GizmoType = EGizmo::Translate;

		FEventCallback m_EventCallback; /// UPDATE ME

		std::shared_ptr<LSceneManagerPanel> SceneManagerPanel{};

		TObjectPtr<LProject> Project{};

		// Editor
		TObjectPtr<LViewport> EditorViewport;
		std::unique_ptr<LNodeEditor> NodeEditor;
		std::unique_ptr<LComponentEditor> ComponentEditor{};
		std::unique_ptr<LContentBrowser> ContentBrowser;
		// ~Editor

		TObjectPtr<LWindow> Window{};
		TObjectPtr<LRenderer2D> Renderer2D{};

		EEditorWindowType CurrentWindowType = EEditorWindowType::None;

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
