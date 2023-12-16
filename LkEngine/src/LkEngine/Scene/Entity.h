#pragma once

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>


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

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			if (HasComponent<T>())
				return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		// template<typename T>
		template<typename T, typename... ARGS>
		void AddExistingComponent(T) 
		{
			m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<ARGS>(args)...);
		}

		bool IsValid() const 
		{ 
			return m_Scene != nullptr;
		}

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }
		SpriteComponent& GetSpriteComponent() { return GetComponent<SpriteComponent>(); }

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		// REMOVE 
		glm::vec3 m_Position{ 1.0f, 1.0f, 1.0f };
		glm::vec3 m_Translation{ 0.0f, 0.0f, 0.0f };
		glm::quat m_Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Inertia{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_LinearVelocity{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_AngularVelocity{ 0.0f, 0.0f, 0.0f };
	};

}
