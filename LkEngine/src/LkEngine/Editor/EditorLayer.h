#pragma once
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include <entt/entt.hpp>


namespace LkEngine {

	enum LK_OBJECT
	{
		LINE, CUBE, SPHERE
	};

	class EditorLayer
	{
	public:
		EditorLayer(s_ptr<Scene> scene);
		~EditorLayer() = default;

		void OnImGuiRender();
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void SelectEntity(Entity& entity);

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

	private:
		s_ptr<Scene> m_Scene = nullptr;
		static Entity m_SelectedEntity;

		// 7 equals 0b111, the gizmo types are set 1 for each axis at an offset of 3 bits
		enum GizmoType
		{
			Translate = 7 << 0,
			Rotate	  = 7 << 3,
			Scale     = 7 << 6
		};

		int m_GizmoType = GizmoType::Translate;

		friend class World;
	};

}