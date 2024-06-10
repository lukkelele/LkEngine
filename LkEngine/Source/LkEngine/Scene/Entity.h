#pragma once

#include <vector>

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include <glm/glm.hpp>

#include <entt/entt.hpp>



namespace LkEngine {

	class LEntity : public LObject
	{
	public:
		LEntity() = default;
		LEntity(entt::entity handle, Scene* scene);
		LEntity(const LEntity& Other) = default;
		~LEntity() = default;

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
		RigidBody2DComponent& RigidBody2D() { return GetComponent<RigidBody2DComponent>(); }
		BoxCollider2DComponent& BoxCollider2D() { return GetComponent<BoxCollider2DComponent>(); }
		MeshComponent& Mesh() { return GetComponent<MeshComponent>(); }

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator TransformComponent&() { return GetComponent<TransformComponent>(); }

		operator bool () const;
		bool operator==(const LEntity& Other) const { return m_EntityHandle == Other.m_EntityHandle && m_Scene == Other.m_Scene; }
		bool operator!=(const LEntity& Other) const { return !(*this == Other); }

		LEntity GetParent() const;
		UUID GetUUID() { return GetComponent<IDComponent>().ID; }

		void SetParent(LEntity parent)
		{
			LEntity CurrentParent = GetParent();
			if (CurrentParent == parent)
			{
				return;
			}

			// If changing parent, remove child from existing parent
			if (CurrentParent)
			{
				CurrentParent.RemoveChild(*this);
			}

			// Setting to null is okay
			SetParentUUID(parent.GetUUID());

			if (parent)
			{
				auto& parentChildren = parent.GetChildren();
				const UUID uuid = GetUUID();
				if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
				{
					parentChildren.emplace_back(GetUUID());
				}
			}
		}

		UUID GetSceneUUID() const;

		void SetParentUUID(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }
		UUID GetParentUUID() const { return GetComponent<RelationshipComponent>().ParentHandle; }
		std::vector<UUID>& GetChildren() { return GetComponent<RelationshipComponent>().Children; }
		const std::vector<UUID>& GetChildren() const { return GetComponent<RelationshipComponent>().Children; }

		bool RemoveChild(LEntity Child)
		{
			const UUID ChildID = Child.GetUUID();
			std::vector<UUID>& Children = GetChildren();
			/* FIXME: Use compressed if statement */
			auto Iter = std::find(Children.begin(), Children.end(), ChildID);
			if (Iter != Children.end())
			{
				Children.erase(Iter);
				return true;
			}

			return false;
		}

		bool IsAncestorOf(LEntity entity) const;
		bool IsDescendantOf(LEntity entity) const { return entity.IsAncestorOf(*this); }

		const entt::entity& _ENTITY_HANDLE() const { return m_EntityHandle; } // Temporary debugging

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		friend class Scene;
		friend class SceneManagerPanel;
		friend class EditorLayer;
	};

}
