#include "LKpch.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Editor/EditorCamera.h"


namespace LkEngine {

	Scene* Scene::ActiveScene;

	Scene::Scene()
	{
		ActiveScene = this;
		//m_Camera = create_s_ptr<SceneCamera>(45.0f, 0.010f, 1000.0f);
		m_Camera = create_s_ptr<SceneCamera>();
		//m_Camera->SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		m_EditorCamera = create_s_ptr<EditorCamera>();
		//m_Camera->SetScene(this);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name);
		return entity;
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { Registry.create(), this };
		LOG_INFO("(UUID) {0} : {1}", name, uuid);
		entity.AddComponent<IDComponent>(uuid);
		TagComponent& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		m_EntityMap[uuid] = entity;
		return entity;
	}

	Entity Scene::GetEntityWithUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };
		return { };
	}

	Entity Scene::FindEntity(std::string_view name)
	{
		auto view = Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity , this };
		}
		return {};
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.getUUID());
		Registry.destroy(entity);
		LOG_CRITICAL("Entity successfully deleted");
	}

	void Scene::OnUpdate(float ts)
	{
		m_Camera->OnUpdate(ts);
		m_EditorCamera->OnUpdate(ts);
#ifdef LK_ENGINE_OLD_IMPL
		m_Camera->OnUpdate(ts);
		m_World->OnUpdate(ts);

		// If mouse has moved -> send World Mouse move event
		if (m_Camera->hasMouseMoved)
			m_World->MouseMoveCallback(Mouse::GetMouseX(), Mouse::GetMouseY());

		if (Mouse::IsButtonPressed(MouseButton::Button0))
			m_World->MouseButtonCallback(MouseButton::Button0, 1, Mouse::GetMouseX(), Mouse::GetMouseY());

		glm::mat4 viewProj = m_Camera->GetViewProjection();
		auto entities = Registry.view<Mesh>();
		for (auto& ent : entities)
		{	
			Entity entity = { ent, this };
			entity.OnUpdate(ts, viewProj);
			Mesh& mesh = entity.GetComponent<Mesh>();

			//m_Renderer->DrawWireframe(entity, Color::Black);
			//m_Renderer->Draw(mesh);
		}
	#endif
	}

	void Scene::Pause(bool paused)
	{
		m_Paused = paused;
		//m_World->Pause(paused);
	}

	void Scene::OnImGuiRender()
	{
	}

	void Scene::SwitchCamera()
	{
		//Camera* editorCamera = dynamic_cast<Camera*>(m_EditorCamera.get());
		//if (m_Camera != m_EditorCamera)
		//{
		//	LOG_DEBUG("Switching cameras");
		//}
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
	}

		template<>
		void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& rigidbody)
		{
			LOG_INFO("{0} : TransformComponent added!", entity.GetName());
		}


}