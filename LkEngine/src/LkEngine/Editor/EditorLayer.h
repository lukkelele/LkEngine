#pragma once

#include "LkEngine/Core/Layer.h"
#include "LkEngine/Math/Math.h"
//#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

#include <entt/entt.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
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

	class EditorLayer : public Layer
	{
	public:
		EditorLayer(Scene& scene);
		~EditorLayer() = default;

		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate	  = 7 << 3,
			Scale     = 7 << 6
		};

		static EditorLayer* Get() { return s_Instance; }
		static s_ptr<EditorLayer> Create(Scene& scene) { return std::make_shared<EditorLayer>(scene); }
		static bool IsEnabled() { return Enabled; }
		static Entity GetSelectedEntity() { return SelectedEntity; }
		static const char* UI_GetSelectedEntityWindowName() { return SelectedEntityWindow.c_str(); }
		void OnImGuiRender();
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void SelectEntity(Entity& entity);
		void SetScene(Scene& scene) { m_Scene = &scene; }
		std::pair<float, float> GetMouseViewportSpace(bool primary_viewport);
		//static void Select(Entity& entity); { SelectedEntityID = entity.GetComponent<IDComponent>().ID; }
		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
		void UI_HandleManualWindowResize();
		void UI_SceneMenu();
		void UI_SceneContent();
        void UI_SelectedEntityProperties();
		void UI_RenderSettingsInformation();
		glm::vec2 GetEditorWindowSize() const;
		bool IsEntitySelected() const;
		
		static glm::vec2 GetTopBarSize();
		static glm::vec2 GetCenteredMousePos();

	private:
		//void RenderViewport(); // TODO
		//void RenderViewport(s_ptr<Image> img);
        void DrawImGuizmo(Entity& entity);
		void HandleExternalWindows();

	public:
		static ImVec2 SelectedEntityMenuSize;
		inline static bool Enabled = true;
		inline static Entity SelectedEntity;
		inline static uint64_t SelectedEntityID = 0;
		inline static glm::vec2 EditorViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f} };
		inline static glm::vec2 EditorViewportPos = { 0.0f, 0.0f };
		inline static glm::vec2 EditorWindowPos = { 0.0f, 0.0f };
		inline static glm::vec2 ViewportScalers = { 1.0f, 1.0f };
		inline static glm::vec2 EditorWindowSize = { 0.0f, 0.0f };
		inline static glm::vec2 TopBarSize = { 0.0f, 30.0f };
		inline static glm::vec2 TopBarFramePadding = { 5.0f, 5.0f };
		inline static glm::vec2 BottomBarSize = { 0.0f, 0.0f };
		inline static glm::vec2 LeftSidebarSize = { 0.0f, 0.0f };
		inline static glm::vec2 LeftSidebarPadding = { 0.0f, 0.0f };
		inline static float TopBarHeight = 0.0f;
		inline static bool ShowRenderInformationWindow = false;

		// Window where the selected entity settings will be shown in
		inline static std::string SelectedEntityWindow = UI_SIDEBAR_RIGHT;

	private:
		//s_ptr<Scene> m_Scene = nullptr;
		Scene* m_Scene = nullptr;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_SecondViewportBounds[2];
		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditor = false;
		int m_GizmoType = GizmoType::Translate;

		inline static EditorLayer* s_Instance = nullptr;
	};

}