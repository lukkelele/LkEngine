#pragma once

#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/Math/Math.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

#include "LkEngine/Renderer/Framebuffer.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/SelectionContext.h"
#include "LkEngine/UI/ContentBrowser.h"

#include "LkEngine/Project/Project.h"

#include "EditorTabManager.h"
#include "ComponentEditor.h"
#include "NodeEditor/NodeEditor.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

    constexpr const char* UI_CORE_VIEWPORT = "##lkengine-core-viewport";
    constexpr const char* UI_RENDER_WINDOW = "##lkengine-render-window";
    constexpr const char* UI_TOP_BAR = "##lkengine-top-bar";
    constexpr const char* UI_BOTTOM_BAR = "##lkengine-lower-bar";
    constexpr const char* UI_SIDEBAR_LEFT = "##lkengine-sidebar-left";
    constexpr const char* UI_SIDEBAR_RIGHT = "##lkengine-sidebar-right";
    constexpr const char* UI_SELECTED_ENTITY_INFO = "##lkengine-selected-entity-info";

	class Scene;
	class SceneManagerPanel;

	class EditorLayer : public Layer
	{
	public:
		enum class WindowType
		{
			None = 0,
			Viewport,  // EditorLayer Viewport, 'normal' mode
			NodeEditorLayer,
		};

		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate = 7 << 3,
			Scale = 7 << 6
		};

	public:
		EditorLayer();
		~EditorLayer() = default;

		void Init();
		void OnUpdate();

		void OnRender();
		void OnImGuiRender();

		void OnEvent(Event& e) override;
		void RegisterEvent(Event& e);
		void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

		Ref<EditorCamera> GetEditorCamera() { return m_EditorCamera; }
		void SetScene(Ref<Scene> scene);
		Ref<Scene> GetCurrentScene() { return m_Scene; }

		const char* UI_GetSelectedEntityWindowName() { return SelectedEntityWindow.c_str(); }
		void SetUpdateWindowFlag(bool flag);

		WindowType GetCurrentWindowType() const { return m_ActiveWindowType; }
		EditorTabManager* GetTabManager() { return m_TabManager; }
		int GetTabCount() const { return m_TabManager->GetTabCount(); }
		std::string GetCurrentTabName() const { return m_TabManager->GetActiveTabName(); }
		glm::vec2 GetEditorWindowSize() const;
		float GetEditorWindowWidth() const;
		float GetEditorWindowHeight() const;
		glm::vec2 GetLeftSidebarSize() const;
		glm::vec2 GetRightSidebarSize() const;
		glm::vec2 GetBottomBarSize() const; 
		float GetViewportScalerX() const; 
		float GetViewportScalerY() const; 
		glm::vec2 GetMenuBarSize() const; 
		glm::vec2 GetTabBarSize() const;

		bool IsEnabled() const { return m_Enabled; }

		static EditorLayer* Get() { return m_Instance; }

	private:
		void RenderViewport();                
		void RenderViewport(Ref<Image> image); 

        void DrawImGuizmo(Entity entity);
		void HandleExternalWindows();

		void UI_MainMenuBar();
		void UI_HandleManualWindowResize();
		void UI_SceneContent();
		void UI_CreateMenu();
		void UI_RenderSettingsWindow();
		void UI_SyncEditorWindowSizes(const glm::vec2& viewportSize); // Update potential changes in editor docking window sizes/positions
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

		Ref<Framebuffer>& GetViewportFramebuffer() { return m_ViewportFramebuffer; }

		Entity CreateCube();

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

		ImVec2 LastSidebarLeftPos = ImVec2(0, 0);
		ImVec2 LastSidebarLeftSize = ImVec2(0, 0);
		ImVec2 LastSidebarRightPos = ImVec2(0, 0);
		ImVec2 LastSidebarRightSize = ImVec2(0, 0);
		ImVec2 LastBottomBarPos = ImVec2(0, 0);
		ImVec2 LastBottomBarSize = ImVec2(0, 0);

		inline static std::string SelectedEntityWindow = UI_SIDEBAR_RIGHT;

	private:
		Ref<Scene> m_Scene = nullptr;
		bool m_Enabled = true;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_SecondViewportBounds[2];
		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditorLayer = false;
		int m_GizmoType = GizmoType::Translate;
		int m_CurrentTabCount = 0; // Incremented to 1 after EditorLayer is initialized

        EventCallbackFn m_EventCallback;

		Ref<Framebuffer> m_ViewportFramebuffer;
		Ref<EditorCamera> m_EditorCamera;

		SceneManagerPanel* m_SceneManagerPanel = nullptr;

		Ref<Project> m_Project;

		NodeEditor* m_NodeEditor;
		EditorTabManager* m_TabManager;
		ComponentEditor* m_ComponentEditor;
		ContentBrowser* m_ContentBrowser;

		Window* m_Window = nullptr;
		WindowType m_ActiveWindowType;

		friend class Physics2D; // For getting UI window size when raycasting
		friend class NodeEditorTab;
		friend class MaterialEditorTab;
		friend class SceneManagerPanel;
		friend class Renderer;
		
		inline static EditorLayer* m_Instance = nullptr;
	};

}