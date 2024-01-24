#pragma once

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include <glm/glm.hpp>

#include <entt/entt.hpp>

#include <vector>


namespace LkEngine {

	class Entity 
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		~Entity() = default;

		void OnUpdate(float ts) {}

		template<typename T, typename ...ARGS>
		T& AddComponent(ARGS&&... args)
		{
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<ARGS>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename... T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T...>(m_EntityHandle);
		}

		template<typename... T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.has<T...>(m_EntityHandle);
		}

		template<typename... T>
		bool HasAny()
		{
			return m_Scene->m_Registry.any<T...>(m_EntityHandle);
		}

		template<typename... T>
		bool HasAny() const
		{
			return m_Scene->m_Registry.any<T...>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity does not have that component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity does not have that component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity does not have that component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponentIfExists()
		{
			m_Scene->m_Registry.remove_if_exists<T>(m_EntityHandle);
		}

		template<typename T, typename... ARGS>
		void AddExistingComponent(T, ARGS&&... args) 
		{
			m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<ARGS>(args)...);
		}

		const std::string& Name() { return GetComponent<TagComponent>().Tag; }
		TagComponent& Tag() { return GetComponent<TagComponent>(); }
		TransformComponent& Transform() { return GetComponent<TransformComponent>(); }
		SpriteComponent& Sprite() { return GetComponent<SpriteComponent>(); }
		CameraComponent& Camera() { return GetComponent<CameraComponent>(); }
		MaterialComponent& Material() { return GetComponent<MaterialComponent>(); }
		RigidBody2DComponent& RigidBody2D() { return GetComponent<RigidBody2DComponent>(); }
		BoxCollider2DComponent& BoxCollider2D() { return GetComponent<BoxCollider2DComponent>(); }

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator TransformComponent&() { return GetComponent<TransformComponent>(); }

		operator bool () const;
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

		Entity GetParent() const;
		UUID GetUUID() { return GetComponent<IDComponent>().ID; }

		void SetParent(Entity parent)
		{
			Entity currentParent = GetParent();
			if (currentParent == parent)
				return;

			// If changing parent, remove child from existing parent
			if (currentParent)
				currentParent.RemoveChild(*this);

			// Setting to null is okay
			SetParentUUID(parent.GetUUID());

			if (parent)
			{
				auto& parentChildren = parent.Children();
				UUID uuid = GetUUID();
				if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
					parentChildren.emplace_back(GetUUID());
			}
		}

		UUID GetSceneUUID() const;

		void SetParentUUID(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }
		UUID GetParentUUID() const { return GetComponent<RelationshipComponent>().ParentHandle; }
		std::vector<UUID>& Children() { return GetComponent<RelationshipComponent>().Children; }
		const std::vector<UUID>& Children() const { return GetComponent<RelationshipComponent>().Children; }

		bool RemoveChild(Entity child)
		{
			UUID childId = child.GetUUID();
			std::vector<UUID>& children = Children();
			auto it = std::find(children.begin(), children.end(), childId);
			if (it != children.end())
			{
				children.erase(it);
				return true;
			}

			return false;
		}

		bool IsAncestorOf(Entity entity) const;
		bool IsDescendantOf(Entity entity) const { return entity.IsAncestorOf(*this); }

		const entt::entity& _ENTITY_HANDLE() const { return m_EntityHandle; } // Temporary debugging

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		friend class Scene;
		friend class SceneManagerPanel;
		friend class Editor;
	};

}
