#include "LKpch.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/UI/UILayer.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/UI/Color.h"


namespace LkEngine {

	Scene* Scene::ActiveScene;

	Scene::Scene()
	{
		ActiveScene = this;
		m_Camera = create_s_ptr<SceneCamera>();
		m_EditorCamera = create_s_ptr<EditorCamera>();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name);
		return entity;
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		TagComponent& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		LOG_DEBUG("Created new entity: {0} (UUID: {1}, tag: {2})", name, uuid, tag.Tag);
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
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
			{
				return Entity{ entity , this };
			}
		}
		return { };
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
		LOG_DEBUG("Entity successfully deleted");
	}

	void Scene::Pause(bool paused)
	{
		m_Paused = paused;
		//m_World->Pause(paused);
	}

	void Scene::SwitchCamera()
	{
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		LK_UNUSED(entity); 
		LK_UNUSED(component);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& rigidbody)
	{
		//LOG_DEBUG("{0} - TransformComponent added", entity.GetName());
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& mesh)
	{
		//LOG_DEBUG("{0} - MeshComponent added", entity.GetName());
	}

	template<typename T>
	void Scene::HandleComponent(Entity& entity)
	{
		if (!entity.HasComponent<MeshComponent>())
			return;

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& mesh = entity.GetComponent<MeshComponent>();

		mesh.BaseShader->Bind();
		mesh.BaseShader->SetUniformMat4f("u_TransformMatrix", transform.GetTransform());
		mesh.BaseShader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
	}

	void Scene::OnUpdate(float ts)
	{
		m_Camera->OnUpdate(ts);
		m_EditorCamera->OnUpdate(ts);

		auto entities = m_Registry.view<TransformComponent>();
		for (auto& ent : entities)
		{	
			Entity entity = { ent, this };

			auto& transform = entity.GetComponent<TransformComponent>();
			if (entity.HasComponent<MeshComponent>())
			{
				entity.OnUpdate(ts);
				auto& mesh = entity.GetComponent<MeshComponent>();
				Renderer::Draw(entity);
			}
		}
	}

	void Scene::OnImGuiRender()
	{
		auto entities = m_Registry.view<TransformComponent>();

		UILayer::SceneEntities(); // Left Sidebar
		UILayer::SelectedEntityMenu(); // Right sidebar
	}

}