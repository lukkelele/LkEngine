#pragma once
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Renderer/OrthographicCamera.h"
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
		s_ptr<Camera> GetActiveCamera() const { return m_ActiveCamera; }
		s_ptr<Camera> GetEditorCamera() const { return m_EditorCamera; }
		s_ptr<World> GetWorld() { return m_World; }
		bool IsRunning() const { return m_IsRunning; }
		void Pause(bool paused);
		void SwitchCamera();
		uint64_t GetEntityCount() const { return m_EntityMap.size(); }

		//COMPONENT_TYPE GetEntities();
		//template<typename COMPONENT_TYPE>
		//entt::basic_view<entt::type_list<entt::constness_as_t<entt::storage_type_t<COMPONENT_TYPE, entt::entity, std::allocator<COMPONENT_TYPE>>, COMPONENT_TYPE>>, entt::type_list<>, void> GetEntities();

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

		//s_ptr<SceneCamera> m_SceneCamera; 
		s_ptr<Camera> m_ActiveCamera; 
		s_ptr<OrthographicCamera> m_Camera2D = nullptr;
		s_ptr<EditorCamera> m_EditorCamera;

		friend class Entity;
	};

}