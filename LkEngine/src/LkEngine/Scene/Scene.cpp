#include "LKpch.h"
#include "Scene.h"
#include "Entity.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Physics2D/Physics2D.h"

#include "LkEngine/Debug/Debugger2D.h"

#include "LkEngine/Editor/Editor.h"


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
			m_EditorCamera = Editor::Get()->GetEditorCamera();
			Editor::Get()->SetScene(*this);
		}

		// TODO: Add SceneManager class for this
		Application::Get()->AddScene(*this);
		s_SceneCounter++;

		// Initiate 2D physics
		Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);

		Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = CreateEntityWithID(UUID(), name);
		return entity;
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		TagComponent& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		LOG_DEBUG("Created new entity: {0} (UUID: {1}, tag: {2})", name, uuid, tag.Tag);
		m_EntityMap[uuid] = entity;
		return entity;
	}

	// TODO: Change name to GetEntityWithID
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
		m_EntityMap.erase(entity.UUID());
		m_Registry.destroy(entity);
		LOG_DEBUG("Entity {} deleted", entity.Name());
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

	void Scene::SortEntities()
	{
		m_Registry.sort<IDComponent>([&](const auto lhs, const auto rhs)
		{
			auto lhsEntity = m_EntityMap.find(lhs.ID);
			auto rhsEntity = m_EntityMap.find(rhs.ID);
			return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
		});
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
		auto sceneView = m_Registry.view<Box2DWorldComponent>();
		auto& world = m_Registry.get<Box2DWorldComponent>(sceneView.front()).World;

		Entity e = { entity, this };
		UUID entityID = e.UUID();
		TransformComponent& transform = e.Transform();
		auto& rigidBody2D = m_Registry.get<RigidBody2DComponent>(entity);

		b2BodyDef bodyDef;
		switch (rigidBody2D.BodyType)
		{
			case RigidBody2DComponent::Type::Static: 
				bodyDef.type = b2_staticBody;
				break;
			case RigidBody2DComponent::Type::Dynamic: 
				bodyDef.type = b2_dynamicBody;
				break;
			case RigidBody2DComponent::Type::Kinematic: 
				bodyDef.type = b2_kinematicBody;
				break;
		}

		bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
		bodyDef.angle = transform.GetRotationEuler().z;

		b2Body* body = world->CreateBody(&bodyDef);
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

		LOG_DEBUG("OnComponentAdded<RigidBody2DComponent>  {}", entity.Name());
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& boxColliderComponent)
	{
		LOG_DEBUG("OnComponentAdded<BoxCollider2DComponent>  {}", entity.Name());

		Entity e = { entity, this };
		auto& transform = e.Transform();

		if (e.HasComponent<RigidBody2DComponent>())
		{
			LK_ASSERT(transform.Scale.x * boxColliderComponent.Size.x > 0 && transform.Scale.y * boxColliderComponent.Size.y > 0);

			auto& rigidBody2D = e.RigidBody2D();
			b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);
			LK_ASSERT(body != nullptr);

			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(transform.Scale.x * boxColliderComponent.Size.x, transform.Scale.y * boxColliderComponent.Size.y);
			//LOG_INFO("BoxCollider for {}, size ({}, {})", entity.Name(), transform.Scale.x * boxColliderComponent.Size.x, transform.Scale.y * boxColliderComponent.Size.y);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &polygonShape;
			fixtureDef.density = boxColliderComponent.Density;
			fixtureDef.friction = boxColliderComponent.Friction;
			fixtureDef.isSensor = boxColliderComponent.IsSensor;
			fixtureDef.restitution = 0.30f;
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

	#ifdef LK_DEBUG
		auto& world = GetBox2DWorld();
		world.World->DebugDraw();
	#endif
	}

#if 0

	void Scene::BeginScene(Timestep ts)
	{
		if (m_Camera == nullptr)
			return;
		BeginScene(*m_Camera, ts);
	}

	void Scene::BeginScene(SceneCamera& cam, Timestep ts)
	{
		cam.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		cam.Update(ts);
	}
#endif


	// TODO: Update this so no RenderCommand calls are needed and its not exclusive towards sprite draw calls
	void Scene::EndScene()
	{
		auto& world = GetBox2DWorld();
		world.World->DebugDraw();

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
						entity.Transform(),
						entity.Sprite(),
						entity.Material(),
						entity.UUID()
					);
				}
				else // Draw color only, no texture
				{
					Renderer::SubmitSprite(
						entity.GetComponent<TransformComponent>(),
						entity.Sprite().Size,
						entity.Sprite().Color,
						entity.UUID()
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