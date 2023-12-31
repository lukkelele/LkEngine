#pragma once

#include "LkEngine/Core/Layer.h"
#include "LkEngine/Math/Math.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/UI/UICore.h"

#include "EditorTabManager.h"
#include "NodeEditor/NodeEditor.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
// TODO: Fix so opengl includes are handles in entrypoint
#include <imgui/imgui_impl_opengl3.h>

#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

    constexpr const char* UI_CORE_VIEWPORT = "##lkengine-core-viewport";
    constexpr const char* UI_RENDER_WINDOW = "##lkengine-render-window";
    constexpr const char* UI_TOP_BAR = "##lkengine-top-bar";
    constexpr const char* UI_BOTTOM_BAR = "##lkengine-lower-bar";
    constexpr const char* UI_SIDEBAR_LEFT = "##lkengine-sidebar-left";
    constexpr const char* UI_SIDEBAR_RIGHT = "##lkengine-sidebar-right";
    constexpr const char* UI_SELECTED_ENTITY_INFO = "##lkengine-selected-entity-info";

	class Scene;

	class Editor
	{
	public:
		enum class WindowType
		{
			None = 0,
			Viewport, // Editor Viewport, 'normal' mode
			NodeEditor,
		};

		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate	  = 7 << 3,
			Scale     = 7 << 6
		};

	public:
		Editor();
		~Editor() = default;

		static Editor* Get() { return s_Instance; }

		void RenderImGui();
		bool IsEnabled() { return m_Enabled; }
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void SetScene(Scene& scene) { m_Scene = &scene; }
		std::pair<float, float> GetMouseViewportSpace(bool primary_viewport);
		Entity GetSelectedEntity() { return SelectedEntity; }
		void SelectEntity(Entity& entity);
		void SetSelectedEntity(Entity& entity);

		EditorCamera* GetEditorCamera() { return m_EditorCamera; }

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void UI_HandleManualWindowResize();
		void UI_SceneContent();
		void UI_CreateMenu();
        void UI_SelectedEntityProperties();
		void UI_RenderSettingsInformation();
		bool IsEntitySelected() const;
		const char* UI_GetSelectedEntityWindowName() { return SelectedEntityWindow.c_str(); }
		void SetUpdateWindowFlag(bool flag); 

		glm::vec2 GetEditorWindowSize() const;
		float GetEditorWindowWidth() const;
		float GetEditorWindowHeight() const;
		uint64_t GetSelectedEntityID() const { return SelectedEntityID; }
		glm::vec2 GetLeftSidebarSize() const { return LeftSidebarSize; }
		glm::vec2 GetRightSidebarSize() const { return RightSidebarSize; }
		glm::vec2 GetBottomBarSize() const { return BottomBarSize; }
		float GetViewportScalerX() const { return ViewportScalers.x; }
		float GetViewportScalerY() const { return ViewportScalers.y; }
		glm::vec2 GetMenuBarSize() const { return MenuBarSize; }
		glm::vec2 GetTabBarSize() const { return TabBarSize; }

		WindowType GetCurrentWindowType() const { return m_ActiveWindowType; }

		int GetTabCount() const { return m_TabManager.GetTabCount(); }
		std::string GetCurrentTabName() const { return m_TabManager.GetActiveTabName(); }

		void UI_SyncEditorWindowSizes(const glm::vec2& viewportSize); // Update potential changes in editor docking window sizes/positions
		void UI_ShowMouseDetails();
		void UI_ShowViewportAndWindowDetails();
		void UI_ShowEditorWindowsDetails();
		void UI_BackgroundColorModificationMenu();

	private:
        void DrawImGuizmo(Entity& entity);
		void HandleExternalWindows();

		void UpdateLeftSidebarSize(ImGuiViewport* viewport);
		void UpdateRightSidebarSize(ImGuiViewport* viewport);

		//void RenderViewport();                 // TODO
		//void RenderViewport(s_ptr<Image> img); // TODO
	public:
		// Flag to determine if an item is currently being created
		inline static bool InCreateItemProcess = false; // if true, the potentially created item is shown in the editor window
		inline static ImVec2 SelectedEntityMenuSize = { 0, 440 }; // TODO: REMOVE/UPDATE
		Entity SelectedEntity;
		uint64_t SelectedEntityID = 0;

		glm::vec2 EditorViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f} };
		glm::vec2 EditorViewportPos = { 0.0f, 0.0f };
		glm::vec2 EditorWindowPos = { 0.0f, 0.0f };
		glm::vec2 EditorWindowSize = { 0.0f, 0.0f };
		glm::vec2 ViewportScalers = { 1.0f, 1.0f };
		glm::vec2 MenuBarSize = { 0.0f, 30.0f };
		glm::vec2 TabBarSize = { 0.0f, 0.0f };
		glm::vec2 BottomBarSize = { 0.0f, 0.0f };
		glm::vec2 LeftSidebarSize = { 0.0f, 0.0f };
		glm::vec2 RightSidebarSize = { 0.0f, 0.0f };
		glm::vec2 BottomBarPos = { 0.0f, 0.0f };
		glm::vec2 LeftSidebarPos = { 0.0f, 0.0f };
		glm::vec2 RightSidebarPos = { 0.0f, 0.0f };

		bool ShouldUpdateWindowSizes = true;

		bool ShowRenderInformationWindow = false;
		bool m_FillSidebarsVertically = true; // Always fill out sidebars vertically

		ImVec2 last_sidebar_left_pos = ImVec2(0, 0);
		ImVec2 last_sidebar_left_size = ImVec2(0, 0);
		ImVec2 last_sidebar_right_pos = ImVec2(0, 0);
		ImVec2 last_sidebar_right_size = ImVec2(0, 0);
		ImVec2 last_bottombar_pos = ImVec2(0, 0);
		ImVec2 last_bottombar_size = ImVec2(0, 0);

		// Window where the selected entity settings will be shown in
		inline static std::string SelectedEntityWindow = UI_SIDEBAR_RIGHT;
		//std::vector<std::string> m_Tabs;
		//std::string m_CurrentTab = "";

	private:
		Scene* m_Scene = nullptr;
		bool m_Enabled = true;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_SecondViewportBounds[2];
		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditor = false;
		int m_GizmoType = GizmoType::Translate;
		int m_CurrentTabCount = 0; // Incremented to 1 after Editor is initialized

		NodeEditor* m_NodeEditor = nullptr;
		EditorCamera* m_EditorCamera = nullptr;
		EditorTabManager m_TabManager;

		WindowType m_ActiveWindowType;

		friend class Physics2D; // For getting UI window size when raycasting

		inline static Editor* s_Instance = nullptr;
	};

}