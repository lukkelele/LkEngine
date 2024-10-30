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
#include "LkEngine/Core/SelectionContext.h"
#include "LkEngine/Core/Math/Math.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Editor/EditorGlobals.h"

#include "LkEngine/Renderer/Framebuffer.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/ContentBrowser.h"

#include "LkEngine/Project/Project.h"

#include "EditorTabManager.h"
#include "ComponentEditor.h"
#include "NodeEditor/NodeEditor.h"

#include "LkEngine/ImGui/ImGuiLayer.h"

#define LK_UI_ENABLE_LEFT_SIDEBAR_CONTENT 1


namespace LkEngine {

	class LScene;
	class LSceneManagerPanel;

	enum class EEditorWindowType
	{
		None = 0,
		Viewport,  // EditorLayer Viewport, 'normal' mode
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
		glm::vec2 GetEditorWindowSize() const;
		float GetEditorWindowWidth() const;
		float GetEditorWindowHeight() const;

		FORCEINLINE glm::vec2 GetLeftSidebarSize() const { return LeftSidebarSize; }
		FORCEINLINE glm::vec2 GetRightSidebarSize() const { return RightSidebarSize; }
		FORCEINLINE glm::vec2 GetBottomBarSize() const { return BottomBarSize; }

		FORCEINLINE float GetViewportScalerX() const { return ViewportScalers.x; }
		FORCEINLINE float GetViewportScalerY() const { return ViewportScalers.y; }

		FORCEINLINE glm::vec2 GetMenuBarSize() const { return MenuBarSize; }
		FORCEINLINE glm::vec2 GetTabBarSize() const { return TabBarSize; }

		FORCEINLINE static LEditorLayer* Get() { return Instance; }

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

		void UI_SyncEditorWindowSizes(const glm::vec2& viewportSize);

		void UI_ShowMouseDetails();
		void UI_ShowViewportAndWindowDetails();
		void UI_ShowEditorWindowsDetails();
		void UI_ClearColorModificationMenu();
		void UI_ViewportTexture();
		void UI_WindowStatistics();
		void UI_TabManager();

		void CheckLeftSidebarSize();
		void CheckRightSidebarSize();
		void CheckBottomBarSize();

		TObjectPtr<LFramebuffer>& GetViewportFramebuffer() { return m_ViewportFramebuffer; }

		LEntity CreateCube();

	public:
		inline static bool InCreateItemProcess = false; // if true, the potentially created item is shown in the editor window // FIXME: REMOVE
		inline static ImVec2 SelectedEntityMenuSize = { 0, 440 }; // TODO: REMOVE/UPDATE

		glm::vec2 EditorViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f} };
		glm::vec2 EditorViewportPos = { 0.0f, 0.0f };
		glm::vec2 EditorWindowPos = { 0.0f, 0.0f };
		glm::vec2 EditorWindowSize = { 0.0f, 0.0f };
		glm::vec2 ViewportScalers = { 1.0f, 1.0f };

		bool ShouldUpdateWindowSizes = true;

		bool ShowRenderSettingsWindow = false;
		bool m_FillSidebarsVertically = true; // Always fill out sidebars vertically

		/// REMOVE
		ImVec2 LastSidebarLeftPos = ImVec2(0, 0);
		ImVec2 LastSidebarLeftSize = ImVec2(0, 0);
		ImVec2 LastSidebarRightPos = ImVec2(0, 0);
		ImVec2 LastSidebarRightSize = ImVec2(0, 0);
		ImVec2 LastBottomBarPos = ImVec2(0, 0);
		ImVec2 LastBottomBarSize = ImVec2(0, 0);
	private:
		/// REMOVE
		inline static glm::vec2 MenuBarSize = { 0.0f, 30.0f };
		inline static glm::vec2 TabBarSize = { 0.0f, 34.0f };
		inline static glm::vec2 BottomBarSize = { 0.0f, 240.0f };
		inline static glm::vec2 LeftSidebarSize = { 340.0f, 0.0f };
		inline static glm::vec2 RightSidebarSize = { 340.0f, 0.0f };
		inline static glm::vec2 BottomBarPos = { 0.0f, 0.0f };
		inline static glm::vec2 LeftSidebarPos = { 0.0f, 0.0f };
		inline static glm::vec2 RightSidebarPos = { 0.0f, 0.0f };

		inline static bool bWindowsHaveChangedInSize = true;
		inline static bool bShowEditorWindowSizesWindow = false;
	private:
		LEditorTabManager& TabManager;

		TObjectPtr<LScene> Scene = nullptr;
		bool m_Enabled = true;

		LVector2 ViewportBounds[2];
		LVector2 SecondViewportBounds[2];

		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditorLayer = false;

		EGizmo GizmoType = EGizmo::Translate;

        FEventCallback m_EventCallback; /// UPDATE ME

		TObjectPtr<LFramebuffer> m_ViewportFramebuffer;
		TObjectPtr<LEditorCamera> EditorCamera;

		TSharedPtr<LSceneManagerPanel> SceneManagerPanel;

		/* Active object. */
		TObjectPtr<LProject> m_Project{};

		// Editor
		TUniquePtr<LNodeEditor> NodeEditor;
		TUniquePtr<LComponentEditor> ComponentEditor{};
		TUniquePtr<LContentBrowser> ContentBrowser;
		// ~Editor

		LWindow* Window = nullptr;
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