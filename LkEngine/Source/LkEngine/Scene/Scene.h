#pragma once

#include <unordered_set>

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/Time/Timer.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Asset/Asset.h"

#include "Components.h"

#include "LkEngine/Editor/EditorCamera.h"

#include <entt/entt.hpp>

namespace LkEngine {

	class LEntity;
	class World;
	class LRenderer;
	class LSceneRenderer;
	struct Physics2DSpecification;

	LK_DECLARE_MULTICAST_DELEGATE(FOnSceneSetActive, const TObjectPtr<LScene>&);
	LK_DECLARE_MULTICAST_DELEGATE(FOnSceneCreated,   const TObjectPtr<LScene>&);
	extern FOnSceneSetActive GOnSceneSetActive;
	extern FOnSceneCreated   GOnSceneCreated;

	enum class ESceneType
	{
		Normal = 0,
		Editor,
	};

	enum class ESceneState
	{
		Edit = 0,
		Play,
		Pause,
		Simulate
	};

	class LScene : public LAsset
	{
	public:
		LScene(const std::string& SceneName, const bool IsEditorScene = false);
		LScene() = delete;
		~LScene() = default;

		void OnRenderRuntime(TObjectPtr<LSceneRenderer> SceneRenderer, const float DeltaTime);
		void OnUpdateRuntime(const float InDeltaTime);
		void OnRenderEditor(TObjectPtr<LSceneRenderer> SceneRenderer, LEditorCamera& EditorCamera, const float DeltaTime);
		void OnUpdateEditor(const float InDeltaTime);

		void EndScene(); /// FIXME
		
		FORCEINLINE LUUID GetUUID() const { return SceneID; }
		LEntity GetMainCameraEntity();

		template<typename T = LEntity>
		std::vector<T> GetEntities();

		uint64_t GetEntityCount() const { return static_cast<uint64_t>(EntityMap.size()); }
		void SortEntities();

		LEntity FindEntity(std::string_view EntityName);
		LEntity GetEntityWithUUID(const LUUID UUID);

		FORCEINLINE entt::registry& GetRegistry() { return Registry; }
		void DestroyEntity(const LEntity Entity);
		bool HasEntity(const LEntity Entity) const;
		bool IsEntityInRegistry(const LEntity Entity) const;

		LEntity CreateEntity(const std::string& InName = "");
		LEntity CreateEntityWithID(const LUUID UUID, const std::string& InName = "");
		LEntity CreateChildEntity(LEntity Parent, const std::string& InName = "");
		LEntity TryGetEntityWithUUID(const LUUID ID);

		void ParentEntity(LEntity Entity, LEntity Parent);
		void UnparentEntity(LEntity Entity, bool bConvertToWorldSpace = true);

		glm::mat4 GetWorldSpaceTransform(LEntity Entity);

		std::string GetName() const { return Name; }
		void SetName(const std::string& InName) { Name = InName; }

		void SetActive(const bool Active);
		FORCEINLINE bool IsActive() const { return (this == ActiveScene.Get()); }

		void Clear();
		void Pause(const bool IsPaused);

		void CopyTo(TObjectPtr<LScene>& TargetScene);

		template<typename T>
		void OnComponentAdded(LEntity Entity, T& TComponent);

		template<typename T>
		static void CopyComponent(entt::registry& DestinationRegistry, 
			                      entt::registry& SourceRegistry, 
			                      const std::unordered_map<LUUID, entt::entity>& enttMap)
		{
			auto SourceEntities = SourceRegistry.view<T>();
			for (auto SourceEntity : SourceEntities)
			{
				entt::entity DestEntity = enttMap.at(SourceRegistry.get<LIDComponent>(SourceEntity).ID);
				auto& SourceComp = SourceRegistry.get<T>(SourceEntity);
				auto& DestComp = DestinationRegistry.emplace_or_replace<T>(DestEntity, SourceComp);
			}
		}

		template<typename TComponent>
		void CopyComponentIfExists(entt::entity Destination, entt::registry& DestinationRegistry, entt::entity Source)
		{
			if (Registry.all_of<TComponent>(Source))
			{
				auto& SourceComp = Registry.get<TComponent>(Source);
				DestinationRegistry.emplace_or_replace<TComponent>(Destination, SourceComp);
			}
		}

	/** @fixme: Problems on Linux */
	#if defined(LK_ENGINE_MSVC)
		template<typename TComponent>
		static void CopyComponentFromScene(LEntity Destination, 
										   TObjectPtr<LScene> DestinationScene, 
										   LEntity Source, 
										   TObjectPtr<LScene> SourceScene)
		{
			SourceScene->CopyComponentIfExists<TComponent>((entt::entity)Destination, DestinationScene->Registry, (entt::entity)Source);
		}
	#endif

		template<typename ...Components>
		auto GetAllEntitiesWith()
		{
			return Registry.view<Components...>();
		}

		std::unordered_set<LUUID> GetAssetList();

		static TObjectPtr<LScene> GetActiveScene() { return ActiveScene; }
		static uint8_t GetSceneCount() { return SceneCounter; }

	public:
		static constexpr const char* FILE_EXTENSION = "lkscene";
	private:
		inline static TObjectPtr<LScene> ActiveScene = nullptr; /* TO REMOVE */
		inline static uint8_t SceneCounter = 0;
	private:
		LUUID SceneID = 0;
		std::string Name = "";
		entt::entity SceneEntity;

		bool bPaused = false;
		bool bEditorScene = false;

		entt::registry Registry{};
		//std::unordered_map<LUUID, LEntity> EntityMap{};
		std::unordered_map<LUUID, entt::entity> EntityMap{};

		uint16_t ViewportWidth = 0;
		uint16_t ViewportHeight = 0;
		int Frames = 0;

		TObjectPtr<LSceneRenderer> Renderer;

		friend class LEntity;
		friend class LEditorLayer;
		friend class LSceneSerializer;
		friend class LSceneManagerPanel;

		LCLASS(LScene);
	};

	namespace Enum 
	{
		static constexpr const char* ToString(const ESceneState SceneState)
		{
			switch (SceneState)
			{
				case ESceneState::Edit:     return "Edit";
				case ESceneState::Play:     return "Play";
				case ESceneState::Pause:    return "Pause";
				case ESceneState::Simulate: return "Simulate";
			}

			LK_CORE_ASSERT(false, "Enum::ToString(ESceneState) failed for value: {}", static_cast<int>(SceneState));
			return nullptr;
		}
	}

}
