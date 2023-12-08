#pragma once
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Renderer/OrthographicCamera.h"
#include "LkEngine/Scene/Entity.h"

#include <entt/entt.hpp>

namespace LkEngine {

	class World;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		static s_ptr<Scene> Create() { return std::make_shared<Scene>(); }

		void BeginScene(Camera& cam, float ts = 1.0f);
		void BeginScene(float ts = 1.0f);
		void EndScene();
		void OnImGuiRender();
		bool IsRunning() const { return m_IsRunning; }
		void Pause(bool paused);

		std::vector<Entity> GetEntities();
		Entity FindEntity(std::string_view name);
		Entity GetEntityWithUUID(UUID uuid);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(Entity entity);

		s_ptr<Camera> GetActiveCamera() const { return m_ActiveCamera; }
		void SetActiveCamera(Camera& cam) { m_ActiveCamera = std::shared_ptr<Camera>(&cam); }
		s_ptr<Camera> GetEditorCamera() const { return m_EditorCamera; }
		s_ptr<World> GetWorld() { return m_World; }
		void SwitchCamera();
		uint64_t GetEntityCount() const { return m_EntityMap.size(); }

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		static Scene* GetActiveScene() { return s_ActiveScene; }

	private:
		template<typename T>
		static void HandleComponent(Entity& entity);

	private:
		inline static Scene* s_ActiveScene = nullptr;
	private:
		bool m_IsRunning = false;
		bool m_Paused = false;
		int m_Frames = 0;
		Timer m_Timer;
		entt::registry m_Registry; 

		std::unordered_map<UUID, entt::entity> m_EntityMap;
		s_ptr<Renderer> m_Renderer;
		s_ptr<World> m_World;

		//s_ptr<SceneCamera> m_SceneCamera; 
		s_ptr<Camera> m_ActiveCamera; 
		s_ptr<OrthographicCamera> m_Camera2D = nullptr;
		s_ptr<EditorCamera> m_EditorCamera;

		friend class Entity;
	};

}