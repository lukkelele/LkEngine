#pragma once
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/SceneCamera.h"
#include <entt/entt.hpp>



namespace LkEngine {

	class Entity;
	class World;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		void OnUpdate(float ts);
		void OnImGuiRender();
		Entity GetEntityWithUUID(UUID uuid);
		Entity FindEntity(std::string_view name);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
		entt::registry& GetRegistry() { return m_Registry; }
		void DestroyEntity(Entity entity);
		s_ptr<Camera> GetCamera() const { return m_Camera; }
		s_ptr<Camera> GetEditorCamera() const { return m_EditorCamera; }
		s_ptr<World> GetWorld() { return m_World; }
		bool IsRunning() const { return m_IsRunning; }
		void Pause(bool paused);
		void SwitchCamera();
		uint64_t GetEntityCount() const { return m_EntityMap.size(); }

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		static Scene* GetActiveScene() { return ActiveScene; }

	private:
		template<typename T>
		static void HandleComponent(Entity& entity);

	public:
		static Scene* ActiveScene;
	private:
		bool m_IsRunning = false;
		bool m_Paused = false;
		int m_Frames = 0;
		Timer m_Timer;
		entt::registry m_Registry; 

		std::unordered_map<UUID, entt::entity> m_EntityMap;
		s_ptr<Renderer> m_Renderer;
		s_ptr<World> m_World;
		s_ptr<SceneCamera> m_Camera; 
		s_ptr<EditorCamera> m_EditorCamera;

		friend class Entity;
	};

}