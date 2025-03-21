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
		LEntity();
		LEntity(entt::entity InHandle, TObjectPtr<LScene> InScene);
		LEntity(entt::entity InHandle, LScene* InScene);
		~LEntity() = default;

		LEntity(const LEntity& Other) = default;

		void Tick(float ts) {}

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
			LK_CORE_ASSERT(HasComponent<T>(), "Entity '{}' does not have that component", Handle);
			return Scene->Registry.get<T>(Handle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			LK_CORE_ASSERT(HasComponent<T>(), "Entity '{}' does not have that component", Handle);
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
		FORCEINLINE LMeshComponent& GetMesh() { return GetComponent<LMeshComponent>(); }

		operator uint32_t() const { return static_cast<uint32_t>(Handle); }
		operator entt::entity() const { return Handle; }
		operator LUUID() const { return GetUUID(); }
		operator LTransformComponent&() { return GetComponent<LTransformComponent>(); }
		operator bool() const;

		bool operator==(const LEntity& Other) const 
		{ 
			return ((Handle == Other.Handle) && (Scene == Other.Scene)); 
		}

		bool operator!=(const LEntity& Other) const { return !(*this == Other); }

		LEntity GetParent();

		FORCEINLINE LUUID GetUUID() const 
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
				std::vector<LUUID>& Children = InParent.GetChildren();
				const LUUID uuid = GetUUID();

				if (std::find(Children.begin(), Children.end(), uuid) == Children.end())
				{
					Children.emplace_back(GetUUID());
				}
			}
		}

		LUUID GetSceneUUID() const;

		FORCEINLINE void SetParentUUID(LUUID parent) 
		{ 
			GetComponent<LRelationshipComponent>().ParentHandle = parent; 
		}

		FORCEINLINE LUUID GetParentUUID() const 
		{ 
			return GetComponent<LRelationshipComponent>().ParentHandle; 
		}

		FORCEINLINE std::vector<LUUID>& GetChildren() 
		{ 
			return GetComponent<LRelationshipComponent>().Children; 
		}

		const std::vector<LUUID>& GetChildren() const { return GetComponent<LRelationshipComponent>().Children; }

		bool RemoveChild(LEntity Child)
		{
			const LUUID ChildID = Child.GetUUID();
			std::vector<LUUID>& Children = GetChildren();

			/* FIXME: Use compressed if statement */
			auto Iter = std::find(Children.begin(), Children.end(), ChildID);
			if (Iter != Children.end())
			{
				Children.erase(Iter);
				return true;
			}

			return false;
		}

		bool IsAncestorOf(LEntity InEntity);

		FORCEINLINE bool IsDescendantOf(LEntity Entity) const 
		{ 
			return Entity.IsAncestorOf(*this); 
		}

	private:
		entt::entity Handle{ entt::null };

		TObjectPtr<LScene> Scene{};

		friend class LEditorLayer;
		friend class LScene;
		friend class LSceneManagerPanel;

		LCLASS(LEntity);
	};

}

/* Logging formatters. */
template<>
struct LK_FMT_LIB::formatter<LkEngine::LEntity> : LK_FMT_LIB::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const LkEngine::LEntity& Entity, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<entt::id_type>(Entity));
    }
};

template<>
struct LK_FMT_LIB::formatter<entt::entity> : LK_FMT_LIB::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const entt::entity& Entity, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<entt::id_type>(Entity));
    }
};

