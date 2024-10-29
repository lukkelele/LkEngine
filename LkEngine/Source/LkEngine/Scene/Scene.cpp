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

	///
	/// FIXME: Unify constructors here
	///

	LScene::LScene(const bool IsEditorScene)
		: bIsEditorScene(IsEditorScene)
		, Name("")
	{
		LCLASS_REGISTER();
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<LSceneComponent>(m_SceneEntity, m_SceneID);

		m_ViewportWidth = LWindow::Get().GetViewportWidth();
		m_ViewportHeight = LWindow::Get().GetViewportHeight();

		if (bIsEditorScene)
		{
			EditorCamera = LEditorLayer::Get()->GetEditorCamera();
			LK_CORE_ASSERT(EditorCamera, "EditorCamera is nullptr");

			LEditorLayer::Get()->SetScene(TObjectPtr<LScene>(this));

			/// FIXME
			LInput::SetScene(TObjectPtr<LScene>(this)); // REMOVE ME
		}

	#if LK_PHYSICS_ENABLED
		Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		Debugger::AttachDebugDrawer2D(&box2dWorld, 
									  Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
	#endif

		LK_CORE_DEBUG_TAG("Scene", "New scene created {}", Name);
		SceneCounter++;
	}

	LScene::LScene(const LString& InName, bool bIsActiveScene, bool bIsEditorScene)
		: bIsEditorScene(bIsEditorScene)
		, Name(InName)
	{
		LCLASS_REGISTER();
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<LSceneComponent>(m_SceneEntity, m_SceneID);

		m_ViewportWidth = LWindow::Get().GetViewportWidth();
		m_ViewportHeight = LWindow::Get().GetViewportHeight();

		if (bIsActiveScene)
		{
			bIsActiveScene = true;
			m_ActiveScene = this;

			/// FIXME
			LInput::SetScene(TObjectPtr<LScene>(this));
		}
		
		if (bIsEditorScene)
		{
			EditorCamera = LEditorLayer::Get()->GetEditorCamera();
			LEditorLayer::Get()->SetScene(TObjectPtr<LScene>(this));
		}

	#if LK_PHYSICS_ENABLED
		/* Initialize 2D physics. */
		Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
	#endif

		LK_CORE_DEBUG_TAG("Scene", "New scene created {}", Name);

		SceneCounter++;
	}

	LScene::~LScene()
	{
		/* TODO: */
	}

	LEntity LScene::CreateEntity(const std::string& name)
	{
		return CreateChildEntity({}, name);
	}

	LEntity LScene::CreateChildEntity(LEntity Parent, const std::string& name)
	{
		LEntity entity = LEntity { m_Registry.create(), this };
		LIDComponent& idComponent = entity.AddComponent<LIDComponent>();
		idComponent.ID = {};

		entity.AddComponent<LTransformComponent>();
		if (!name.empty())
		{
			entity.AddComponent<LTagComponent>(name);
		}

		entity.AddComponent<LRelationshipComponent>();

		if (Parent)
		{
			entity.SetParent(Parent);
		}

		m_EntityIDMap[idComponent.ID] = entity;

		SortEntities();

		return entity;
	}

	LEntity LScene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		LEntity entity = { m_Registry.create(), this };
		entity.AddComponent<LIDComponent>(uuid);

		LTagComponent& TagComponent = entity.AddComponent<LTagComponent>();
		TagComponent.Tag = name.empty() ? "Entity" : name;
		m_EntityIDMap[uuid] = entity;

		return entity;
	}

	// TODO: Change name to GetEntityWithID
	LEntity LScene::GetEntityWithUUID(UUID ID) const
	{
		LK_CORE_VERIFY(m_EntityIDMap.find(ID) != m_EntityIDMap.end(), "Entity with ID \"{}\" is not present in the scene", ID);
		return m_EntityIDMap.at(ID);
#if 0
		if (m_EntityIDMap.find(ID) != m_EntityIDMap.end())
		{
			return { m_EntityIDMap.at(ID), this };
		}

		return { entt::null, nullptr };
#endif
	}

	LEntity LScene::TryGetEntityWithUUID(UUID id) const
	{
		if (const auto Iter = m_EntityIDMap.find(id); Iter != m_EntityIDMap.end())
		{
			return Iter->second;
		}

		return LEntity{};
	}

	LEntity LScene::FindEntity(std::string_view name)
	{
		auto view = m_Registry.view<LTagComponent>();
		for (auto entity : view)
		{
			const LTagComponent& tc = view.get<LTagComponent>(entity);
			if (tc.Tag == name)
			{
				return LEntity{ entity , this };
			}
		}

		return {};
	}

	void LScene::DestroyEntity(LEntity entity)
	{
		m_EntityIDMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
		LK_CORE_DEBUG("Entity {} deleted", entity.Name());
	}

	bool LScene::IsEntityInRegistry(LEntity entity) const
	{
		return m_Registry.has<LIDComponent>(entity);
	}

	std::vector<LEntity> LScene::GetEntities()
	{
		auto view = m_Registry.view<LIDComponent>();
		std::vector<LEntity> entities{};
		for (const auto& entity : view)
		{
			entities.push_back({ entity, this });
		}

		return entities;
	}

	bool LScene::HasEntity(LEntity entity) const
	{
		return (m_Registry.has<LIDComponent>(entity) 
			&& (entity.m_Scene != nullptr) 
			&& (entity.m_EntityHandle != entt::null));
	}

	TObjectPtr<LScene> LScene::CreateEmpty()
	{
		return TObjectPtr<LScene>::Create("Empty", false, false);
	}

	void LScene::SortEntities()
	{
		m_Registry.sort<LIDComponent>([&](const auto Lhs, const auto Rhs)
		{
			auto LhsEntity = m_EntityIDMap.find(Lhs.ID);
			auto RhsEntity = m_EntityIDMap.find(Rhs.ID);
			return static_cast<uint32_t>(LhsEntity->second) < static_cast<uint32_t>(RhsEntity->second);
		});
	}

	void LScene::Pause(const bool IsPaused)
	{
		bPaused = IsPaused;
	#if 0
		m_World->Pause(IsPaused);
	#endif
	}

	void LScene::SwitchCamera()
	{
	}

	template<typename T>
	void LScene::OnComponentAdded(LEntity Entity, T& TComponent)
	{
		LK_UNUSED(Entity); 
		LK_UNUSED(TComponent);
	}

	template<>
	void LScene::OnComponentAdded<LIDComponent>(LEntity Entity, LIDComponent& id) 
	{
	}

	template<>
	void LScene::OnComponentAdded<LTransformComponent>(LEntity Entity, LTransformComponent& rigidbody)
	{
	}

	template<>
	void LScene::OnComponentAdded<LSpriteComponent>(LEntity Entity, LSpriteComponent& SpriteComponent)
	{
	}

	template<>
	void LScene::OnComponentAdded<LCameraComponent>(LEntity Entity, LCameraComponent& LCameraComponent)
	{
	}

	template<>
	void LScene::OnComponentAdded<LMeshComponent>(LEntity Entity, LMeshComponent& MeshComponent)
	{
		// Add asset handle
	}

	template<>
	void LScene::OnComponentAdded<LStaticMeshComponent>(LEntity Entity, LStaticMeshComponent& StaticMeshComponent)
	{
	}

	template<>
	void LScene::OnComponentAdded<LRigidBody2DComponent>(LEntity Entity, LRigidBody2DComponent& rigidBody2DComponent)
	{
		auto sceneView = m_Registry.view<Box2DWorldComponent>();
		auto& world = m_Registry.get<Box2DWorldComponent>(sceneView.front()).World;

		LEntity e = { Entity, this };
		UUID entityID = e.GetUUID();
		LTransformComponent& transform = e.Transform();
		auto& rigidBody2D = m_Registry.get<LRigidBody2DComponent>(Entity);

		b2BodyDef bodyDef;
		switch (rigidBody2D.BodyType)
		{
			case LRigidBody2DComponent::Type::Static:
			{
				bodyDef.type = b2_staticBody;
				break;
			}

			case LRigidBody2DComponent::Type::Dynamic: 
			{
				bodyDef.type = b2_dynamicBody;
				break;
			}

			case LRigidBody2DComponent::Type::Kinematic: 
			{
				bodyDef.type = b2_kinematicBody;
				break;
			}

			default:
			{
				LK_CORE_ASSERT(false, "Rigidbody2DComponent was incorrectly created");
				break;
			}
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
		//LK_CORE_DEBUG("OnComponentAdded<LRigidBody2DComponent>  {}", entity.Name());
	}

	template<>
	void LScene::OnComponentAdded<LBoxCollider2DComponent>(LEntity InEntity, LBoxCollider2DComponent& boxColliderComponent)
	{
		LEntity Entity = { InEntity, this };
		auto& transform = Entity.Transform();

		if (Entity.HasComponent<LRigidBody2DComponent>())
		{
			auto& rigidBody2D = Entity.GetComponent<LRigidBody2DComponent>();
			b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

			LK_ASSERT((transform.Scale.x * boxColliderComponent.Size.x > 0)
					  && (transform.Scale.y * boxColliderComponent.Size.y > 0));
			LK_ASSERT(body, "RigidBody b2Body is nullptr");

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

	void LScene::ParentEntity(LEntity Entity, LEntity Parent)
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

	void LScene::UnparentEntity(LEntity Entity, bool bConvertToWorldSpace)
	{
		LEntity Parent = TryGetEntityWithUUID(Entity.GetParentUUID());

		if (!Parent)
		{
			return;
		}

		auto& ParentChildren = Parent.GetChildren();
		ParentChildren.erase(std::remove(ParentChildren.begin(), ParentChildren.end(), Entity.GetUUID()), ParentChildren.end());

		//if (bConvertToWorldSpace)
		//{
		//	ConvertToWorldSpace(entity);
		//}

		Entity.SetParentUUID(0);
	}

	void LScene::CopyTo(TObjectPtr<LScene>& target)
	{
		LK_CORE_DEBUG_TAG("Scene", "Starting to copy scene \"{}\"", Name);

		target->Name = Name;
		target->bIsEditorScene = bIsEditorScene;

		std::unordered_map<UUID, entt::entity> enttMap;
		auto idComponents = m_Registry.view<LIDComponent>();
		for (auto entity : idComponents)
		{
			const auto uuid = m_Registry.get<LIDComponent>(entity).ID; // Retrieve entity UUID
			const auto& name = m_Registry.get<LTagComponent>(entity).Tag;     // Retrieve the name of the entity
			LEntity e = target->CreateEntityWithID(uuid, name);        // Create identical entity on the target scene 
			enttMap[uuid] = e.m_EntityHandle;
		}

#if 0
		auto targetView = target->m_Registry.view<LIDComponent>();
		for (auto entity : targetView)
		{
			//UUID id = targetView.get<LIDComponent>(entity).ID;
			target->m_Registry.destroy(entity);
		}
#endif

		CopyComponent<LTagComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<LTransformComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<LSpriteComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<LCameraComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<LRigidBody2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<LBoxCollider2DComponent>(target->m_Registry, m_Registry, enttMap);

		// This ensures a consistent ordering when iterating LIDComponent's in the UI_SceneContent menu
		target->SortEntities();
		//target->SetPhysics2DGravity(GetPhysics2DGravity());

		target->m_ViewportWidth = m_ViewportWidth;
		target->m_ViewportHeight = m_ViewportHeight;
	}

	void LScene::OnRender(TObjectPtr<LSceneRenderer> renderer, FTimestep Timestep)
	{
		SceneRendererCamera sceneCamera;
		sceneCamera.Camera = m_Camera;
		sceneCamera.ViewMatrix = m_Camera->GetViewMatrix();

		renderer->BeginScene(sceneCamera);
	}

	void LScene::OnRenderEditor(LEditorCamera& EditorCamera, FTimestep Timestep)
	{
		EditorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		EditorCamera.OnUpdate(Timestep);

#if 0
		/* FIXME: This should NOT be here ... */
		LRenderer::SubmitQuad({ 200, -20, 1350 }, 
							  { 3000, 1800 }, 
							  LTextureLibrary::Get().GetTexture("skybox-ice-back"));

		LEditorLayer::Get()->OnUpdate(); /// UPDATE
#endif
	}

	LEntity LScene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<LCameraComponent>();
		for (auto Entity : view)
		{
			auto& LCameraComponentRef = view.get<LCameraComponent>(Entity);
			if (LCameraComponentRef.Primary)
			{
				LK_CORE_ASSERT(LCameraComponentRef.Camera->GetOrthographicSize() 
					|| LCameraComponentRef.Camera->GetDegPerspectiveVerticalFOV(), "Camera is not fully initialized");
				return { Entity, this };
			}
		}

		return {};
	}

#if 0
	void LScene::BeginScene(Timestep ts)
	{
		if (m_Camera == nullptr)
			return;
		BeginScene(*m_Camera, ts);
	}

	void LScene::BeginScene(SceneCamera& cam, Timestep ts)
	{
		cam.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		cam.Update(ts);
	}
#endif

	void LScene::EndScene()
	{
		GetBox2DWorld().World->DebugDraw();
	}

	void LScene::SetCamera(TObjectPtr<LSceneCamera> cam)
	{ 
		m_Camera = cam;
	}

	void LScene::SetCamera(LSceneCamera* cam)
	{ 
		m_Camera = TObjectPtr<LSceneCamera>(cam);
	}

	void LScene::SetEditorCamera(const TObjectPtr<LEditorCamera> InEditorCamera)
	{ 
		EditorCamera = InEditorCamera;
	}

	Box2DWorldComponent& LScene::GetBox2DWorld()
	{
		return m_Registry.get<Box2DWorldComponent>(m_SceneEntity);
	}

	void LScene::SetActiveScene(TObjectPtr<LScene> InScene)
	{
		// TODO: Verify scene validity
		m_ActiveScene = InScene;
	}

	void LScene::Clear()
	{
		m_Registry.clear<AllComponents>();
	}

	void LScene::Initiate2DPhysics(const Physics2DSpecification& specification)
	{
		// Check to see if there already exists a 2D world for the scene and exit function if it does
		if (m_Registry.has<Box2DWorldComponent>(m_SceneEntity) == true)
		{
			LK_CORE_DEBUG_TAG("Scene", "2D physics already initialized for the scene \"{}\"", Name);
			auto& box2dWorld = m_Registry.get<Box2DWorldComponent>(m_SceneEntity);
			if (!box2dWorld.HasDebugDrawerAttached() && specification.DebugDrawer)
			{
				//Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
			}
		}
		else
		{
			b2Vec2 gravity = Utils::ConvertToB2(specification.Gravity);
			Box2DWorldComponent& box2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, std::make_unique<b2World>(gravity));
			box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);

			// Attach debug drawer
			if (specification.DebugDrawer)
			{
				//Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
			}
		}
	}

	static void InsertMeshMaterials(TObjectPtr<LMeshSource> meshSource, 
									std::unordered_set<FAssetHandle>& assetList)
	{
		// Mesh materials
		const auto& materials = meshSource->GetMaterials();
		for (const auto& material : materials)
		{
			TObjectPtr<LTexture2D> albedoTexture = material->GetTexture("u_AlbedoTexture");
			if (albedoTexture && albedoTexture->Handle) // White texture has Handle == 0
			{
				assetList.insert(albedoTexture->Handle);
			}

			TObjectPtr<LTexture2D> normalTexture = material->GetTexture("u_NormalTexture");
			if (normalTexture && albedoTexture->Handle)
			{
				assetList.insert(normalTexture->Handle);
			}

			TObjectPtr<LTexture2D> metalnessTexture = material->GetTexture("u_MetalnessTexture");
			if (metalnessTexture && albedoTexture->Handle)
			{
				assetList.insert(metalnessTexture->Handle);
			}

			TObjectPtr<LTexture2D> roughnessTexture = material->GetTexture("u_RoughnessTexture");
			if (roughnessTexture && albedoTexture->Handle)
			{

			}
				assetList.insert(roughnessTexture->Handle);
		}
	}


	std::unordered_set<FAssetHandle> LScene::GetAssetList()
	{
		std::unordered_set<FAssetHandle> assetList;
		std::unordered_set<FAssetHandle> missingAssets;

		// LMeshComponent
		{
			auto view = m_Registry.view<LMeshComponent>();
			for (auto entity : view)
			{
				auto& mc = m_Registry.get<LMeshComponent>(entity);
				if (mc.Mesh)
				{
					if (LAssetManager::IsMemoryAsset(mc.Mesh))
					{
						LK_CORE_DEBUG_TAG("Scene", "Asset {} is memory asset", mc.Mesh);
						//continue;
					}

					if (LAssetManager::IsAssetHandleValid(mc.Mesh))
					{
						/* FIXME: Should this be inserted before MeshSource is validated? */
						assetList.insert(mc.Mesh);

						/* MeshSource is required too. */
						TObjectPtr<LMesh> mesh = LAssetManager::GetAsset<LMesh>(mc.Mesh);
						if (!mesh)
						{
							continue;
						}

						TObjectPtr<LMeshSource> meshSource = mesh->GetMeshSource();
						if (meshSource && LAssetManager::IsAssetHandleValid(meshSource->Handle))
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
						if (LAssetManager::IsAssetHandleValid(materialAssetHandle))
						{
							assetList.insert(materialAssetHandle);

							TObjectPtr<LMaterialAsset> materialAsset = LAssetManager::GetAsset<LMaterialAsset>(materialAssetHandle);

							std::array<TObjectPtr<LTexture2D>, 4> textures = {
								materialAsset->GetAlbedoMap(),
								materialAsset->GetNormalMap(),
								materialAsset->GetMetalnessMap(),
								materialAsset->GetRoughnessMap()
							};

							/* Textures. */
							for (const TObjectPtr<LTexture2D>& Texture : textures)
							{
								if (Texture)
								{
									assetList.insert(Texture->Handle);
								}
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

		/* LStaticMeshComponent. */
		{
			auto view = m_Registry.view<LStaticMeshComponent>();
			for (auto entity : view)
			{
				auto& mc = m_Registry.get<LStaticMeshComponent>(entity);
				if (mc.StaticMesh)
				{
					if (LAssetManager::IsMemoryAsset(mc.StaticMesh))
					{
						/* continue; */
					}

					if (LAssetManager::IsAssetHandleValid(mc.StaticMesh))
					{
						assetList.insert(mc.StaticMesh);

						// MeshSource is required too
						TObjectPtr<LStaticMesh> mesh = LAssetManager::GetAsset<LStaticMesh>(mc.StaticMesh);
						TObjectPtr<LMeshSource> meshSource = mesh->GetMeshSource();
						if (meshSource && LAssetManager::IsAssetHandleValid(meshSource->Handle))
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
						if (LAssetManager::IsAssetHandleValid(materialAssetHandle))
						{
							assetList.insert(materialAssetHandle);

							TObjectPtr<LMaterialAsset> materialAsset = LAssetManager::GetAsset<LMaterialAsset>(materialAssetHandle);

							std::array<TObjectPtr<LTexture2D>, 4> textures = {
								materialAsset->GetAlbedoMap(),
								materialAsset->GetNormalMap(),
								materialAsset->GetMetalnessMap(),
								materialAsset->GetRoughnessMap()
							};

							// Textures
							for (const TObjectPtr<LTexture2D>& texture : textures)
							{
								if (texture)
								{
									assetList.insert(texture->Handle);
								}
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