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
		m_Camera = create_s_ptr<SceneCamera>(45.0f, 0.010f, 1000.0f);
		m_Camera->SetPosition(glm::vec3(0.0f, 15.0f, -46.0f));
		//m_Camera->SetScene(this);
		//m_World = create_s_ptr<World>();
		//m_World->InitPhysics(this);

	}
	
	Scene::~Scene()
	{
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
		tag.tag = name.empty() ? "Entity" : name;
		m_EntityMap[uuid] = entity;
		return entity;
	}

	Entity Scene::getEntityWithUUID(UUID uuid)
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
			if (tc.tag == name)
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
		m_Camera->OnImGuiRender();
	}

	void Scene::SwitchCamera()
	{
		Camera* currentCamera = m_Camera.get();
		Camera* editorCamera = dynamic_cast<Camera*>(m_EditorCamera.get());

		if (currentCamera == editorCamera)
		{
	
		}
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		// static assert	
	}
		template<>
		void Scene::OnComponentAdded<Mesh>(Entity entity, Mesh& mesh)
		{
			LOG_INFO("{0} : MeshComponent added!", entity.GetName());
		}

		template<>
		void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& rigidbody)
		{
			LOG_INFO("{0} : TransformComponent added!", entity.GetName());
		}

		template<>
		void Scene::OnComponentAdded<Material>(Entity entity, Material& rigidbody)
		{
			LOG_INFO("{0} : MaterialComponent added!", entity.GetName());
		}


#ifdef LK_ENGINE_OLD_IMPL
		template<>
		void Scene::OnComponentAdded<BoxColliderShape>(Entity entity, BoxColliderShape& boxColliderShape)
		{
			// TODO: OnComponentRemoval -> remove rigidbody
			entity.AddComponent<Rigidbody>(boxColliderShape.GetRigidbody());
			LOG_INFO("{0} : BoxColliderShape added!", entity.GetName());
		}

		template<>
		void Scene::OnComponentAdded<Rigidbody>(Entity entity, Rigidbody& rigidbody)
		{
			m_World->AddRigidbodyToWorld(rigidbody);
			LOG_INFO("{0} : RigidbodyComponent added!", entity.GetName());
		}
#endif

}