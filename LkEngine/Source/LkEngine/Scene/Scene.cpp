#include "LKpch.h"
#include "Scene.h"

#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/SceneRenderer.h"

#include "LkEngine/Core/Application.h"

namespace LkEngine {

	/* TODO: MOVE FUNCTION */
	static void InsertMeshMaterials(TObjectPtr<LMeshSource> MeshSource, std::unordered_set<LUUID>& AssetList);

	FOnSceneSetActive GOnSceneSetActive{};
	FOnSceneCreated   GOnSceneCreated{};

	LScene::LScene(const std::string& SceneName, const bool IsEditorScene)
		: Name(SceneName)
		, bEditorScene(IsEditorScene)
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(!SceneName.empty(), "Scene requires a name");

		SceneEntity = Registry.create();
		LK_CORE_TRACE_TAG("Scene", "Created {} with handle: {}", Name, SceneEntity);

		ViewportWidth = LWindow::Get().GetViewportWidth();
		ViewportHeight = LWindow::Get().GetViewportHeight();
	}

	LScene::~LScene()
	{
		LK_CORE_WARN_TAG("Scene", "Destroying: {}", !Name.empty() ? Name : "NULL");
	}

	LEntity LScene::CreateEntity(const std::string& EntityName)
	{
		return CreateChildEntity({}, EntityName);
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

		EntityMap[IDComp.ID] = Entity;

		SortEntities();

		return Entity;
	}

	LEntity LScene::CreateEntityWithID(const LUUID UUID, const std::string& Name)
	{
		LEntity Entity = { Registry.create(), this };
		Entity.AddComponent<LIDComponent>(UUID);

		LTagComponent& TagComponent = Entity.AddComponent<LTagComponent>();
		TagComponent.Tag = Name.empty() ? "Entity" : Name;
		EntityMap[UUID] = Entity;

		return Entity;
	}

	LEntity LScene::GetEntityWithUUID(const LUUID ID)
	{
		LK_CORE_ASSERT(EntityMap.find(ID) != EntityMap.end(), "Entity '{}' is not present in the scene", ID);
		/** @fixme: FIXME */
		//return EntityMap.at(ID);
		return LEntity((entt::entity)EntityMap.at(ID), this);
	}

	LEntity LScene::TryGetEntityWithUUID(LUUID ID)
	{
		if (const auto Iter = EntityMap.find(ID); Iter != EntityMap.end())
		{
			//return Iter->second;
			/** @fixme: FIXME */
			return LEntity(Iter->second, this);
		}

		return LEntity{};
	}

	LEntity LScene::FindEntity(std::string_view Name)
	{
		auto View = Registry.view<LTagComponent>();
		for (auto Entity : View)
		{
			const LTagComponent& TagComp = View.get<LTagComponent>(Entity);
			if (TagComp.Tag == Name)
			{
				return LEntity{ Entity , this };
			}
		}

		return {};
	}

	template<typename T>
	std::vector<T> LScene::GetEntities()
	{
		static_assert(std::disjunction_v<std::is_same<T, LEntity>, std::is_same<T, LUUID>>, "Invalid type T");
		return {};
	}

	template<>
	std::vector<LEntity> LScene::GetEntities()
	{
		auto View = Registry.view<LIDComponent>();
		std::vector<LEntity> Entities;
		Entities.resize(View.size());
		for (auto EntityID : View)
		{
			Entities.push_back({ EntityID, this });
		}

		return Entities;
	}

	template<>
	std::vector<LUUID> LScene::GetEntities()
	{
		auto View = Registry.view<LIDComponent>();
		std::vector<LUUID> Entities;
		Entities.resize(View.size());
		for (auto EntityID : View)
		{
			/* FIXME: Must be another way of converting an EntityID to a UUID. */
			//Entities.push_back(LEntity(EntityID, this).GetUUID());
			Entities.push_back(LEntity(EntityID, this));
		}

		return Entities;
	}

	void LScene::DestroyEntity(const LEntity Entity)
	{
		EntityMap.erase(Entity.GetUUID());
		Registry.destroy(Entity);
		LK_CORE_DEBUG_TAG("Scene", "Destroyed entity: {}", Entity.Name());
	}

	void LScene::DestroyEntity(std::string_view EntityName)
	{
		auto View = Registry.view<LTagComponent>();
		for (auto Entity : View)
		{
			const LTagComponent& TagComp = View.get<LTagComponent>(Entity);
			LK_CORE_INFO("Entity: {} ({})  NAME EQUAL: {}", TagComp.Tag, Entity, TagComp.Tag == EntityName ? "YES" : "No");
			//if (strcmp(TagComp.Tag.c_str(), Name.c_str()) == 0)
			//if (std::string_view(TagComp.Tag) == Name)
			//if (TagComp.Tag == Name.c_str())
			if (TagComp.Tag == EntityName)
			{
				//return LEntity{ Entity , this };
				LEntity Temp{ Entity , this };
				EntityMap.erase(Temp.GetUUID());
				Registry.destroy(Entity);
				LK_CORE_DEBUG_TAG("Scene", "Destroyed entity: {}", EntityName);
				return;
			}
		}

		LK_CORE_WARN_TAG("Scene", "Failed to destroy: {}", EntityName);
		//Registry.destroy(Entity);
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
			auto LhsEntity = EntityMap.find(Lhs.ID);
			auto RhsEntity = EntityMap.find(Rhs.ID);
			return static_cast<uint32_t>(LhsEntity->second) < static_cast<uint32_t>(RhsEntity->second);
		});
	}

	void LScene::Pause(const bool IsPaused)
	{
		if (bPaused != IsPaused)
		{
			/* TODO: Notify change. */
		}
	#if 0
		World->Pause(IsPaused);
	#endif

		bPaused = IsPaused;
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
		//ParentChildren.erase(std::remove(ParentChildren.begin(), ParentChildren.end(), Entity.GetUUID()), ParentChildren.end());
		auto Iter = std::remove(ParentChildren.begin(), ParentChildren.end(), Entity.GetUUID());
		ParentChildren.erase(Iter, ParentChildren.end());

	#if 0
		if (bConvertToWorldSpace)
		{
			ConvertToWorldSpace(Entity);
		}
	#endif

		Entity.SetParentUUID(0);
	}

	glm::mat4 LScene::GetWorldSpaceTransform(LEntity Entity)
	{
		LK_PROFILE_FUNC();
		glm::mat4 Transform(1.0f);

		if (Entity.HasComponent<LRelationshipComponent>())
		{
			LEntity Parent = TryGetEntityWithUUID(Entity.GetParentUUID());
			if (Parent)
			{
				Transform = GetWorldSpaceTransform(Parent);
			}
		}

		return (Transform * Entity.Transform().GetTransform());
	}

	void LScene::CopyTo(TObjectPtr<LScene>& TargetScene)
	{
		LK_CORE_VERIFY(TargetScene);
		LK_CORE_DEBUG_TAG("Scene", "Copying scene: {}", Name);

		TargetScene->Name = Name;
		TargetScene->bEditorScene = bEditorScene;

		std::unordered_map<LUUID, entt::entity> EntityMap;
		auto View = Registry.view<LIDComponent>();
		for (auto Entity : View)
		{
			const LUUID EntityUUID = Registry.get<LIDComponent>(Entity).ID;
			const std::string& EntityName = Registry.get<LTagComponent>(Entity).Tag;

			/* Create identical entity on the target scene. */
			LEntity EntityCopy = TargetScene->CreateEntityWithID(EntityUUID, EntityName);
			EntityMap[EntityUUID] = EntityCopy.Handle;
		}

	#if 0
		auto TargetView = TargetScene->Registry.view<LIDComponent>();
		for (auto Entity : TargetView)
		{
			LUUID UUID = TargetView.get<LIDComponent>(Entity).ID;
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

		TargetScene->ViewportWidth = ViewportWidth;
		TargetScene->ViewportHeight = ViewportHeight;
	}

	void LScene::OnRenderRuntime(TObjectPtr<LSceneRenderer> SceneRenderer, const float DeltaTime)
	{
		LEntity CameraEntity = GetMainCameraEntity();
		if (!CameraEntity || !CameraEntity.HasAny<LCameraComponent>())
		{
			LK_CORE_WARN_TAG("Scene", "Entity is missing a camera component");
			return;
		}

		LCameraComponent& CameraComp = CameraEntity.GetComponent<LCameraComponent>();
		const LSceneCamera& SceneCamera = CameraComp.Camera;

		SceneRenderer->BeginScene(SceneCamera);
	}

	void LScene::OnRenderEditor(TObjectPtr<LSceneRenderer> SceneRenderer, LEditorCamera& EditorCamera, const float DeltaTime)
	{
		LK_CORE_ASSERT(SceneRenderer);
		LK_CORE_ASSERT((ViewportWidth > 0) && (ViewportHeight > 0), "Invalid viewport size");
		EditorCamera.SetViewportSize(ViewportWidth, ViewportHeight);

		SceneRenderer->SetScene(this);
		SceneRenderer->BeginScene(EditorCamera);

		/* Static Meshes. */
		{
			auto View = Registry.view<LStaticMeshComponent>();
			for (entt::entity EntityID : View)
			{
				LEntity Entity = LEntity(EntityID, this);
				auto& StaticMeshComp = View.get<LStaticMeshComponent>(EntityID);

				if (LAssetManager::IsAssetHandleValid(StaticMeshComp.StaticMesh))
				{
					TObjectPtr<LStaticMesh> StaticMesh = LAssetManager::GetAsset<LStaticMesh>(StaticMeshComp.StaticMesh);
					LK_CORE_ASSERT(StaticMesh, "StaticMesh {} is nullptr", StaticMeshComp.StaticMesh);
					LK_CORE_ASSERT(StaticMeshComp.MaterialTable, "MaterialTable is nullptr for LStaticMeshComponent: {}", StaticMeshComp.StaticMesh);

					const glm::mat4 Transform = GetWorldSpaceTransform(Entity);

					/**
					 * Temporarily assigning different materials based on the entity name until
					 * material serialization is working as intended.
					 */
				#if 1
					TObjectPtr<LMaterialAsset> MeshMaterial;
					if (Entity.Name().find("Cube") != std::string::npos)
					{
						MeshMaterial = LEditorAssetManager::Material_WoodContainer;
					}
					else if (Entity.Name().find("Ramp") != std::string::npos)
					{
						MeshMaterial = LEditorAssetManager::Material_Metal;
					}
					else if (Entity.Name().find("Plane") != std::string::npos)
					{
						MeshMaterial = LEditorAssetManager::Material_Metal;
					}
					else
					{
						MeshMaterial = LEditorAssetManager::Material_Metal;
					}

					SceneRenderer->SubmitStaticMesh(StaticMesh, StaticMeshComp.MaterialTable, Transform, MeshMaterial);
				#else
					SceneRenderer->SubmitStaticMesh(StaticMesh, StaticMeshComp.MaterialTable, Transform);
				#endif
				}
			}
		}

		/* Meshes. */
		{
			auto View = Registry.view<LMeshComponent>();
			for (entt::entity EntityID : View)
			{
				LEntity Entity = LEntity(EntityID, this);
				auto& MeshComp = View.get<LMeshComponent>(EntityID);

				if (LAssetManager::IsAssetHandleValid(MeshComp.Mesh))
				{
					LK_CORE_ASSERT(MeshComp.MaterialTable, "MaterialTable is nullptr for LMeshComponent: {}", MeshComp.Mesh);
					const glm::mat4 Transform = GetWorldSpaceTransform(Entity);

					TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(MeshComp.Mesh);
					LK_CORE_ASSERT(Mesh, "Mesh {} is nullptr", MeshComp.Mesh);
					SceneRenderer->SubmitMesh(Mesh, MeshComp.SubmeshIndex, MeshComp.MaterialTable, Transform);
				}
			}
		}

		SceneRenderer->EndScene();
	}

	void LScene::OnUpdateRuntime(const float InDeltaTime)
	{
		LK_PROFILE_FUNC();
	}

	void LScene::OnUpdateEditor(const float InDeltaTime)
	{
		LK_PROFILE_FUNC();
	}

	void LScene::EndScene()
	{
	}

	void LScene::SetActive(const bool Active)
	{
		if (Active && (this != ActiveScene.Get()))
		{
			LK_CORE_DEBUG("Setting active scene: {}", GetName());
			ActiveScene = this;
			LK_CORE_VERIFY(ActiveScene);

			GOnSceneSetActive.Broadcast(this);
		}
	}

	void LScene::Clear()
	{
		LK_CORE_TRACE_TAG("Scene", "Clearing: {}", Name);
		Registry.clear<AllComponents>();
	}

	LEntity LScene::GetMainCameraEntity()
	{
		auto EntityView = Registry.view<LCameraComponent>();
		for (auto Entity : EntityView)
		{
			LCameraComponent& CameraComponent = EntityView.get<LCameraComponent>(Entity);
			if (CameraComponent.bPrimary)
			{
				LK_CORE_VERIFY((CameraComponent.Camera.GetOrthographicSize() > 0) 
							   || (CameraComponent.Camera.GetPerspectiveVerticalFov() > 0), "Camera is not fully initialized");
				return { Entity, this };
			}
		}
		
		LK_CORE_FATAL_TAG("Scene", "GetMainCameraEntity failed");
		return {};
	}

	std::unordered_set<LUUID> LScene::GetAssetList()
	{
		std::unordered_set<LUUID> AssetList;
		std::unordered_set<LUUID> MissingAssets;

		// LMeshComponent
		{
			auto View = Registry.view<LMeshComponent>();
			for (auto Entity : View)
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
						TObjectPtr<LMesh> Mesh = LAssetManager::GetAsset<LMesh>(MeshComp.Mesh);
						if (!Mesh)
						{
							continue;
						}

						TObjectPtr<LMeshSource> MeshSource = Mesh->GetMeshSource();
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
			auto View = Registry.view<LStaticMeshComponent>();
			for (auto Entity : View)
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

	static void InsertMeshMaterials(TObjectPtr<LMeshSource> MeshSource, std::unordered_set<LUUID>& AssetList)
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
