#pragma once

#include <vector>

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"

#include <glm/glm.hpp>

#include <entt/entt.hpp>


namespace LkEngine {

	class LEntity : public LObject
	{
	public:
		LEntity() = default;
		LEntity(entt::entity InHandle, LScene* InScene);
		LEntity(entt::entity InHandle, TObjectPtr<LScene> InScene);
		~LEntity() = default;

		LEntity(const LEntity& Other) = default;

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

		FORCEINLINE const std::string& Name() { return GetComponent<LTagComponent>().Tag; }
		FORCEINLINE LTagComponent& Tag() { return GetComponent<LTagComponent>(); }
		FORCEINLINE LTransformComponent& Transform() { return GetComponent<LTransformComponent>(); }
		//FORCEINLINE LSpriteComponent& Sprite() { return GetComponent<LSpriteComponent>(); }
		//FORCEINLINE LCameraComponent& Camera() { return GetComponent<LCameraComponent>(); }

		//FORCEINLINE LRigidBody2DComponent& RigidBody2D() { return GetComponent<LRigidBody2DComponent>(); }
		//FORCEINLINE LBoxCollider2DComponent& BoxCollider2D() { return GetComponent<LBoxCollider2DComponent>(); }

		/** 
		 * GetMesh 
		 * 
		 *  Throws error if entity doesn't have a mesh component. 
		 */
		FORCEINLINE LMeshComponent& GetMesh() 
		{ 
			return GetComponent<LMeshComponent>(); 
		}

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator LTransformComponent&() { return GetComponent<LTransformComponent>(); }

		operator bool () const;
		bool operator==(const LEntity& Other) const { return m_EntityHandle == Other.m_EntityHandle && m_Scene == Other.m_Scene; }
		bool operator!=(const LEntity& Other) const { return !(*this == Other); }

		LEntity GetParent() const;
		UUID GetUUID() { return GetComponent<LIDComponent>().ID; }

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

		FORCEINLINE void SetParentUUID(UUID parent) 
		{ 
			GetComponent<LRelationshipComponent>().ParentHandle = parent; 
		}

		FORCEINLINE UUID GetParentUUID() const 
		{ 
			return GetComponent<LRelationshipComponent>().ParentHandle; 
		}

		FORCEINLINE std::vector<UUID>& GetChildren() 
		{ 
			return GetComponent<LRelationshipComponent>().Children; 
		}

		const std::vector<UUID>& GetChildren() const { return GetComponent<LRelationshipComponent>().Children; }

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

	private:
		entt::entity m_EntityHandle{ entt::null };

		TObjectPtr<LScene> m_Scene{};

		friend class LEditorLayer;
		friend class LScene;
		friend class LSceneManagerPanel; /// REWORK THIS

		LCLASS(LEntity);
	};

}
