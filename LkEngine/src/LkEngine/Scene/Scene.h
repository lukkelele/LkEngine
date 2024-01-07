#pragma once

#include "LkEngine/Core/UUID.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Editor/EditorCamera.h"

#include "LkEngine/Renderer/OrthographicCamera.h" // remove

#include <entt/entt.hpp>


namespace LkEngine {

	// Forward declarations
	class Entity;
	class World;
	class Renderer;

	using EntityMap = std::unordered_map<UUID, Entity>;

	struct SceneInfo
	{
		std::string Name = "";
		bool Paused = false;
		int Frames = 0;
	};

	class Scene
	{
	public:
		Scene(const std::string& name, bool activeScene = true, bool editorScene = false);
		~Scene() = default;

		static Scene* GetActiveScene() { return s_ActiveScene; }
		static void SetActiveScene(Scene& scene) { s_ActiveScene = &scene; }
		static std::string GetActiveSceneName() { if (s_ActiveScene) { return s_ActiveScene->GetName(); } return ""; }
		static uint8_t GetSceneCount() { return s_SceneCounter; }

		void OnRender(s_ptr<SceneCamera> cam, Timestep ts);
		void OnRenderEditor(EditorCamera& editorCamera, Timestep ts);
		void EndScene();

		std::string GetName() const { return m_SceneInfo.Name; }
		void SetName(const std::string& name) { m_SceneInfo.Name = name; }
		SceneInfo GetSceneInfo() const { return m_SceneInfo; }
		SceneInfo& GetSceneInfo() { return m_SceneInfo; }

		std::vector<Entity> GetEntities();
		uint64_t GetEntityCount() const { return m_EntityMap.size(); }
		void SortEntities();
		Entity FindEntity(std::string_view name);
		Entity GetEntityWithUUID(UUID uuid);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithID(UUID uuid, const std::string& name);
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(Entity entity);
		bool IsEntityInRegistry(Entity entity) const;

		void Pause(bool paused);
		void SwitchCamera();
		void SetCamera(s_ptr<SceneCamera> cam);
		void SetCamera(SceneCamera* cam);
		void SetEditorCamera(EditorCamera* editorCamera);
		s_ptr<SceneCamera> GetCamera() { return m_Camera; }

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
		inline static Scene* s_ActiveScene = nullptr;
		inline static uint8_t s_SceneCounter = 0;
	private:
		entt::entity m_SceneEntity;
		SceneInfo m_SceneInfo;
		EntityMap m_EntityMap;
		entt::registry m_Registry; 
		Timer m_Timer;
		bool m_EditorScene = false; // Blank scene

		uint16_t m_ViewportWidth, m_ViewportHeight;

		s_ptr<SceneCamera> m_Camera = nullptr;
		EditorCamera* m_EditorCamera = nullptr;

		s_ptr<Renderer> m_Renderer;
		s_ptr<SceneCamera> m_Camera2D = nullptr; // remove

		friend class Entity;
		friend class SceneSerializer;
	};

}