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

		const std::string& Name() { return GetComponent<TagComponent>().Tag; }
		UUID UUID() { return GetComponent<IDComponent>().ID; }
		TransformComponent& Transform() { return GetComponent<TransformComponent>(); }
		SpriteComponent& Sprite() { return GetComponent<SpriteComponent>(); }
		CameraComponent& Camera() { return GetComponent<CameraComponent>(); }
		MaterialComponent& Material() { return GetComponent<MaterialComponent>(); }
		RigidBody2DComponent& RigidBody2D() { return GetComponent<RigidBody2DComponent>(); }
		BoxCollider2DComponent& BoxCollider2D() { return GetComponent<BoxCollider2DComponent>(); }

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator TransformComponent&() { return GetComponent<TransformComponent>(); }
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
