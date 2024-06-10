#include "LKpch.h"
#include "Scene.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Physics2D/Physics2D.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/SceneRenderer.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Debug/Debugger2D.h"

#include "LkEngine/Core/Application.h"


namespace LkEngine {

	Scene::Scene(bool bIsEditorScene)
		: m_EditorScene(bIsEditorScene)
		, Name("")
	{
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

		m_ViewportWidth = LWindow::Get().GetViewportWidth();
		m_ViewportHeight = LWindow::Get().GetViewportHeight();

		if (bIsEditorScene)
		{
			EditorCamera = EditorLayer::Get()->GetEditorCamera();
			LK_CORE_ASSERT(EditorCamera, "EditorCamera is nullptr");
			EditorLayer::Get()->SetScene(Ref<Scene>(this));
			Input::SetScene(Ref<Scene>(this)); // REMOVE ME
			LApplication::Get()->SetScene(Ref<Scene>(this)); // REMOVE ME
		}

		// Initiate 2D physics
		Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);

		LK_CORE_DEBUG_TAG("Scene", "New scene created {}", Name);
	}

	Scene::Scene(const LString& InName, bool bIsActiveScene, bool bIsEditorScene)
		: m_EditorScene(bIsEditorScene)
		, Name(InName)
	{
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

		m_ViewportWidth = LWindow::Get().GetViewportWidth();
		m_ViewportHeight = LWindow::Get().GetViewportHeight();

		if (bIsActiveScene)
		{
			m_IsActiveScene = true;
			m_ActiveScene = this;
			Input::SetScene(Ref<Scene>(this));
		}
		
		if (bIsEditorScene)
		{
			EditorCamera = EditorLayer::Get()->GetEditorCamera();
			EditorLayer::Get()->SetScene(Ref<Scene>(this));
			LApplication::Get()->SetScene(Ref<Scene>(this));
		}

		/* Initialize 2D physics. */
		Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);

		LK_CORE_DEBUG_TAG("Scene", "New scene created {}", Name);
	}

	Scene::~Scene()
	{
		/* TODO: */
	}

	LEntity Scene::CreateEntity(const std::string& name)
	{
		return CreateChildEntity({}, name);
	}

	LEntity Scene::CreateChildEntity(LEntity Parent, const std::string& name)
	{
		LEntity entity = LEntity{ m_Registry.create(), this };
		IDComponent& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = {};

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
		{
			entity.AddComponent<TagComponent>(name);
		}

		entity.AddComponent<RelationshipComponent>();

		if (Parent)
		{
			entity.SetParent(Parent);
		}

		m_EntityIDMap[idComponent.ID] = entity;

		SortEntities();

		return entity;
	}

	LEntity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		LEntity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		m_EntityIDMap[uuid] = entity;

		return entity;
	}

	// TODO: Change name to GetEntityWithID
	LEntity Scene::GetEntityWithUUID(UUID uuid)
	{
		if (m_EntityIDMap.find(uuid) != m_EntityIDMap.end())
		{
			return { m_EntityIDMap.at(uuid), this };
		}

		return { entt::null, nullptr };
	}

	LEntity Scene::TryGetEntityWithUUID(UUID id) const
	{
		if (const auto Iter = m_EntityIDMap.find(id); Iter != m_EntityIDMap.end())
		{
			return Iter->second;
		}

		return LEntity{};
	}

	LEntity Scene::FindEntity(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
			{
				return LEntity{ entity , this };
			}
		}

		return {};
	}

	void Scene::DestroyEntity(LEntity entity)
	{
		m_EntityIDMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
		LK_CORE_DEBUG("Entity {} deleted", entity.Name());
	}

	bool Scene::IsEntityInRegistry(LEntity entity) const
	{
		return m_Registry.has<IDComponent>(entity);
	}

	std::vector<LEntity> Scene::GetEntities()
	{
		auto view = m_Registry.view<IDComponent>();
		std::vector<LEntity> entities{};
		for (const auto& entity : view)
		{
			entities.push_back({ entity, this });
		}

		return entities;
	}

	bool Scene::HasEntity(LEntity entity) const
	{
		return (m_Registry.has<IDComponent>(entity) 
			&& (entity.m_Scene != nullptr) 
			&& (entity.m_EntityHandle != entt::null));
	}

	Ref<Scene> Scene::CreateEmpty()
	{
		return Ref<Scene>::Create("Empty", false, false);
	}

	void Scene::SortEntities()
	{
		m_Registry.sort<IDComponent>([&](const auto Lhs, const auto Rhs)
		{
			auto LhsEntity = m_EntityIDMap.find(Lhs.ID);
			auto RhsEntity = m_EntityIDMap.find(Rhs.ID);
			return static_cast<uint32_t>(LhsEntity->second) < static_cast<uint32_t>(RhsEntity->second);
		});
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
	void Scene::OnComponentAdded(LEntity Entity, T& TComponent)
	{
		LK_UNUSED(Entity); 
		LK_UNUSED(TComponent);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(LEntity Entity, IDComponent& id) 
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(LEntity Entity, TransformComponent& rigidbody)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteComponent>(LEntity Entity, SpriteComponent& sprite)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(LEntity Entity, CameraComponent& camera)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(LEntity Entity, MeshComponent& mesh)
	{
		// Add asset handle
	}

	template<>
	void Scene::OnComponentAdded<StaticMeshComponent>(LEntity Entity, StaticMeshComponent& staticMesh)
	{
	}

	template<>
	void Scene::OnComponentAdded<RigidBody2DComponent>(LEntity Entity, RigidBody2DComponent& rigidBody2DComponent)
	{
		auto sceneView = m_Registry.view<Box2DWorldComponent>();
		auto& world = m_Registry.get<Box2DWorldComponent>(sceneView.front()).World;

		LEntity e = { Entity, this };
		UUID entityID = e.GetUUID();
		TransformComponent& transform = e.Transform();
		auto& rigidBody2D = m_Registry.get<RigidBody2DComponent>(Entity);

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
			default:
				LK_CORE_ASSERT(false, "Rigidbody2DComponent was incorrectly created");
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
		//LK_CORE_DEBUG("OnComponentAdded<RigidBody2DComponent>  {}", entity.Name());
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(LEntity Entity, BoxCollider2DComponent& boxColliderComponent)
	{
		LEntity e = { Entity, this };
		auto& transform = e.Transform();

		if (e.HasComponent<RigidBody2DComponent>())
		{
			LK_ASSERT(transform.Scale.x * boxColliderComponent.Size.x > 0 && transform.Scale.y * boxColliderComponent.Size.y > 0);

			auto& rigidBody2D = e.RigidBody2D();
			b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);
			LK_ASSERT(body != nullptr);

			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(transform.Scale.x * boxColliderComponent.Size.x, transform.Scale.y * boxColliderComponent.Size.y);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &polygonShape;
			fixtureDef.density = boxColliderComponent.Density;
			fixtureDef.friction = boxColliderComponent.Friction;
			fixtureDef.isSensor = boxColliderComponent.IsSensor;
			fixtureDef.restitution = 0.30f;
			body->CreateFixture(&fixtureDef);
		}
	}

	void Scene::ParentEntity(LEntity Entity, LEntity Parent)
	{
		if (Parent.IsDescendantOf(Entity))
		{
			UnparentEntity(Parent);

			LEntity NewParent = TryGetEntityWithUUID(Entity.GetParentUUID());
			if (NewParent)
			{
				UnparentEntity(Entity);
				ParentEntity(Parent, NewParent);
			}
		}
		else
		{
			LEntity PreviousParent = TryGetEntityWithUUID(Entity.GetParentUUID());

			if (PreviousParent)
			{
				UnparentEntity(Entity);
			}
		}

		Entity.SetParentUUID(Parent.GetUUID());
		Parent.GetChildren().push_back(Entity.GetUUID());

		//ConvertToLocalSpace(entity);
	}

	void Scene::UnparentEntity(LEntity Entity, bool bConvertToWorldSpace)
	{
		LEntity Parent = TryGetEntityWithUUID(Entity.GetParentUUID());

		if (!Parent)
		{
			return;
		}

		auto& ParentChildren = Parent.GetChildren();
		ParentChildren.erase(std::remove(ParentChildren.begin(), ParentChildren.end(), Entity.GetUUID()), ParentChildren.end());

		//if (bConvertToWorldSpace)
			//ConvertToWorldSpace(entity);

		Entity.SetParentUUID(0);
	}

	void Scene::CopyTo(Ref<Scene>& target)
	{
		LK_CORE_DEBUG_TAG("Scene", "Starting to copy scene \"{}\"", Name);

		target->Name = Name;
		target->m_EditorScene = m_EditorScene;

		std::unordered_map<UUID, entt::entity> enttMap;
		auto idComponents = m_Registry.view<IDComponent>();
		for (auto entity : idComponents)
		{
			const auto uuid = m_Registry.get<IDComponent>(entity).ID; // Retrieve entity UUID
			const auto& name = m_Registry.get<TagComponent>(entity).Tag;     // Retrieve the name of the entity
			LEntity e = target->CreateEntityWithID(uuid, name);        // Create identical entity on the target scene 
			enttMap[uuid] = e.m_EntityHandle;
		}

#if 0
		auto targetView = target->m_Registry.view<IDComponent>();
		for (auto entity : targetView)
		{
			//UUID id = targetView.get<IDComponent>(entity).ID;
			target->m_Registry.destroy(entity);
		}
#endif

		CopyComponent<TagComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<TransformComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SpriteComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CameraComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<RigidBody2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<BoxCollider2DComponent>(target->m_Registry, m_Registry, enttMap);

		// This ensures a consistent ordering when iterating IDComponent's in the UI_SceneContent menu
		target->SortEntities();
		//target->SetPhysics2DGravity(GetPhysics2DGravity());

		target->m_ViewportWidth = m_ViewportWidth;
		target->m_ViewportHeight = m_ViewportHeight;
	}

	void Scene::OnRender(Ref<SceneRenderer> renderer, FTimestep Timestep)
	{
		SceneRendererCamera sceneCamera;
		sceneCamera.Camera = m_Camera;
		sceneCamera.ViewMatrix = m_Camera->GetViewMatrix();
		renderer->BeginScene(sceneCamera);
	}

	void Scene::OnRenderEditor(LEditorCamera& editorCamera, FTimestep Timestep)
	{
		editorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		editorCamera.OnUpdate(Timestep);

		Renderer::SubmitQuad({ 200, -20, 1350 }, { 3000, 1800 }, TextureLibrary::Get()->GetTexture("skybox-ice-back"));

		EditorLayer::Get()->OnUpdate();

#if 0
	#ifdef LK_DEBUG
		auto& world = GetBox2DWorld();
		world.World->DebugDraw();
	#endif
#endif
	}

	LEntity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto Entity : view)
		{
			auto& CameraComponentRef = view.get<CameraComponent>(Entity);
			if (CameraComponentRef.Primary)
			{
				LK_CORE_ASSERT(CameraComponentRef.Camera->GetOrthographicSize() 
					|| CameraComponentRef.Camera->GetDegPerspectiveVerticalFOV(), "Camera is not fully initialized");
				return { Entity, this };
			}
		}

		return {};
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

	void Scene::EndScene()
	{
		GetBox2DWorld().World->DebugDraw();
	}

	void Scene::SetCamera(Ref<SceneCamera> cam)
	{ 
		m_Camera = cam;
	}

	void Scene::SetCamera(SceneCamera* cam)
	{ 
		m_Camera = Ref<SceneCamera>(cam);
	}

	void Scene::SetEditorCamera(const Ref<LEditorCamera> InEditorCamera)
	{ 
		EditorCamera = InEditorCamera;
	}

	Box2DWorldComponent& Scene::GetBox2DWorld()
	{
		return m_Registry.get<Box2DWorldComponent>(m_SceneEntity);
	}

	void Scene::SetActiveScene(Ref<Scene>& scene)
	{
		m_ActiveScene = scene.Raw();
	}

	void Scene::Clear()
	{
		m_Registry.clear<AllComponents>();
	}

	void Scene::Initiate2DPhysics(const Physics2DSpecification& specification)
	{
		// Check to see if there already exists a 2D world for the scene and exit function if it does
		if (m_Registry.has<Box2DWorldComponent>(m_SceneEntity) == true)
		{
			LK_CORE_DEBUG_TAG("Scene", "2D physics already initialized for the scene \"{}\"", Name);
			auto& box2dWorld = m_Registry.get<Box2DWorldComponent>(m_SceneEntity);
			if (box2dWorld.HasDebugDrawerAttached() == false && specification.DebugDrawer == true)
				Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
		}
		else
		{
			b2Vec2 gravity = Utils::ConvertToB2(specification.Gravity);
			Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(gravity));
			box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);

			// Attach debug drawer
			if (specification.DebugDrawer)
			{
				Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
			}
		}
	}

	static void InsertMeshMaterials(Ref<MeshSource> meshSource, std::unordered_set<AssetHandle>& assetList)
	{
		// Mesh materials
		const auto& materials = meshSource->GetMaterials();
		for (auto material : materials)
		{
			Ref<Texture2D> albedoTexture = material->GetTexture("u_AlbedoTexture");
			if (albedoTexture && albedoTexture->Handle) // White texture has Handle == 0
			{
				assetList.insert(albedoTexture->Handle);
			}

			Ref<Texture2D> normalTexture = material->GetTexture("u_NormalTexture");
			if (normalTexture && albedoTexture->Handle)
			{
				assetList.insert(normalTexture->Handle);
			}

			Ref<Texture2D> metalnessTexture = material->GetTexture("u_MetalnessTexture");
			if (metalnessTexture && albedoTexture->Handle)
			{
				assetList.insert(metalnessTexture->Handle);
			}

			Ref<Texture2D> roughnessTexture = material->GetTexture("u_RoughnessTexture");
			if (roughnessTexture && albedoTexture->Handle)
			{

			}
				assetList.insert(roughnessTexture->Handle);
		}
	}


	std::unordered_set<AssetHandle> Scene::GetAssetList()
	{
		std::unordered_set<AssetHandle> assetList;
		std::unordered_set<AssetHandle> missingAssets;

		// MeshComponent
		{
			auto view = m_Registry.view<MeshComponent>();
			for (auto entity : view)
			{
				auto& mc = m_Registry.get<MeshComponent>(entity);
				if (mc.Mesh)
				{
					if (AssetManager::IsMemoryAsset(mc.Mesh))
					{
						LK_CORE_DEBUG_TAG("Scene", "Asset {} is memory asset", mc.Mesh);
						//continue;
					}

					if (AssetManager::IsAssetHandleValid(mc.Mesh))
					{
						assetList.insert(mc.Mesh);

						// MeshSource is required too
						Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(mc.Mesh);
						if (!mesh)
							continue;

						Ref<MeshSource> meshSource = mesh->GetMeshSource();
						if (meshSource && AssetManager::IsAssetHandleValid(meshSource->Handle))
						{
							assetList.insert(meshSource->Handle);
							InsertMeshMaterials(meshSource, assetList);
						}
					}
					else
					{
						missingAssets.insert(mc.Mesh);
					}
				}

				if (mc.MaterialTable)
				{
					auto& materialAssets = mc.MaterialTable->GetMaterials();
					for (auto& [index, materialAssetHandle] : materialAssets)
					{
						if (AssetManager::IsAssetHandleValid(materialAssetHandle))
						{
							assetList.insert(materialAssetHandle);

							Ref<MaterialAsset> materialAsset = AssetManager::GetAsset<MaterialAsset>(materialAssetHandle);

							std::array<Ref<Texture2D>, 4> textures = {
								materialAsset->GetAlbedoMap(),
								materialAsset->GetNormalMap(),
								materialAsset->GetMetalnessMap(),
								materialAsset->GetRoughnessMap()
							};

							// Textures
							for (auto& texture : textures)
							{
								if (texture)
									assetList.insert(texture->Handle);
							}
						}
						else
						{
							missingAssets.insert(materialAssetHandle);
						}
					}
				}
			}
		}

		// StaticMeshComponent
		{
			auto view = m_Registry.view<StaticMeshComponent>();
			for (auto entity : view)
			{
				auto& mc = m_Registry.get<StaticMeshComponent>(entity);
				if (mc.StaticMesh)
				{
					if (AssetManager::IsMemoryAsset(mc.StaticMesh))
					{
						//continue;
					}

					if (AssetManager::IsAssetHandleValid(mc.StaticMesh))
					{
						assetList.insert(mc.StaticMesh);

						// MeshSource is required too
						Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(mc.StaticMesh);
						Ref<MeshSource> meshSource = mesh->GetMeshSource();
						if (meshSource && AssetManager::IsAssetHandleValid(meshSource->Handle))
						{
							assetList.insert(meshSource->Handle);
							InsertMeshMaterials(meshSource, assetList);
						}
					}
					else
					{
						missingAssets.insert(mc.StaticMesh);
					}
				}

				if (mc.MaterialTable)
				{
					auto& materialAssets = mc.MaterialTable->GetMaterials();
					for (auto& [index, materialAssetHandle] : materialAssets)
					{
						if (AssetManager::IsAssetHandleValid(materialAssetHandle))
						{
							assetList.insert(materialAssetHandle);

							Ref<MaterialAsset> materialAsset = AssetManager::GetAsset<MaterialAsset>(materialAssetHandle);

							std::array<Ref<Texture2D>, 4> textures = {
								materialAsset->GetAlbedoMap(),
								materialAsset->GetNormalMap(),
								materialAsset->GetMetalnessMap(),
								materialAsset->GetRoughnessMap()
							};

							// Textures
							for (auto texture : textures)
							{
								if (texture)
									assetList.insert(texture->Handle);
							}
						}
						else
						{
							missingAssets.insert(materialAssetHandle);
						}
					}
				}
			}
		}

		LK_CORE_DEBUG_TAG("Scene", "Returning asset list of size={}", assetList.size());
		return assetList;
	}



}