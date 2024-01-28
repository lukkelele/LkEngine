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

	class Editor : public Layer
	{
	public:
		enum class WindowType
		{
			None = 0,
			Viewport,  // Editor Viewport, 'normal' mode
			NodeEditor,
		};

		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate = 7 << 3,
			Scale = 7 << 6
		};

	public:
		Editor();
		~Editor() = default;

		void OnUpdate();
		void OnEvent(Event& e) override;

		void OnRender();
		void OnImGuiRender();

		bool IsEnabled() { return m_Enabled; }
		Ref<EditorCamera> GetEditorCamera() { return m_EditorCamera; }
		void SetScene(Ref<Scene> scene);
		Ref<Scene> GetCurrentScene() { return m_Scene; }

		void UI_HandleManualWindowResize();
		void UI_SceneContent();
		void UI_CreateMenu();
		void UI_RenderSettingsWindow();
		const char* UI_GetSelectedEntityWindowName() { return SelectedEntityWindow.c_str(); }
		void SetUpdateWindowFlag(bool flag);

		void UI_SyncEditorWindowSizes(const glm::vec2& viewportSize); // Update potential changes in editor docking window sizes/positions
		void UI_ShowMouseDetails();
		void UI_ShowViewportAndWindowDetails();
		void UI_ShowEditorWindowsDetails();
		void UI_ClearColorModificationMenu();

		//Ref<Framebuffer> GetFramebuffer() { return m_Framebuffer; }

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

		static Editor* Get() { return m_Instance; }

	private:
		void RenderViewport();                
		void RenderViewport(Ref<Image> image); 

        void DrawImGuizmo(Entity entity);
		void HandleExternalWindows();

		void UpdateLeftSidebarSize(ImGuiViewport* viewport);
		void UpdateRightSidebarSize(ImGuiViewport* viewport);


	public:
		// Flag to determine if an item is currently being created
		inline static bool InCreateItemProcess = false; // if true, the potentially created item is shown in the editor window
		inline static ImVec2 SelectedEntityMenuSize = { 0, 440 }; // TODO: REMOVE/UPDATE

		glm::vec2 EditorViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f} };
		glm::vec2 EditorViewportPos = { 0.0f, 0.0f };
		glm::vec2 EditorWindowPos = { 0.0f, 0.0f };
		glm::vec2 EditorWindowSize = { 0.0f, 0.0f };
		glm::vec2 ViewportScalers = { 1.0f, 1.0f };

		bool ShouldUpdateWindowSizes = true;

		bool ShowRenderSettingsWindow = false;
		bool m_FillSidebarsVertically = true; // Always fill out sidebars vertically

		ImVec2 last_sidebar_left_pos = ImVec2(0, 0);
		ImVec2 last_sidebar_left_size = ImVec2(0, 0);
		ImVec2 last_sidebar_right_pos = ImVec2(0, 0);
		ImVec2 last_sidebar_right_size = ImVec2(0, 0);
		ImVec2 last_bottombar_pos = ImVec2(0, 0);
		ImVec2 last_bottombar_size = ImVec2(0, 0);

		inline static std::string SelectedEntityWindow = UI_SIDEBAR_RIGHT;

	private:
		Ref<Scene> m_Scene = nullptr;
		bool m_Enabled = true;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_SecondViewportBounds[2];
		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditor = false;
		int m_GizmoType = GizmoType::Translate;
		int m_CurrentTabCount = 0; // Incremented to 1 after Editor is initialized

		//Ref<Framebuffer> m_Framebuffer;

		Ref<EditorCamera> m_EditorCamera = nullptr;
		SceneManagerPanel* m_SceneManagerPanel = nullptr;

		Ref<Project> m_TargetProject;
		Ref<Project> m_CachedProjects[5];

		NodeEditor* m_NodeEditor;
		EditorTabManager* m_TabManager;
		ComponentEditor m_ComponentEditor;
		ContentBrowser* m_ContentBrowser;

		Window* m_Window = nullptr;
		WindowType m_ActiveWindowType;

		friend class Physics2D; // For getting UI window size when raycasting
		friend class NodeEditorTab;
		friend class MaterialEditorTab;
		friend class SceneManagerPanel;
		
		inline static Editor* m_Instance = nullptr;
	};

}