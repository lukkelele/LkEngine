#pragma once

#include <unordered_set>

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Timer.h"

#include "LkEngine/Asset/Asset.h"

#include "Components.h"

#include "LkEngine/Editor/EditorCamera.h"

#include <entt/entt.hpp>


namespace LkEngine {

	class Entity;
	class World;
	class Renderer;
	class SceneRenderer;
	struct Physics2DSpecification;

	using EntityMap = std::unordered_map<UUID, Entity>;

	class Scene : public Asset
	{
	public:
		Scene(bool editorScene = true);
		Scene(const std::string& name, bool activeScene = true, bool editorScene = false);
		~Scene() = default;

		void OnRender(Ref<SceneRenderer> renderer, Timestep ts);
		void OnRenderEditor(EditorCamera& editorCamera, Timestep ts);
		void EndScene();

		static Ref<Scene> CreateEmpty();

		Entity GetMainCameraEntity();
		std::vector<Entity> GetEntities();
		uint64_t GetEntityCount() const { return m_EntityIDMap.size(); }
		void SortEntities();
		Entity FindEntity(std::string_view name);
		Entity GetEntityWithUUID(UUID uuid);
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(Entity entity);
		bool HasEntity(Entity entity) const;
		bool IsEntityInRegistry(Entity entity) const;

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "");
		Entity CreateChildEntity(Entity parent, const std::string& name = "");
		Entity TryGetEntityWithUUID(UUID id) const;

		void ParentEntity(Entity entity, Entity parent);
		void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		void SetAsActive(bool active) { m_IsActiveScene = active; }
		void SetAsEditorScene(bool editorScene) { m_EditorScene = editorScene; }
		void Clear();

		Ref<EditorCamera> GetEditorCamera() { return m_EditorCamera; }

		void Pause(bool paused);
		void SwitchCamera();
		void SetCamera(Ref<SceneCamera> cam);
		void SetCamera(SceneCamera* cam);
		void SetEditorCamera(const Ref<EditorCamera> editorCamera);
		Ref<SceneCamera> GetMainCamera() { return m_Camera; }
		UUID GetUUID() const { return m_SceneID; }

		Box2DWorldComponent& GetBox2DWorld();
		void Initiate2DPhysics(const Physics2DSpecification& specification);

		void CopyTo(Ref<Scene>& target);

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		template<typename T>
		static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			auto srcEntities = srcRegistry.view<T>();
			for (auto srcEntity : srcEntities)
			{
				entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);
				auto& srcComponent = srcRegistry.get<T>(srcEntity);
				auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
			}
		}

		template<typename TComponent>
		void CopyComponentIfExists(entt::entity dst, entt::registry& dstRegistry, entt::entity src)
		{
			if (m_Registry.has<TComponent>(src))
			{
				auto& srcComponent = m_Registry.get<TComponent>(src);
				dstRegistry.emplace_or_replace<TComponent>(dst, srcComponent);
			}
		}

		template<typename TComponent>
		static void CopyComponentFromScene(Entity dst, Ref<Scene> dstScene, Entity src, Ref<Scene> srcScene)
		{
			srcScene->CopyComponentIfExists<TComponent>((entt::entity)dst, dstScene->m_Registry, (entt::entity)src);
		}

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		static void SetActiveScene(Ref<Scene>& scene);
		static std::string GetActiveSceneName() { if (m_ActiveScene) { return m_ActiveScene->GetName(); } return ""; }
		static uint8_t GetSceneCount() { return m_SceneCounter; }
		static Ref<Scene> GetActiveScene() { return m_ActiveScene; }

		std::unordered_set<AssetHandle> GetAssetList();

	private:
		inline static Scene* m_ActiveScene = nullptr;
		inline static uint8_t m_SceneCounter = 0;
	private:
		std::string m_Name = "";
		entt::entity m_SceneEntity;
		UUID m_SceneID = 0;
		bool m_Paused = false;
		int m_Frames = 0;

		EntityMap m_EntityIDMap;
		entt::registry m_Registry; 

		Timer m_Timer;
		bool m_EditorScene = false; // Blank scene
		AssetHandle m_AssetHandle;
		bool m_IsActiveScene = false;

		uint16_t m_ViewportWidth, m_ViewportHeight;

		Ref<SceneCamera> m_Camera = nullptr;
		Ref<SceneCamera> m_Camera2D = nullptr;
		Ref<EditorCamera> m_EditorCamera = nullptr;

		Ref<SceneRenderer> m_Renderer = nullptr;

		friend class Entity;
		friend class EditorLayer;
		friend class SceneSerializer;
		friend class SceneManagerPanel;
	};

}