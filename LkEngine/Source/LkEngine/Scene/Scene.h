#pragma once

#include <unordered_set>

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/String.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Asset/Asset.h"

#include "Components.h"

#include "LkEngine/Editor/EditorCamera.h"

#include <entt/entt.hpp>


namespace LkEngine {

	class LEntity;
	class World;
	class Renderer;
	class SceneRenderer;
	struct Physics2DSpecification;

	using EntityMap = std::unordered_map<UUID, LEntity>;

	class Scene : public LObject, public Asset
	{
	public:
		Scene(const bool bIsEditorScene = true);
		Scene(const LString& SceneName, bool bIsActiveScene = true, bool bIsEditorScene = false);
		~Scene();

		void OnRender(Ref<SceneRenderer> renderer, FTimestep Timestep);
		void OnRenderEditor(LEditorCamera& editorCamera, FTimestep Timestep);
		void EndScene();

		static Ref<Scene> CreateEmpty();

		LEntity GetMainCameraEntity();
		std::vector<LEntity> GetEntities();
		FORCEINLINE uint64_t GetEntityCount() const { return m_EntityIDMap.size(); }
		void SortEntities();
		LEntity FindEntity(std::string_view name);
		LEntity GetEntityWithUUID(UUID uuid);
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(LEntity Entity);
		bool HasEntity(LEntity Entity) const;
		bool IsEntityInRegistry(LEntity Entity) const;

		LEntity CreateEntity(const std::string& InName = "");
		LEntity CreateEntityWithID(UUID uuid, const std::string& InName = "");
		LEntity CreateChildEntity(LEntity Parent, const std::string& InName = "");
		LEntity TryGetEntityWithUUID(UUID id) const;

		void ParentEntity(LEntity Entity, LEntity Parent);
		void UnparentEntity(LEntity Entity, bool bConvertToWorldSpace = true);

		FORCEINLINE std::string GetName() const { return Name; }
		void SetName(const std::string& InName) { Name = InName; }
		void SetAsActive(bool active) { m_IsActiveScene = active; }
		void SetAsEditorScene(bool bIsEditorScene) { m_EditorScene = bIsEditorScene; }
		void Clear();

		Ref<LEditorCamera> GetEditorCamera() { return EditorCamera; }

		void Pause(bool paused);
		void SwitchCamera();
		void SetCamera(Ref<SceneCamera> cam);
		void SetCamera(SceneCamera* cam);
		void SetEditorCamera(const Ref<LEditorCamera> EditorCamera);
		Ref<SceneCamera> GetMainCamera() { return m_Camera; }
		UUID GetUUID() const { return m_SceneID; }

		Box2DWorldComponent& GetBox2DWorld();
		void Initiate2DPhysics(const Physics2DSpecification& PhysicsSpecification);

		void CopyTo(Ref<Scene>& target);

		template<typename T>
		void OnComponentAdded(LEntity Entity, T& TComponent);

		template<typename T>
		static void CopyComponent(entt::registry& DestinationRegistry, 
			                      entt::registry& SourceRegistry, 
			                      const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			auto SourceEntities = SourceRegistry.view<T>();
			for (auto SourceEntity : SourceEntities)
			{
				entt::entity destEntity = enttMap.at(SourceRegistry.get<IDComponent>(SourceEntity).ID);
				auto& SourceComponent = SourceRegistry.get<T>(SourceEntity);
				auto& destComponent = DestinationRegistry.emplace_or_replace<T>(destEntity, SourceComponent);
			}
		}

		template<typename TComponent>
		void CopyComponentIfExists(entt::entity Destination, entt::registry& DestinationRegistry, entt::entity Source)
		{
			if (m_Registry.has<TComponent>(Source))
			{
				auto& SourceComponent = m_Registry.get<TComponent>(Source);
				DestinationRegistry.emplace_or_replace<TComponent>(Destination, SourceComponent);
			}
		}

		template<typename TComponent>
		static void CopyComponentFromScene(LEntity Destination, Ref<Scene> DestinationScene, LEntity Source, Ref<Scene> SourceScene)
		{
			SourceScene->CopyComponentIfExists<TComponent>((entt::entity)Destination, DestinationScene->m_Registry, (entt::entity)Source);
		}

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		static void SetActiveScene(Ref<Scene>& scene);

		FORCEINLINE static std::string GetActiveSceneName()
		{
			if (m_ActiveScene)
			{
				return m_ActiveScene->GetName();
			}

			return "";
		}

		static uint8_t GetSceneCount() { return m_SceneCounter; }
		static Ref<Scene> GetActiveScene() { return m_ActiveScene; }

		std::unordered_set<AssetHandle> GetAssetList();

	private:
		inline static Scene* m_ActiveScene = nullptr;
		inline static uint8_t m_SceneCounter = 0;
	private:
		std::string Name = "";
		entt::entity m_SceneEntity;
		UUID m_SceneID = 0;
		bool m_Paused = false;
		int m_Frames = 0;

		EntityMap m_EntityIDMap;
		entt::registry m_Registry{};

		Timer m_Timer;
		bool m_EditorScene = false; // Blank scene
		AssetHandle m_AssetHandle;
		bool m_IsActiveScene = false;

		uint16_t m_ViewportWidth, m_ViewportHeight;

		Ref<SceneCamera> m_Camera = nullptr;
		Ref<SceneCamera> m_Camera2D = nullptr;
		Ref<LEditorCamera> EditorCamera = nullptr;

		Ref<SceneRenderer> m_Renderer = nullptr;

		friend class LEntity;
		friend class EditorLayer;
		friend class SceneSerializer;
		friend class SceneManagerPanel;
	};

}