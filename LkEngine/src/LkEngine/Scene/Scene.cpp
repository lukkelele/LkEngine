#include "LKpch.h"
#include "Scene.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Physics2D/Physics2D.h"

#include "LkEngine/Debug/Debugger2D.h"

#include "LkEngine/Debug/Box2DDebugger.h"

#include "LkEngine/Editor/EditorCamera.h"


namespace LkEngine {

	Scene::Scene(const std::string& name, bool activeScene, bool editorScene)
		: m_EditorScene(editorScene)
	{
		m_SceneEntity = m_Registry.create();
		m_SceneInfo.Name = name;

		m_ViewportWidth = Window::Get()->GetViewportWidth();
		m_ViewportHeight = Window::Get()->GetViewportHeight();

		if (activeScene)	
			s_ActiveScene = this;
		

		if (editorScene)
		{
			m_EditorCamera = EditorLayer::Get()->GetEditorCamera();
			EditorLayer::Get()->SetScene(*this);
		}

		Application::Get()->AddScene(*this);
		s_SceneCounter++;

		// Initiate 2D physics
		Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);

		Debugger::AttachDebugDrawer2D(&box2dWorld);
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
				return Entity{ entity , this };
		}
		return { };
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
		LOG_DEBUG("Entity {} deleted", entity.GetName());
	}

	bool Scene::IsEntityInRegistry(Entity entity) const
	{
		return m_Registry.any_of<IDComponent>(entity);
	}

	std::vector<Entity> Scene::GetEntities()
	{
		std::vector<Entity> entities;
		auto view = m_Registry.view<IDComponent>();
		for (const auto& entity : view)
			entities.push_back({ entity, this });
		return entities;
	}

	void Scene::Pause(bool paused)
	{
		m_SceneInfo.Paused = paused;
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
	}

	template<>
	void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& rigidBody2DComponent)
	{
		//auto sceneView = m_Registry.view<Box2DWorldComponent>();
		//auto& world = m_Registry.get<Box2DWorldComponent>(sceneView.front()).World;
		auto& world = GetBox2DWorld();

		Entity e = { entity, this };
		UUID entityID = e.GetComponent<IDComponent>().ID;
		TransformComponent& transform = e.GetComponent<TransformComponent>();
		auto& rigidBody2D = m_Registry.get<RigidBody2DComponent>(entity);

		b2BodyDef bodyDef;
		if (rigidBody2D.BodyType == RigidBody2DComponent::Type::Static)
			bodyDef.type = b2_staticBody;
		else if (rigidBody2D.BodyType == RigidBody2DComponent::Type::Dynamic)
			bodyDef.type = b2_dynamicBody;
		else if (rigidBody2D.BodyType == RigidBody2DComponent::Type::Kinematic)
			bodyDef.type = b2_kinematicBody;

		bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
		bodyDef.angle = transform.GetRotationEuler().z;

		b2Body* body = world.World->CreateBody(&bodyDef);
		body->SetFixedRotation(rigidBody2D.FixedRotation);

		b2MassData massData = body->GetMassData();
		massData.mass = rigidBody2D.Mass;
		body->SetMassData(&massData);
		body->SetGravityScale(rigidBody2D.GravityScale);
		body->SetLinearDamping(rigidBody2D.LinearDrag);
		body->SetAngularDamping(rigidBody2D.AngularDrag);
		body->SetBullet(rigidBody2D.IsBullet);
		body->GetUserData().pointer = (uintptr_t)entityID;
		rigidBody2D.RuntimeBody = body;

		LOG_DEBUG("OnComponentAdded<RigidBody2DComponent>  {}", entity.GetName());
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& boxColliderComponent)
	{
		LOG_DEBUG("OnComponentAdded<BoxCollider2DComponent>  {}", entity.GetName());

		Entity e = { entity, this };
		auto& transform = e.Transform();
		//auto& boxCollider2D = m_Registry.get<BoxCollider2DComponent>(entity);

		if (e.HasComponent<RigidBody2DComponent>())
		{
			LK_ASSERT(transform.Scale.x * boxColliderComponent.Size.x > 0 && transform.Scale.y * boxColliderComponent.Size.y > 0);

			auto& rigidBody2D = e.GetComponent<RigidBody2DComponent>();
			b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);
			LK_ASSERT(body != nullptr);

			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(transform.Scale.x * boxColliderComponent.Size.x, transform.Scale.y * boxColliderComponent.Size.y);
			LOG_INFO("BoxCollider for {}, size ({}, {})", entity.GetName(), transform.Scale.x * boxColliderComponent.Size.x, transform.Scale.y * boxColliderComponent.Size.y);
			//polygonShape.SetAsBox(1000, 1000);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &polygonShape;
			fixtureDef.density = boxColliderComponent.Density;
			fixtureDef.friction = boxColliderComponent.Friction;
			body->CreateFixture(&fixtureDef);
			LOG_DEBUG("Added b2FixtureDef to body");
		}
	}

	void Scene::OnRender(s_ptr<SceneCamera> sceneCamera, Timestep ts)
	{
	}

	void Scene::OnRenderEditor(EditorCamera& editorCamera, Timestep ts)
	{
		editorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		editorCamera.Update(ts);
	}

	void Scene::BeginScene(Timestep ts)
	{
		if (m_Camera == nullptr)
			return;
		BeginScene(*m_Camera, ts);
	}

	void Scene::BeginScene(const glm::mat4& viewProjection, Timestep ts)
	{
		signed int velocityIterations = 6;
		signed int positionIterations = 2;

		if (m_SceneInfo.Paused == false)
		{
			// Update the world
			auto& world = GetBox2DWorld();
			world.World->Step((1.0f / 60.0f), 12, 4);
			world.World->DebugDraw();
		}
	}

	void Scene::BeginScene(SceneCamera& cam, Timestep ts)
	{
		cam.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		cam.Update(ts);

		signed int velocityIterations = 6;
		signed int positionIterations = 2;

		if (m_SceneInfo.Paused == false)
		{
			// Update the world
			auto& world = GetBox2DWorld();
			world.World->Step((1.0f / 60.0f), 12, 4);
			world.World->DebugDraw();
		}
	}


	// TODO: Update this so no RenderCommand calls are needed and its not exclusive towards sprite draw calls
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

	void Scene::SetCamera(s_ptr<SceneCamera> cam)
	{ 
		m_Camera = cam;
	}

	void Scene::SetCamera(SceneCamera* cam)
	{ 
		m_Camera = std::shared_ptr<SceneCamera>(cam);
	}

	void Scene::SetEditorCamera(EditorCamera* cam)
	{ 
		m_EditorCamera = cam;
	}

	Box2DWorldComponent& Scene::GetBox2DWorld()
	{
		return m_Registry.get<Box2DWorldComponent>(m_SceneEntity);
	}

}