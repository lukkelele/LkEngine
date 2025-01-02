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

	using EntityMap = std::unordered_map<LUUID, LEntity>;

	LK_DECLARE_MULTICAST_DELEGATE(FOnSceneSetActive, const TObjectPtr<LScene>&);
	LK_DECLARE_MULTICAST_DELEGATE(FOnSceneCreated,   const TObjectPtr<LScene>&);

	/** OnSceneSetActive. */
	extern FOnSceneSetActive GOnSceneSetActive;
	extern FOnSceneCreated   GOnSceneCreated;

	enum class ESceneType
	{
		Normal = 0,
		Editor,
	};

	class LScene : public LAsset
	{
	public:
		LScene(const std::string& SceneName, const bool IsEditorScene = false);
		LScene() = delete;
		~LScene() = default;

		void OnRender(TObjectPtr<LSceneRenderer> SceneRenderer, const float DeltaTime);
		void OnRenderEditor(TObjectPtr<LSceneRenderer> SceneRenderer, LEditorCamera& EditorCamera, const float DeltaTime);
		void EndScene(); /// FIXME
		
		LEntity GetMainCameraEntity();

		/**
		 * GetEntities
		 * 
		 *  Get registered entities as LEntity or UUID.
		 */
		template<typename T = LEntity>
		std::vector<T> GetEntities();

		FORCEINLINE uint64_t GetEntityCount() const { return m_EntityIDMap.size(); }
		void SortEntities();

		LEntity FindEntity(std::string_view name);
		LEntity GetEntityWithUUID(LUUID uuid) const;

		FORCEINLINE entt::registry& GetRegistry() { return Registry; }
		void DestroyEntity(const LEntity Entity);
		bool HasEntity(const LEntity Entity) const;
		bool IsEntityInRegistry(const LEntity Entity) const;

		LEntity CreateEntity(const std::string& InName = "");
		LEntity CreateEntityWithID(LUUID uuid, const std::string& InName = "");
		LEntity CreateChildEntity(LEntity Parent, const std::string& InName = "");
		LEntity TryGetEntityWithUUID(LUUID ID) const;

		void ParentEntity(LEntity Entity, LEntity Parent);
		void UnparentEntity(LEntity Entity, bool bConvertToWorldSpace = true);

		glm::mat4 GetWorldSpaceTransform(LEntity Entity);

		FORCEINLINE std::string GetName() const { return Name; }
		FORCEINLINE void SetName(const std::string& InName) { Name = InName; }
		FORCEINLINE bool IsActiveScene() const { return this == ActiveScene.Get(); }

		void SetActive(const bool Active);

		void Clear();
		void Pause(const bool IsPaused);

		void SetCamera(TObjectPtr<LSceneCamera> InSceneCamera);
		void SwitchCamera();

		FORCEINLINE TObjectPtr<LSceneCamera> GetMainCamera() { return Camera; } 

		FORCEINLINE LUUID GetUUID() const { return SceneID; }

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

		template<typename TComponent>
		static void CopyComponentFromScene(LEntity Destination, 
										   TObjectPtr<LScene> DestinationScene, 
										   LEntity Source, 
										   TObjectPtr<LScene> SourceScene)
		{
			SourceScene->CopyComponentIfExists<TComponent>((entt::entity)Destination, 
														   DestinationScene->Registry, 
														   (entt::entity)Source);
		}

		template<typename ...Components>
		auto GetAllEntitiesWith()
		{
			return Registry.view<Components...>();
		}

		static void SetActiveScene(TObjectPtr<LScene> InScene);

		FORCEINLINE static std::string GetActiveSceneName()
		{
			if (ActiveScene)
			{
				return ActiveScene->GetName();
			}

			return "";
		}

		/**
		 * @brief Get count of existing scenes.
		 */
		FORCEINLINE static uint8_t GetSceneCount() { return SceneCounter; }

		static TObjectPtr<LScene> GetActiveScene() { return ActiveScene; }

		std::unordered_set<FAssetHandle> GetAssetList();

	public:
		static constexpr const char* FILE_EXTENSION = "lkscene";
	private:
		inline static TObjectPtr<LScene> ActiveScene = nullptr;
		inline static uint8_t SceneCounter = 0;
	private:
		std::string Name = "";

		FAssetHandle SceneID = 0;
		entt::entity SceneEntity;

		bool bPaused = false;
		int m_Frames = 0;

		EntityMap m_EntityIDMap;
		entt::registry Registry{};

		bool bIsEditorScene = false; /// REMOVE

		uint16_t ViewportWidth = 0;
		uint16_t ViewportHeight = 0;

		TObjectPtr<LSceneCamera> Camera = nullptr;
		//TObjectPtr<LEditorCamera> EditorCamera = nullptr;

		TObjectPtr<LSceneRenderer> Renderer = nullptr;

		friend class LEntity;
		friend class LEditorLayer;
		friend class LSceneSerializer;
		friend class LSceneManagerPanel;

		LCLASS(LScene);
	};

}
