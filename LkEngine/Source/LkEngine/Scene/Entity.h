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
		LEntity(entt::entity InHandle, TObjectPtr<LScene> InScene);
		~LEntity() = default;

		LEntity(const LEntity& Other) = default;

		void OnUpdate(float ts) {}

		template<typename T, typename ...TArgs>
		T& AddComponent(TArgs&&... Args)
		{
			T& Component = Scene->Registry.emplace<T>(Handle, std::forward<TArgs>(Args)...);
			Scene->OnComponentAdded<T>(*this, Component);

			return Component;
		}

		template<typename... T>
		bool HasComponent()
		{
			return Scene->Registry.all_of<T...>(Handle);
		}

		template<typename... T>
		bool HasComponent() const
		{
			return Scene->Registry.all_of<T...>(Handle);
		}

		template<typename... T>
		bool HasAny()
		{
			return Scene->Registry.any_of<T...>(Handle);
		}

		template<typename... T>
		bool HasAny() const
		{
			return Scene->Registry.any_of<T...>(Handle);
		}

		template<typename T>
		T& GetComponent()
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity does not have that component");
			return Scene->Registry.get<T>(Handle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity does not have that component");
			return Scene->Registry.get<T>(Handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity does not have that component");
			Scene->Registry.remove<T>(Handle);
		}

		template<typename T>
		void RemoveComponentIfExists()
		{
			if (Scene->Registry.all_of<T>(Handle))
			{
				Scene->Registry.remove<T>(Handle);
			}
		}

		template<typename T, typename... ARGS>
		void AddExistingComponent(T, ARGS&&... args) 
		{
			Scene->Registry.emplace<T>(Handle, std::forward<ARGS>(args)...);
		}

		FORCEINLINE const std::string& Name() const { return GetComponent<LTagComponent>().Tag; }
		FORCEINLINE LTagComponent& Tag() { return GetComponent<LTagComponent>(); }
		FORCEINLINE LTransformComponent& Transform() { return GetComponent<LTransformComponent>(); }

		/** GetMesh, throws error if entity doesn't have the component.*/
		FORCEINLINE LMeshComponent& GetMesh() 
		{ 
			return GetComponent<LMeshComponent>(); 
		}

		operator uint32_t() const { return static_cast<uint32_t>(Handle); }
		operator entt::entity() const { return Handle; }
		operator LTransformComponent&() { return GetComponent<LTransformComponent>(); }
		operator bool() const;

		bool operator==(const LEntity& Other) const 
		{ 
			return ((Handle == Other.Handle) && (Scene == Other.Scene)); 
		}

		bool operator!=(const LEntity& Other) const { return !(*this == Other); }

		LEntity GetParent() const;

		FORCEINLINE UUID GetUUID() const 
		{ 
			return GetComponent<LIDComponent>().ID; 
		}

		FORCEINLINE void SetParent(LEntity InParent)
		{
			LEntity CurrentParent = GetParent();
			if (CurrentParent == InParent)
			{
				return;
			}

			/* If changing parent, remove child from existing attached parent. */
			if (CurrentParent)
			{
				CurrentParent.RemoveChild(*this);
			}

			/* Setting to null is okay. */
			SetParentUUID(InParent.GetUUID());

			if (InParent)
			{
				std::vector<UUID>& Children = InParent.GetChildren();
				const UUID uuid = GetUUID();

				if (std::find(Children.begin(), Children.end(), uuid) == Children.end())
				{
					Children.emplace_back(GetUUID());
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

		FORCEINLINE bool IsDescendantOf(LEntity Entity) const 
		{ 
			return Entity.IsAncestorOf(*this); 
		}

	private:
		entt::entity Handle{ entt::null };

		TObjectPtr<LScene> Scene{};

		friend class LEditorLayer;
		friend class LScene;
		friend class LSceneManagerPanel; /// REWORK THIS

		LCLASS(LEntity);
	};

}

/* Logging formatters. */
template<>
struct std::formatter<LkEngine::LEntity> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const LkEngine::LEntity& Entity, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<entt::id_type>(Entity));
    }
};

template<>
struct std::formatter<entt::entity> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const entt::entity& Entity, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<entt::id_type>(Entity));
    }
};

