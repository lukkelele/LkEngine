#pragma once
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Scene/Scene.h"
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
    constexpr const char* UI_SELECTED_ENTITY_DETAILS = "##lkengine-selected-entity-details";

	class EditorLayer : public Layer
	{
	public:
		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate	  = 7 << 3,
			Scale     = 7 << 6
		};

		//EditorLayer(s_ptr<Scene> scene);
		EditorLayer(Scene& scene);
		~EditorLayer() = default;

		static EditorLayer* Get() { return s_Instance; }
		static bool IsEnabled() { return Enabled; }
		static Entity GetSelectedEntity() { return SelectedEntity; }

		void OnImGuiRender();
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void SelectEntity(Entity& entity);
		std::pair<float, float> GetMouseViewportSpace(bool primary_viewport);
		static void Select(Entity& entity) { SelectedEntityID = entity.GetComponent<IDComponent>().ID; }

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void UI_HandleManualWindowResize();
		void UI_SceneMenu();
		void UI_SceneContent();
        void UI_SelectedEntity();
		//std::pair<float, float> GetViewportPos();

		glm::vec2 GetEditorWindowSize() const;
		glm::vec2 GetViewportBounds(int viewportIndex, int boundIndex);

	private:
		void RenderViewport(); // TODO
		//void RenderViewport(s_ptr<Image> img);
        void DrawImGuizmo(Entity& entity);

	public:
		inline static Entity SelectedEntity;
		inline static uint64_t SelectedEntityID = 0;
        static std::string SelectedEntityLabel; // remove
		static ImVec2 SelectedEntityMenuSize;
		static glm::vec2 EditorViewportBounds[2];
		static glm::vec2 EditorViewportPos;
		static glm::vec2 EditorWindowPos;
		static glm::vec2 ViewportScalers;
		static glm::vec2 EditorWindowSize;
		static bool Enabled;
	private:
		s_ptr<Scene> m_Scene = nullptr;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_SecondViewportBounds[2];
		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditor = false;
		int m_GizmoType = GizmoType::Translate;

		inline static EditorLayer* s_Instance = nullptr;
	};

}