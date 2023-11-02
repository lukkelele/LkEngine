#include "LKpch.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/UI/UILayer.h"
#include "LkEngine/UI/Property.h"
#include "LkEngine/Renderer/Color.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Physics/2D/Physics2D.h"
#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

	Scene* Scene::ActiveScene;

	Scene::Scene()
	{
		ActiveScene = this;
		auto window = Window::Get();
		float width = window->GetWidth();
		float height = window->GetHeight();
		//m_ActiveCamera = create_s_ptr<OrthographicCamera>(0, width, 0, height);
		//m_Camera2D = create_s_ptr<OrthographicCamera>(0, width, height, 0);
		m_Camera2D = create_s_ptr<OrthographicCamera>(0, width, 0, height);
		m_ActiveCamera = m_Camera2D;
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

	std::vector<Entity> Scene::GetEntities()
	{
		auto view = m_Registry.view<IDComponent>();
		std::vector<Entity> entities;
		for (const auto& entity : view)
			entities.push_back({ entity, this });
		return entities;
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
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& mesh)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteComponent>(Entity entity, SpriteComponent& mesh)
	{
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

	template<typename RaycastTResult>
	void Scene::HandleRaycast(std::vector<RaycastTResult>& raycastResults)
	{
		LK_UNUSED(raycastResults);
	}

	template<>
	void Scene::HandleRaycast(std::vector<Raycast2DResult>& raycastResults)
	{
		int raycastHits = raycastResults.size();

		if (raycastHits == 1)
		{
		    Raycast2DResult raycast = raycastResults.at(0);
		    Entity entity = raycast.HitEntity;
		    auto& mc = entity.GetComponent<MeshComponent>();
		    EditorLayer::SelectedEntityID = raycast.HitEntity.GetUUID();
		}
		else if (raycastHits > 1)
		{
		    for (const auto& raycast : raycastResults)
		    {
		        Entity entity = raycast.HitEntity;
		        
		        uint64_t hitEntityID = entity.GetUUID();
		        if (Mouse::IsButtonPressed(MouseButton::ButtonLeft) && EditorLayer::SelectedEntityID == 0)
		        {
		            Entity currentEntity = EditorLayer::SelectedEntity;
		            EditorLayer::SelectedEntityID = hitEntityID;
		            EditorLayer::SelectedEntity = raycast.HitEntity;
		        }
		    }
		}
	}

	void Scene::BeginScene(float ts)
	{
		auto entities = m_Registry.view<TransformComponent>();
		for (auto& ent : entities)
		{	
			Entity entity = { ent, this };

			auto& transform = entity.GetComponent<TransformComponent>();
			//if (entity.HasComponent<MeshComponent>())
			if (entity.HasComponent<SpriteComponent>())
			{
				RenderCommand::DrawSprite(entity.GetComponent<TransformComponent>(), 
									      entity.GetComponent<SpriteComponent>(), 
						                  entity.GetUUID()
				);
			}
		}
	}

	void Scene::EndScene()
	{

	}

	void Scene::OnImGuiRender()
	{
		//auto entities = m_Registry.view<TransformComponent>();
	}

}