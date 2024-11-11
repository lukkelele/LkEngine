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

		virtual void OnRender() override;
		virtual void OnRenderUI() override;

		FORCEINLINE TObjectPtr<LEditorCamera> GetEditorCamera() 
		{ 
			return EditorCamera; 
		}

		void SetScene(TObjectPtr<LScene> InScene);

		FORCEINLINE TObjectPtr<LScene> GetCurrentScene() 
		{ 
			return Scene; 
		}

		void SetUpdateWindowFlag(bool flag);

		FORCEINLINE bool IsEnabled() const { return m_Enabled; }

		FORCEINLINE EEditorWindowType GetCurrentWindowType() const 
		{ 
			return CurrentWindowType; 
		}

		/* FIXME: Update all of these, refactor away. */
		FORCEINLINE LVector2 GetEditorWindowSize() const { return EditorViewport->GetSize(); }

		FORCEINLINE glm::vec2 GetLeftSidebarSize() const { return LeftSidebarSize; }
		FORCEINLINE glm::vec2 GetRightSidebarSize() const { return RightSidebarSize; }
		FORCEINLINE glm::vec2 GetBottomBarSize() const { return BottomBarSize; }
		FORCEINLINE glm::vec2 GetMenuBarSize() const { return MenuBarSize; }
		FORCEINLINE glm::vec2 GetTabBarSize() const { return TabBarSize; }

		FORCEINLINE static LEditorLayer* Get() 
		{ 
			LK_VERIFY(Instance, "Invalid editor instance");
			return Instance; 
		}

	private:
		void RenderViewport();                
		void RenderViewport(TObjectPtr<LImage> Image); 

        void DrawObjectGizmo(const TObjectPtr<LObject>& InObject);
		void HandleExternalWindows();

		void UI_MainMenuBar();
		void UI_HandleManualWindowResize();

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

		void PrepareForLeftSidebar() const;
		void PrepareForRightSidebar() const;
		void PrepareForBottomBar() const;

		TObjectPtr<LFramebuffer>& GetViewportFramebuffer() { return ViewportFramebuffer; }

		LEntity CreateCube();

	public:
		glm::vec2 ViewportScalers = { 1.0f, 1.0f };

		bool bShouldUpdateWindowSizes = true;
		bool ShowRenderSettingsWindow = false;
		bool bFillSidebarsVertically = true; // Always fill out sidebars vertically
	private:
		/// REMOVE
		inline static LVector2 MenuBarSize = { 0.0f, 30.0f };
		inline static LVector2 TabBarSize = { 0.0f, 34.0f };
		inline static LVector2 BottomBarSize = { 0.0f, 240.0f };
		inline static LVector2 LeftSidebarSize = { 340.0f, 0.0f };
		inline static LVector2 RightSidebarSize = { 340.0f, 0.0f };
		inline static LVector2 BottomBarPos = { 0.0f, 0.0f };
		inline static LVector2 LeftSidebarPos = { 0.0f, 0.0f };
		inline static LVector2 RightSidebarPos = { 0.0f, 0.0f };

		inline static bool bWindowsHaveChangedInSize = true;
		inline static bool bShowEditorWindowSizesWindow = false;
	private:
		LEditorTabManager& TabManager;

		TObjectPtr<LScene> Scene = nullptr;
		bool m_Enabled = true;

		LVector2 ViewportBounds[2];

		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditorLayer = false;

		EGizmo GizmoType = EGizmo::Translate;

        FEventCallback m_EventCallback; /// UPDATE ME

		TObjectPtr<LFramebuffer> ViewportFramebuffer;
		TObjectPtr<LEditorCamera> EditorCamera;

		/** @brief The editor viewport.  */
		TObjectPtr<LViewport> EditorViewport;

		TSharedPtr<LSceneManagerPanel> SceneManagerPanel;

		/* Active object. */
		TObjectPtr<LProject> m_Project{};

		// Editor
		TUniquePtr<LNodeEditor> NodeEditor;
		TUniquePtr<LComponentEditor> ComponentEditor{};
		TUniquePtr<LContentBrowser> ContentBrowser;
		// ~Editor

		TObjectPtr<LWindow> Window{};

		EEditorWindowType CurrentWindowType = EEditorWindowType::None;

		/// REWORK ALL THESE FRIEND DECLARATIONS
		friend class Physics2D; // For getting UI window size when raycasting

		friend class LNodeEditorTab;
		friend class LMaterialEditorTab;
		friend class LSceneManagerPanel; 
		friend class LRenderer;
		
		inline static LEditorLayer* Instance = nullptr;
	};

}