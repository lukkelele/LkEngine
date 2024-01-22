#pragma once

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

	using EntityMap = std::unordered_map<UUID, Entity>;

	struct SceneInfo
	{
		std::string Name = "";
		bool Paused = false;
		int Frames = 0;
	};

	class Scene : public Asset
	{
	public:
		Scene(bool editorScene = true);
		Scene(const std::string& name, bool activeScene = true, bool editorScene = false);
		~Scene() = default;

		static Scene* GetActiveScene() { return m_ActiveScene; }
		static void SetActiveScene(Ref<Scene>& scene);
		static std::string GetActiveSceneName() { if (m_ActiveScene) { return m_ActiveScene->GetName(); } return ""; }
		static uint8_t GetSceneCount() { return s_SceneCounter; }

		void OnRender(Ref<SceneRenderer> renderer, Timestep ts);
		void OnRenderEditor(EditorCamera& editorCamera, Timestep ts);
		void EndScene();

		std::string GetName() const { return m_SceneInfo.Name; }
		void SetName(const std::string& name) { m_SceneInfo.Name = name; }
		SceneInfo GetSceneInfo() const { return m_SceneInfo; }
		SceneInfo& GetSceneInfo() { return m_SceneInfo; }
		void SetAsActive(bool active) { m_IsActiveScene = active; }

		Entity GetMainCameraEntity();
		std::vector<Entity> GetEntities();
		uint64_t GetEntityCount() const { return m_EntityMap.size(); }
		void SortEntities();
		Entity FindEntity(std::string_view name);
		Entity GetEntityWithUUID(UUID uuid);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "");
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(Entity entity);
		bool IsEntityInRegistry(Entity entity) const;

		void Pause(bool paused);
		void SwitchCamera();
		void SetCamera(Ref<SceneCamera> cam);
		void SetCamera(SceneCamera* cam);
		void SetEditorCamera(const Ref<EditorCamera> editorCamera);
		Ref<SceneCamera> GetMainCamera() { return m_Camera; }

		// TODO: Generalized and abstract 2d physics impl.
		Box2DWorldComponent& GetBox2DWorld();

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		inline static Scene* m_ActiveScene = nullptr;
		inline static uint8_t s_SceneCounter = 0;
	private:
		entt::entity m_SceneEntity;
		SceneInfo m_SceneInfo;
		EntityMap m_EntityMap;
		entt::registry m_Registry; 
		Timer m_Timer;
		bool m_EditorScene = false; // Blank scene
		AssetHandle m_AssetHandle;
		bool m_IsActiveScene = false;

		uint16_t m_ViewportWidth, m_ViewportHeight;

		Ref<EditorCamera> m_EditorCamera = nullptr;

		Ref<SceneCamera> m_Camera = nullptr;
		Ref<SceneCamera> m_Camera2D = nullptr;
		Ref<SceneRenderer> m_Renderer = nullptr;

		friend class Entity;
		friend class SceneLoader;
		friend class SceneSerializer;
	};

}