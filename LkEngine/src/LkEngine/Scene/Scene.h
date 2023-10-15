#pragma once
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Renderer/Renderer.h"
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
		const s_ptr<Camera> getCamera() const { return m_Camera; }

		Entity GetEntityWithUUID(UUID uuid);
		Entity FindEntity(std::string_view name);
		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
		void DestroyEntity(Entity entity);

		s_ptr<World> GetWorld() { return m_World; }
		bool IsRunning() const { return m_IsRunning; }
		void Pause(bool paused);

		void SwitchCamera();

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		static Scene* GetActiveScene() { return ActiveScene; }

	public:
		static Scene* ActiveScene;
		entt::registry Registry;
	private:
		bool m_IsRunning = false;
		bool m_Paused = false;
		int m_Frames = 0;
		Timer m_Timer;

		std::unordered_map<UUID, entt::entity> m_EntityMap;
		s_ptr<Renderer> m_Renderer;
		s_ptr<World> m_World;
		//s_ptr<SceneCamera> m_Camera; // Rework this
		s_ptr<Camera> m_Camera; // Rework this
		s_ptr<EditorCamera> m_EditorCamera;
	};

}