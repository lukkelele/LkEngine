#pragma once

#include "LkEngine/Core/UUID.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Renderer/OrthographicCamera.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Editor/EditorCamera.h"

#include <entt/entt.hpp>


namespace LkEngine {

	// Forward declarations
	class Entity;
	class World;
	class Renderer;

	typedef std::unordered_map<UUID, Entity> EntityMap;

	class Scene
	{
	public:
		Scene(const std::string& name, bool activeScene = true);
		~Scene() = default;

		static s_ptr<Scene> Create(const std::string& name, bool activeScene = true); 
		static Scene* GetActiveScene() { return s_ActiveScene; }
		static void SetActiveScene(Scene& scene) { s_ActiveScene = &scene; }
		static std::string GetActiveSceneName() { if (s_ActiveScene) { return s_ActiveScene->GetName(); } return ""; }
		static uint8_t GetSceneCount() { return s_SceneCounter; }

		void BeginScene(SceneCamera& cam, float ts = 1.0f);
		void BeginScene(float ts = 1.0f);
		void EndScene();
		bool IsRunning() const { return m_IsRunning; }
		void Pause(bool paused);

		std::vector<Entity> GetEntities();
		Entity FindEntity(std::string_view name);
		Entity GetEntityWithUUID(UUID uuid);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(Entity entity);
		bool IsEntityInRegistry(Entity entity) const;
		SceneCamera* GetActiveCamera() { return m_SceneCamera; }
		void SetActiveCamera(SceneCamera* cam);
		s_ptr<EditorCamera> GetEditorCamera() const { return m_EditorCamera; }
		s_ptr<World> GetWorld() { return m_World; }
		void SwitchCamera();
		uint64_t GetEntityCount() const { return m_EntityMap.size(); }
		std::string GetName() const { return m_Name; }
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		inline static Scene* s_ActiveScene = nullptr;
		inline static uint8_t s_SceneCounter = 0;
	private:
		bool m_IsRunning = false;
		bool m_Paused = false;
		int m_Frames = 0;
		Timer m_Timer;
		std::string m_Name;
		entt::registry m_Registry; 
		uint16_t m_ViewportWidth, m_ViewportHeight;
		EntityMap m_EntityMap;
		SceneCamera* m_SceneCamera = nullptr;
		s_ptr<SceneCamera> m_Camera2D = nullptr;
		s_ptr<Renderer> m_Renderer;
		s_ptr<World> m_World;
		s_ptr<EditorCamera> m_EditorCamera;

		friend class Entity;
	};

}