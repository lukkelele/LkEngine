#pragma once
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include <entt/entt.hpp>


namespace LkEngine {

	class EditorLayer
	{
	public:
		EditorLayer(s_ptr<Scene> scene);
		~EditorLayer() = default;

		void OnImGuiRender();
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void SelectEntity(Entity& entity);
		std::pair<float, float> GetMouseViewportSpace(bool primary_viewport);

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		static void Select(Entity& entity) { SelectedEntity = entity; }

	public:
		static Entity SelectedEntity;
	private:

		s_ptr<Scene> m_Scene = nullptr;

		// 7 equals 0b111, the gizmo types are set 1 for each axis at an offset of 3 bits
		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate	  = 7 << 3,
			Scale     = 7 << 6
		};

		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_SecondViewportBounds[2];

		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditor = false;

		int m_GizmoType = GizmoType::Translate;

		friend class World;
	};

}