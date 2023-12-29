#include "LKpch.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Color.h"
#include "LkEngine/Physics2D/Physics2D.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/UI/Property.h"


namespace LkEngine {

	Scene::Scene(const std::string& name, bool activeScene)
		: m_Name(name)
	{
		auto window = Window::Get();
		float width = window->GetWidth();
		float height = window->GetHeight();

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_Camera2D = std::make_shared<SceneCamera>();
		m_Camera2D->SetOrthographic(width, height, -1.0f, 1.0f);
		m_SceneCamera = m_Camera2D;

		if (activeScene)
		{
			s_ActiveScene = this;
		}
	}

	s_ptr<Scene> Scene::Create(const std::string& name, bool activeScene)
	{
		s_ptr<Scene> scene = new_s_ptr<Scene>(name, activeScene);
		auto* editorLayer = EditorLayer::Get();
		if (editorLayer != nullptr && editorLayer->IsEnabled())
		{
			editorLayer->SetScene(*scene);
		}
		Application::Get()->AddScene(*scene);
		s_SceneCounter++;

		return scene;
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
		//return { (entt::entity)-1, NULL };
		//return { (entt::entity)-1, nullptr };
		return { };
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
		LOG_DEBUG("Entity successfully deleted");
	}

	bool Scene::IsEntityInRegistry(Entity entity) const
	{
		return m_Registry.any_of<IDComponent>(entity);
	}

	std::vector<Entity> Scene::GetEntities()
	{
		auto view = m_Registry.view<IDComponent>();
		std::vector<Entity> entities;
		for (const auto& entity : view)
		{
			entities.push_back({ entity, this });
		}
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
	void Scene::OnComponentAdded<SpriteComponent>(Entity entity, SpriteComponent& sprite)
	{
		if (entity.HasComponent<MaterialComponent>())
		{
			auto& mc = entity.GetComponent<MaterialComponent>();
			auto& texture = mc.GetTexture();
			if (texture != nullptr)
			{
				//sprite.TextureName = texture->GetName();
			}
		}
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& camera)
	{
	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& mc)
	{
		if (entity.HasComponent<SpriteComponent>() && mc.GetTexture() != nullptr)
		{
			//entity.GetComponent<SpriteComponent>().TextureName = mc.GetTextureName();
			//LOG_TRACE("Detected SpriteComponent when adding MaterialComponent, setting the TextureName to mc.GetTextureName()");
		}
	}

	void Scene::BeginScene(float ts)
	{
		BeginScene(*m_SceneCamera, ts);
	}

	void Scene::BeginScene(SceneCamera& cam, float ts)
	{
		cam.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		cam.Update(ts);
	}

	void Scene::EndScene()
	{
		auto entityTags = m_Registry.view<TagComponent>();

		for (auto& entityTag : entityTags)
		{	
			Entity entity = { entityTag, this };
			if (entity.HasComponent<SpriteComponent>() && entity.HasComponent<TransformComponent>())
			{
				// If MaterialComponent exists, use it to gather pointer to texture
				if (entity.HasComponent<MaterialComponent>())
				{
					RenderCommand::DrawSprite(
						entity.GetComponent<TransformComponent>(), 
						entity.GetComponent<SpriteComponent>(),
						entity.GetComponent<MaterialComponent>(),
						entity.GetUUID()
					);
				}
				else // Draw color only, no texture
				{
					RenderCommand::DrawSprite(
						entity.GetComponent<TransformComponent>(), 
						entity.GetComponent<SpriteComponent>(),
						entity.GetUUID()
					);
				}
			}
		}
	}

}