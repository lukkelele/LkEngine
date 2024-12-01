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

	/* TODO: MOVE FUNCTION */
	static void InsertMeshMaterials(TObjectPtr<LMeshSource> MeshSource, std::unordered_set<FAssetHandle>& AssetList);

	FOnSceneSetActive GOnSceneSetActive{};
	FOnSceneCreated   GOnSceneCreated{};

	LScene::LScene(const std::string& SceneName, const bool IsEditorScene)
		: Name(SceneName)
		, bIsEditorScene(IsEditorScene)
	{
		LCLASS_REGISTER();

		SceneEntity = Registry.create();
		LK_CORE_DEBUG_TAG("Scene", "Scene created '{}' with handle '{}'", Name, SceneEntity);

		/* The file extension is handled by LSceneSerializer. */
		const std::filesystem::path SceneFile = LK_FORMAT_STRING("Scenes/{}", Name);

		/* Attempt to load scene data, if any exist. */
		LSceneSerializer Serializer(this);
		if (Serializer.Deserialize(SceneFile))
		{
			LK_CORE_DEBUG("Scene serialization OK for '{}'", Name);
		}
		else
		{
			LK_CORE_WARN("Scene serialization failed, loading default values for '{}'", Name);

			Registry.emplace<LSceneComponent>(SceneEntity, SceneID);

			m_ViewportWidth = LWindow::Get().GetViewportWidth();
			m_ViewportHeight = LWindow::Get().GetViewportHeight();
			LK_CORE_DEBUG_TAG("Scene", "Viewport ({}, {})", m_ViewportWidth, m_ViewportHeight);
		}

		LK_INFO("Created scene called '{}'", Name);
	}

	LEntity LScene::CreateEntity(const std::string& name)
	{
		return CreateChildEntity({}, name);
	}

	LEntity LScene::CreateChildEntity(LEntity Parent, const std::string& Name)
	{
		LEntity Entity = LEntity{ Registry.create(), this };
		LIDComponent& IDComp = Entity.AddComponent<LIDComponent>();
		IDComp.ID = {};

		Entity.AddComponent<LTransformComponent>();
		if (!Name.empty())
		{
			Entity.AddComponent<LTagComponent>(Name);
		}

		Entity.AddComponent<LRelationshipComponent>();

		if (Parent)
		{
			Entity.SetParent(Parent);
		}

		m_EntityIDMap[IDComp.ID] = Entity;

		SortEntities();

		return Entity;
	}

	LEntity LScene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		LEntity Entity = { Registry.create(), this };
		Entity.AddComponent<LIDComponent>(uuid);

		LTagComponent& TagComponent = Entity.AddComponent<LTagComponent>();
		TagComponent.Tag = name.empty() ? "Entity" : name;
		m_EntityIDMap[uuid] = Entity;

		return Entity;
	}

	// TODO: Change name to GetEntityWithID
	LEntity LScene::GetEntityWithUUID(UUID ID) const
	{
		LK_ASSERT(m_EntityIDMap.find(ID) != m_EntityIDMap.end(), "Entity \"{}\" is not present in the scene", ID);
		return m_EntityIDMap.at(ID);
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
		/* TODO: Use predicate instead of for-loop like this. */
		auto View = Registry.view<LTagComponent>();
		for (auto Entity : View)
		{
			const LTagComponent& tc = View.get<LTagComponent>(Entity);
			if (tc.Tag == name)
			{
				return LEntity{ Entity , this };
			}
		}

		return {};
	}

	std::vector<LEntity> LScene::GetEntities()
	{
		auto view = Registry.view<LIDComponent>();
		std::vector<LEntity> entities{};
		for (const auto& Entity : view)
		{
			entities.push_back({ Entity, this });
		}

		return entities;
	}

	void LScene::DestroyEntity(const LEntity Entity)
	{
		m_EntityIDMap.erase(Entity.GetUUID());
		Registry.destroy(Entity);
		LK_CORE_DEBUG("Entity {} deleted", Entity.Name());
	}

	bool LScene::HasEntity(const LEntity Entity) const
	{
		return (Registry.all_of<LIDComponent>(Entity) && Entity.Scene && (Entity.Handle != entt::null));
	}

	bool LScene::IsEntityInRegistry(const LEntity Entity) const
	{
		return Registry.all_of<LIDComponent>(Entity);
	}

	void LScene::SortEntities()
	{
		Registry.sort<LIDComponent>([&](const auto Lhs, const auto Rhs)
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
		#if 0 /// DISABLED
		auto sceneView = Registry.view<Box2DWorldComponent>();
		auto& world = Registry.get<Box2DWorldComponent>(sceneView.front()).World;

		LEntity e = { Entity, this };
		UUID EntityID = e.GetUUID();
		LTransformComponent& transform = e.Transform();
		auto& rigidBody2D = Registry.get<LRigidBody2DComponent>(Entity);

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
		body->GetUserData().pointer = (uintptr_t)EntityID;
		rigidBody2D.RuntimeBody = body;
		#endif
		//LK_CORE_DEBUG("OnComponentAdded<LRigidBody2DComponent>  {}", Entity.Name());
	}

	template<>
	void LScene::OnComponentAdded<LBoxCollider2DComponent>(LEntity InEntity, LBoxCollider2DComponent& boxColliderComponent)
	{
		LEntity Entity = { InEntity, this };
		auto& transform = Entity.Transform();

		#if 0 /// DISABLED
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

		#endif
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

		//ConvertToLocalSpace(Entity);
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
		//	ConvertToWorldSpace(Entity);
		//}

		Entity.SetParentUUID(0);
	}

	void LScene::CopyTo(TObjectPtr<LScene>& TargetScene)
	{
		LK_CORE_DEBUG_TAG("Scene", "Starting to copy scene \"{}\"", Name);

		TargetScene->Name = Name;
		TargetScene->bIsEditorScene = bIsEditorScene;

		std::unordered_map<UUID, entt::entity> EntityMap;
		auto View = Registry.view<LIDComponent>();
		for (auto Entity : View)
		{
			/* Retrieve the UUID and the entity name. */
			const UUID EntityUUID = Registry.get<LIDComponent>(Entity).ID;
			const std::string& EntityName = Registry.get<LTagComponent>(Entity).Tag;

			/* Create identical Entity on the target scene. */
			LEntity EntityCopy = TargetScene->CreateEntityWithID(EntityUUID, EntityName);
			EntityMap[EntityUUID] = EntityCopy.Handle;
		}

#if 0
		auto targetView = TargetScene->Registry.view<LIDComponent>();
		for (auto Entity : targetView)
		{
			//UUID id = targetView.get<LIDComponent>(Entity).ID;
			TargetScene->Registry.destroy(Entity);
		}
#endif

		CopyComponent<LTagComponent>(TargetScene->Registry, Registry, EntityMap);
		CopyComponent<LTransformComponent>(TargetScene->Registry, Registry, EntityMap);
		CopyComponent<LSpriteComponent>(TargetScene->Registry, Registry, EntityMap);
		CopyComponent<LCameraComponent>(TargetScene->Registry, Registry, EntityMap);
		CopyComponent<LRigidBody2DComponent>(TargetScene->Registry, Registry, EntityMap);
		CopyComponent<LBoxCollider2DComponent>(TargetScene->Registry, Registry, EntityMap);

		/* This ensures a consistent ordering when iterating ID's in the UI_SceneContent menu. */
		TargetScene->SortEntities();

		TargetScene->m_ViewportWidth = m_ViewportWidth;
		TargetScene->m_ViewportHeight = m_ViewportHeight;
	}

	void LScene::OnRender(TObjectPtr<LSceneRenderer> SceneRenderer, FTimestep Timestep)
	{
		FSceneRendererCamera SceneCamera;
		SceneCamera.Camera = Camera;
		SceneCamera.ViewMatrix = Camera->GetViewMatrix();

		SceneRenderer->BeginScene(SceneCamera);
	}

	void LScene::OnRenderEditor(LEditorCamera& EditorCamera, FTimestep Timestep)
	{
		EditorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		EditorCamera.OnUpdate(Timestep);
	}

	LEntity LScene::GetMainCameraEntity()
	{
		auto EntityView = Registry.view<LCameraComponent>();
		for (auto Entity : EntityView)
		{
			LCameraComponent& CameraComponent = EntityView.get<LCameraComponent>(Entity);
			if (CameraComponent.Primary)
			{
				LK_VERIFY(
					CameraComponent.Camera->GetOrthographicSize() || CameraComponent.Camera->GetDegPerspectiveVerticalFOV(), 
					"Camera is not fully initialized"
				);

				return { Entity, this };
			}
		}

		return {};
	}

	void LScene::EndScene()
	{
	}

	void LScene::SetCamera(TObjectPtr<LSceneCamera> InCamera)
	{ 
		LK_VERIFY(InCamera);
		Camera = InCamera;
	}

	void LScene::SetActiveScene(TObjectPtr<LScene> InScene)
	{
		LK_VERIFY(InScene, "Invalid scene");
		LK_DEBUG("Set active scene to: '{}'", InScene->GetName());
		ActiveScene = InScene;
	}

	void LScene::SetActive(const bool Active)
	{
		if (Active && (ActiveScene.Get() != this))
		{
			LK_CORE_DEBUG("Setting active scene: {}", GetName());
			ActiveScene = this;
			LK_CORE_VERIFY(ActiveScene, "Active scene reference is not valid");

			GOnSceneSetActive.Broadcast(this);
		}
	}

	void LScene::Clear()
	{
		Registry.clear<AllComponents>();
	}

	std::unordered_set<FAssetHandle> LScene::GetAssetList()
	{
		std::unordered_set<FAssetHandle> AssetList;
		std::unordered_set<FAssetHandle> MissingAssets;

		// LMeshComponent
		{
			auto MeshView = Registry.view<LMeshComponent>();
			for (auto Entity : MeshView)
			{
				LMeshComponent& MeshComp = Registry.get<LMeshComponent>(Entity);
				if (MeshComp.Mesh)
				{
					if (LAssetManager::IsMemoryAsset(MeshComp.Mesh))
					{
						LK_CORE_DEBUG_TAG("Scene", "Asset {} is memory asset", MeshComp.Mesh);
						//continue;
					}

					if (LAssetManager::IsAssetHandleValid(MeshComp.Mesh))
					{
						/* FIXME: Should this be inserted before MeshSource is validated? */
						AssetList.insert(MeshComp.Mesh);

						/* MeshSource is required too. */
						TObjectPtr<LMesh> mesh = LAssetManager::GetAsset<LMesh>(MeshComp.Mesh);
						if (!mesh)
						{
							continue;
						}

						TObjectPtr<LMeshSource> MeshSource = mesh->GetMeshSource();
						if (MeshSource && LAssetManager::IsAssetHandleValid(MeshSource->Handle))
						{
							AssetList.insert(MeshSource->Handle);
							InsertMeshMaterials(MeshSource, AssetList);
						}
					}
					else
					{
						MissingAssets.insert(MeshComp.Mesh);
					}
				}

				if (MeshComp.MaterialTable)
				{
					auto& MaterialAssets = MeshComp.MaterialTable->GetMaterials();
					for (auto& [index, MatAssetHandle] : MaterialAssets)
					{
						if (LAssetManager::IsAssetHandleValid(MatAssetHandle))
						{
							AssetList.insert(MatAssetHandle);

							TObjectPtr<LMaterialAsset> MatAsset = LAssetManager::GetAsset<LMaterialAsset>(MatAssetHandle);

							std::array<TObjectPtr<LTexture2D>, 4> Textures = {
								MatAsset->GetAlbedoMap(),
								MatAsset->GetNormalMap(),
								MatAsset->GetMetalnessMap(),
								MatAsset->GetRoughnessMap()
							};

							/* Textures. */
							for (const TObjectPtr<LTexture2D>& Texture : Textures)
							{
								if (Texture)
								{
									AssetList.insert(Texture->Handle);
								}
							}
						}
						else
						{
							MissingAssets.insert(MatAssetHandle);
						}
					}
				}
			}
		}

		/* LStaticMeshComponent. */
		{
			auto StaticMeshView = Registry.view<LStaticMeshComponent>();
			for (auto Entity : StaticMeshView)
			{
				auto& StaticMeshComp = Registry.get<LStaticMeshComponent>(Entity);
				if (StaticMeshComp.StaticMesh)
				{
					if (LAssetManager::IsMemoryAsset(StaticMeshComp.StaticMesh))
					{
						/* continue; */
					}

					if (LAssetManager::IsAssetHandleValid(StaticMeshComp.StaticMesh))
					{
						AssetList.insert(StaticMeshComp.StaticMesh);

						// MeshSource is required too
						TObjectPtr<LStaticMesh> mesh = LAssetManager::GetAsset<LStaticMesh>(StaticMeshComp.StaticMesh);
						TObjectPtr<LMeshSource> MeshSource = mesh->GetMeshSource();
						if (MeshSource && LAssetManager::IsAssetHandleValid(MeshSource->Handle))
						{
							AssetList.insert(MeshSource->Handle);
							InsertMeshMaterials(MeshSource, AssetList);
						}
					}
					else
					{
						MissingAssets.insert(StaticMeshComp.StaticMesh);
					}
				}

				if (StaticMeshComp.MaterialTable)
				{
					auto& MaterialAssets = StaticMeshComp.MaterialTable->GetMaterials();
					for (auto& [index, MatAssetHandle] : MaterialAssets)
					{
						if (LAssetManager::IsAssetHandleValid(MatAssetHandle))
						{
							AssetList.insert(MatAssetHandle);

							TObjectPtr<LMaterialAsset> MatAsset = LAssetManager::GetAsset<LMaterialAsset>(MatAssetHandle);
							std::array<TObjectPtr<LTexture2D>, 4> Textures = {
								MatAsset->GetAlbedoMap(),
								MatAsset->GetNormalMap(),
								MatAsset->GetMetalnessMap(),
								MatAsset->GetRoughnessMap()
							};

							// Textures
							for (const TObjectPtr<LTexture2D>& Texture : Textures)
							{
								if (Texture)
								{
									AssetList.insert(Texture->Handle);
								}
							}
						}
						else
						{
							MissingAssets.insert(MatAssetHandle);
						}
					}
				}
			}
		}

		LK_CORE_DEBUG_TAG("Scene", "Returning asset list of size={}", AssetList.size());
		return AssetList;
	}

	static void InsertMeshMaterials(TObjectPtr<LMeshSource> MeshSource, std::unordered_set<FAssetHandle>& AssetList)
	{
		// Mesh materials
		const auto& materials = MeshSource->GetMaterials();
		for (const auto& material : materials)
		{
			TObjectPtr<LTexture2D> albedoTexture = material->GetTexture("u_AlbedoTexture");
			if (albedoTexture && albedoTexture->Handle) // White texture has Handle == 0
			{
				AssetList.insert(albedoTexture->Handle);
			}

			TObjectPtr<LTexture2D> normalTexture = material->GetTexture("u_NormalTexture");
			if (normalTexture && albedoTexture->Handle)
			{
				AssetList.insert(normalTexture->Handle);
			}

			TObjectPtr<LTexture2D> metalnessTexture = material->GetTexture("u_MetalnessTexture");
			if (metalnessTexture && albedoTexture->Handle)
			{
				AssetList.insert(metalnessTexture->Handle);
			}

			TObjectPtr<LTexture2D> roughnessTexture = material->GetTexture("u_RoughnessTexture");
			if (roughnessTexture && albedoTexture->Handle)
			{

			}
				AssetList.insert(roughnessTexture->Handle);
		}
	}

}
