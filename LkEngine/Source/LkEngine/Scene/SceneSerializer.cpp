#include "LKpch.h"
#include "SceneSerializer.h"

#include "LkEngine/Serialization/YAMLSerialization.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "SceneCamera.h"

#include "LkEngine/Renderer/Camera.h"

#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

	std::string LSceneSerializer::FILE_FILTER = std::format(
		"LkEngine Scene (*.{})\0*.{}\0", LScene::FILE_EXTENSION, LScene::FILE_EXTENSION
	);

	LSceneSerializer::LSceneSerializer(LScene* InScene)
		: Scene(InScene)
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(Scene);
	}

	LSceneSerializer::LSceneSerializer(const TObjectPtr<LScene>& InScene)
		: LSceneSerializer((LScene*)InScene.Get())
	{
	}

	void LSceneSerializer::Serialize(const std::filesystem::path& Filepath)
	{
		LK_CORE_DEBUG_TAG("SceneSerializer", "Serializing: {}", Filepath.string());
		YAML::Emitter Out;
		SerializeToYaml(Out);

		std::ofstream FileOut(Filepath);
		FileOut << Out.c_str();
	}

	void LSceneSerializer::SerializeToYaml(YAML::Emitter& Out)
	{
		Out << YAML::BeginMap;
		Out << YAML::Key << "Scene"       << YAML::Value << Scene->GetName();
		Out << YAML::Key << "Active"      << YAML::Value << Scene->IsActive();
		Out << YAML::Key << "EditorScene" << YAML::Value << Scene->bEditorScene;
		Out << YAML::Key << "SceneHandle" << YAML::Value << static_cast<uint32_t>(Scene->SceneEntity);

		LEditorLayer& Editor = LEditorLayer::Get();

		/* EditorCamera. */
		if (Editor.GetEditorCamera())
		{
			SerializeEditorCamera(Out, *Editor.GetEditorCamera());
		}

		/* Entities. */
		Out << YAML::Key << "Entities";
		Out << YAML::Value << YAML::BeginSeq;
		{
			/* Sort entities by UUID. */
			std::map<LUUID, entt::entity> SortedEntityMap;
			auto IDComponentView = Scene->Registry.view<LIDComponent>();
			for (const entt::entity Entity : IDComponentView)
			{
				SortedEntityMap[IDComponentView.get<LIDComponent>(Entity).ID] = Entity;
			}
	
			/* Serialize sorted entities. */
			for (const auto& [ID, Entity] : SortedEntityMap)
			{
				SerializeEntity(Out, { Entity, Scene->ActiveScene });
			}
		}
		Out << YAML::EndSeq;

		Out << YAML::EndMap;
	}

	bool LSceneSerializer::Deserialize(const std::filesystem::path& Filepath)
	{
		LK_CORE_VERIFY(Scene);
		LK_CORE_VERIFY(!Filepath.empty(), "Deserialization failed, invalid filepath: '{}'", Filepath.string());
		Scene->Clear();

		if (!LFileSystem::Exists(Filepath))
		{
			LK_CORE_WARN_TAG("SceneSerializer", "File does not exist: {}", Filepath.string());
			return false;
		}

		LK_CORE_DEBUG_TAG("SceneSerializer", "Deserializing: {}", Filepath.filename().string());
		bool bOperationSuccess = false;

		std::ifstream InputStream(Filepath);
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();
		try
		{
			bOperationSuccess = DeserializeFromYaml(StringStream.str());
		}
		catch (const YAML::Exception& Exception)
		{
			LK_CORE_ASSERT(false, "Failed to deserialize '{}', error: {}", Filepath.string(), Exception.what());
			bOperationSuccess = false;
		}

		return bOperationSuccess;
	}

	bool LSceneSerializer::DeserializeRuntime(const LUUID InSceneHandle)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		return true;
	}

	TObjectPtr<LScene> LSceneSerializer::LoadScene()
	{
		if (Scene)
		{
			Scene->ViewportWidth = LWindow::Get().GetViewportWidth();
			Scene->ViewportHeight = LWindow::Get().GetViewportHeight();
		}
		else
		{
			LK_CORE_ERROR_TAG("SceneSerializer", "LoadScene failed, no scene available");
		}

		return Scene;
	}

	bool LSceneSerializer::DeserializeFromYaml(const std::string& YamlString)
	{
		LK_CORE_VERIFY(Scene);
		if (YamlString.empty())
		{
			LK_CORE_ERROR_TAG("SceneSerializer", "Cannot deserialize scene, yaml string is empty");
			return false;
		}

		YAML::Node Data = YAML::Load(YamlString);
		if (!Data["Scene"])
		{
			LK_CORE_ERROR_TAG("SceneSerializer", "Yaml node 'Scene' does not exist, yaml data:\n{}", YamlString);
			return false;
		}

		/* Scene. */
		std::string SceneName = Data["Scene"].as<std::string>(); 
		Scene->SetName(SceneName);

		const bool bIsActiveScene = (Data["Active"].as<std::string>() == "true" ? true : false);
		Scene->SetActive(bIsActiveScene);

		const bool bIsEditorScene = Data["EditorScene"].as<std::string>() == "true" ? true : false;
		Scene->bEditorScene = bIsEditorScene;
		const uint32_t SceneEntityHandle = Data["SceneHandle"].as<uint32_t>();

		Scene->ViewportWidth = LWindow::Get().GetViewportWidth();
		Scene->ViewportHeight = LWindow::Get().GetViewportHeight();

		/* EditorCamera. */
		const YAML::Node& EditorCameraNode = Data["EditorCamera"];
		if (EditorCameraNode)
		{
			TObjectPtr<LEditorCamera> EditorCamera = LEditorLayer::Get().GetEditorCamera();
			DeserializeEditorCamera(EditorCameraNode, *EditorCamera);
		}

		/* Scene entities. */
		YAML::Node EntitiesNode = Data["Entities"];
		if (EntitiesNode)
		{
			DeserializeEntities(EntitiesNode, Scene);
		}

		/* Sort the LIDComponent by the entity handle. */
		Scene->Registry.sort<LIDComponent>([this](const auto Lhs, const auto Rhs)
		{
			auto LhsEntity = Scene->EntityMap.find(Lhs.ID);
			auto RhsEntity = Scene->EntityMap.find(Rhs.ID);
			return (static_cast<uint32_t>(LhsEntity->second) < static_cast<uint32_t>(RhsEntity->second));
		});

		return true;
	}

	void LSceneSerializer::SerializeRuntime(const LUUID InSceneHandle)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		LK_UNUSED(InSceneHandle);
	}

	void LSceneSerializer::SerializeEntity(YAML::Emitter& Out, LEntity Entity)
	{
		/* TODO: Validate that no other entry exists already. */
		if (!Entity.HasComponent<LTagComponent>() && !Entity.HasComponent<LTransformComponent>())
		{
			LK_CORE_INFO("Skipping serialization of entity: {}", Entity);
			return;
		}

		/* REMOVE ME */
		if (Entity.GetComponent<LTagComponent>().Tag == "DebugCube")
		{
			LK_CORE_WARN("Skipping serialization of entity: DebugCube");
			return;
		}

		const LUUID ID = Entity.GetUUID();
		Out << YAML::BeginMap; /* Entity */
		Out << YAML::Key << "Entity";
		Out << YAML::Value << ID;
		LK_CORE_TRACE_TAG("SceneSerializer", "Serializing entity: {}", ID);

		/* TagComponent */
		const LTagComponent& TagComp = Entity.GetComponent<LTagComponent>().Tag;
		{
			Out << YAML::Key << "TagComponent";
			Out << YAML::BeginMap;
			Out << YAML::Key << "Tag" << YAML::Value << TagComp.Tag;
			Out << YAML::EndMap; /* TagComponent */
		}

		if (Entity.HasComponent<LTransformComponent>())
		{
			Out << YAML::Key << "TransformComponent";
			Out << YAML::BeginMap;
			const LTransformComponent& Transform = Entity.GetComponent<LTransformComponent>();
			Out << YAML::Key << "Position" << YAML::Value << Transform.Translation;
			Out << YAML::Key << "Rotation" << YAML::Value << Transform.GetRotationEuler();
			Out << YAML::Key << "Scale" << YAML::Value << Transform.Scale;
			Out << YAML::EndMap; /* TransformComponent */
		}

		if (Entity.HasComponent<LSpriteComponent>())
		{
			Out << YAML::Key << "SpriteComponent";
			Out << YAML::BeginMap; 

			const LSpriteComponent& Sprite = Entity.GetComponent<LSpriteComponent>();
			Out << YAML::Key << "Size" << YAML::Value << Sprite.GetSize();
			Out << YAML::Key << "Color" << YAML::Value << Sprite.GetColor();

			Out << YAML::EndMap; 
		}

		/* Mesh. */
		if (Entity.HasComponent<LMeshComponent>())
		{
			Out << YAML::Key << "MeshComponent";
			Out << YAML::BeginMap; /* MeshComponent */

			const auto& MeshComp = Entity.GetComponent<LMeshComponent>();
			Out << YAML::Key << "AssetID" << YAML::Value << MeshComp.Mesh;

			TObjectPtr<LMaterialTable> MaterialTable = MeshComp.MaterialTable;
			LK_CORE_ASSERT(MaterialTable, "Serialization of a LMeshComponent failed, the MaterialTable is not valid");
			if (MaterialTable->GetMaterialCount() > 0)
			{
				Out << YAML::Key << "MaterialTable" << YAML::Value << YAML::BeginMap; /* MaterialTable */
				for (uint32_t Index = 0; Index < MaterialTable->GetMaterialCount(); Index++)
				{
					const FAssetHandle Handle = (MaterialTable->HasMaterial(Index) ? MaterialTable->GetMaterial(Index) : (FAssetHandle)0);
					Out << YAML::Key << Index << YAML::Value << Handle;
				}
				Out << YAML::EndMap; /* MaterialTable */
			}
			else
			{
				LK_CORE_TRACE_TAG("SceneSerializer", "Skipping serialization of material table for '{}' (Mesh), the table is empty", MeshComp.Mesh);
			}

			Out << YAML::Key << "Visible" << YAML::Value << MeshComp.bVisible;
			Out << YAML::EndMap; /* MeshComponent */
		}

		/* Static Mesh. */
		if (Entity.HasComponent<LStaticMeshComponent>())
		{
			Out << YAML::Key << "StaticMeshComponent";
			Out << YAML::BeginMap; /* MeshComponent */

			const LStaticMeshComponent& StaticMeshComp = Entity.GetComponent<LStaticMeshComponent>();
			Out << YAML::Key << "AssetID" << YAML::Value << StaticMeshComp.StaticMesh;

			TObjectPtr<LMaterialTable> MaterialTable = StaticMeshComp.MaterialTable;
			LK_CORE_ASSERT(MaterialTable, "Serialization of a LStaticMeshComponent failed, the MaterialTable is not valid");
			if (MaterialTable->GetMaterialCount() > 0)
			{
				Out << YAML::Key << "MaterialTable" << YAML::Value << YAML::BeginMap; /* MaterialTable */
				for (uint32_t Index = 0; Index < MaterialTable->GetMaterialCount(); Index++)
				{
					const FAssetHandle Handle = (MaterialTable->HasMaterial(Index) ? MaterialTable->GetMaterial(Index) : (FAssetHandle)0);
					Out << YAML::Key << Index << YAML::Value << Handle;
				}
				Out << YAML::EndMap; /* MaterialTable */
			}
			else
			{
				LK_CORE_TRACE_TAG("SceneSerializer", "Skipping serialization of materials for '{}' ({}), the table is empty", Entity.Name(), StaticMeshComp.StaticMesh);
			}

			Out << YAML::Key << "Visible" << YAML::Value << StaticMeshComp.bVisible;
			Out << YAML::EndMap; /* MeshComponent */
		}


		if (Entity.HasComponent<LCameraComponent>())
		{
			Out << YAML::Key << "CameraComponent";
			Out << YAML::BeginMap; /* LCameraComponent */

			const LCameraComponent& CameraComponent = Entity.GetComponent<LCameraComponent>();
			const LCamera& Camera = *CameraComponent.Camera;

			Out << YAML::Key << "Camera" << YAML::Value;
			Out << YAML::BeginMap; 
			{
				Out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(Camera.GetProjectionType());
				Out << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
				Out << YAML::Key << "OrthographicFar" << YAML::Value << Camera.GetOrthographicFarClip();
			}
			Out << YAML::EndMap;
			Out << YAML::EndMap; /* LCameraComponent */
		}

		if (Entity.HasComponent<LRigidBody2DComponent>())
		{
			const LRigidBody2DComponent& Rigidbody2DComponent = Entity.GetComponent<LRigidBody2DComponent>();
			Out << YAML::Key << "RigidBody2DComponent";
			Out << YAML::BeginMap;
			Out << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(Rigidbody2DComponent.BodyType);
			Out << YAML::Key << "FixedRotation" << YAML::Value << Rigidbody2DComponent.FixedRotation;
			Out << YAML::EndMap;
		}

		if (Entity.HasComponent<LBoxCollider2DComponent>())
		{
			const LBoxCollider2DComponent& BoxCollider2DComponent = Entity.GetComponent<LBoxCollider2DComponent>();
			Out << YAML::Key << "BoxCollider2DComponent";
			Out << YAML::BeginMap;
			Out << YAML::Key << "Offset" << YAML::Value << BoxCollider2DComponent.Offset;
			Out << YAML::Key << "Size" << YAML::Value << BoxCollider2DComponent.Size;
			Out << YAML::Key << "Density" << YAML::Value << BoxCollider2DComponent.Density;
			Out << YAML::Key << "Friction" << YAML::Value << BoxCollider2DComponent.Friction;
			Out << YAML::EndMap; 
		}

		Out << YAML::EndMap; /* Entity */
	}

	void LSceneSerializer::DeserializeEntities(YAML::Node& EntitiesNode, TObjectPtr<LScene> SceneRef)
	{
		LK_VERIFY(SceneRef, "Deserialization of entities failed, scene reference is not valid");
		for (const YAML::Node& EntityNode : EntitiesNode)
		{
			const uint64_t EntityID = EntityNode["Entity"].as<uint64_t>();

			std::string EntityName{};
			const YAML::Node& TagComponentNode = EntityNode["TagComponent"];
			if (TagComponentNode)
			{
				EntityName = TagComponentNode["Tag"].as<std::string>();
			}

			LK_CORE_TRACE_TAG("SceneSerializer", "Creating entity: {} ({})", EntityName, EntityID);
			LEntity DeserializedEntity = SceneRef->CreateEntityWithID(EntityID, EntityName);

			const YAML::Node& TransformComponentNode = EntityNode["TransformComponent"];
			if (TransformComponentNode)
			{
				/* Entities always have transforms. */
				LTransformComponent& TransformComponent = DeserializedEntity.AddComponent<LTransformComponent>();
				TransformComponent.Translation = TransformComponentNode["Position"].as<glm::vec3>(glm::vec3(0.0f));

				const YAML::Node& RotationNode = TransformComponentNode["Rotation"];
				if (RotationNode.size() == 4)
				{
					TransformComponent.SetRotation(TransformComponentNode["Rotation"].as<glm::quat>(glm::quat()));
				}
				else
				{
					TransformComponent.SetRotationEuler(TransformComponentNode["Rotation"].as<glm::vec3>(glm::vec3(0.0f)));
				}

				TransformComponent.Scale = TransformComponentNode["Scale"].as<glm::vec3>();
			}

			const YAML::Node& SpriteComponentNode = EntityNode["SpriteComponent"];
			if (SpriteComponentNode)
			{
				auto& SpriteComponent = DeserializedEntity.AddComponent<LSpriteComponent>();
				SpriteComponent.Size = SpriteComponentNode["Size"].as<glm::vec2>(glm::vec2(0.0f));
				SpriteComponent.Color = SpriteComponentNode["Color"].as<glm::vec4>(glm::vec4(0.0f));
			}

			/* Static Mesh. */
			{
				const YAML::Node& StaticMeshCompNode = EntityNode["StaticMeshComponent"];
				if (StaticMeshCompNode)
				{
					static_assert(std::is_same_v<FAssetHandle::SizeType, uint64_t>);
					LStaticMeshComponent& MeshComp = DeserializedEntity.AddComponent<LStaticMeshComponent>();
					MeshComp.StaticMesh = StaticMeshCompNode["AssetID"].as<uint64_t>();

					if (StaticMeshCompNode["MaterialTable"])
					{
						YAML::Node MaterialTableNode = StaticMeshCompNode["MaterialTable"];
						for (auto MaterialEntry : MaterialTableNode)
						{
							const uint32_t Index = MaterialEntry.first.as<uint32_t>();
							const FAssetHandle MatAsset = MaterialEntry.second.as<FAssetHandle>();
							MeshComp.MaterialTable->SetMaterial(Index, MatAsset);
						}
					}

					if (StaticMeshCompNode["Visible"])
					{
						MeshComp.bVisible = StaticMeshCompNode["Visible"].as<bool>();
					}
					LK_CORE_TRACE_TAG("SceneSerializer", "Deserialized StaticMeshComponent: {}", MeshComp.StaticMesh);
				}
			}

			/* Mesh. */
			{
				const YAML::Node& MeshCompNode = EntityNode["MeshComponent"];
				if (MeshCompNode)
				{
					static_assert(std::is_same_v<FAssetHandle::SizeType, uint64_t>);
					LMeshComponent& MeshComp = DeserializedEntity.AddComponent<LMeshComponent>();
					MeshComp.Mesh = MeshCompNode["AssetID"].as<FAssetHandle>();

					if (MeshCompNode["MaterialTable"])
					{
						YAML::Node MaterialTableNode = MeshCompNode["MaterialTable"];
						for (auto MaterialEntry : MaterialTableNode)
						{
							const uint32_t Index = MaterialEntry.first.as<uint32_t>();
							const FAssetHandle Handle = MaterialEntry.second.as<FAssetHandle>();

							//if (Handle > 0)
							if (LAssetManager::IsAssetHandleValid(Handle))
							{
								MeshComp.MaterialTable->SetMaterial(Index, Handle);
							}
							else
							{
								LK_CORE_ERROR_TAG("SceneSerializer", "Could not set material '{}' in table with index {} (Mesh)", Handle, Index);
							}
						}
					}

					if (MeshCompNode["Visible"])
					{
						MeshComp.bVisible = MeshCompNode["Visible"].as<bool>();
					}
				}
			}

			const YAML::Node& CameraComponentNode = EntityNode["CameraComponent"];
			if (CameraComponentNode)
			{
				LCameraComponent& CameraComponent = DeserializedEntity.AddComponent<LCameraComponent>();
				CameraComponent.Camera = TObjectPtr<LSceneCamera>::Create();

				const YAML::Node& CameraNode = CameraComponentNode["Camera"];
				if (CameraNode.IsMap())
				{
					if (CameraNode["ProjectionType"])
					{
						CameraComponent.Camera->SetProjectionType(static_cast<ECameraProjection>(CameraNode["ProjectionType"].as<int>()));
					}

					if (CameraNode["OrthographicNear"])
					{
						CameraComponent.Camera->SetOrthographicNearClip(CameraNode["OrthographicNear"].as<float>());
					}

					if (CameraNode["OrthographicFar"])
					{
						CameraComponent.Camera->SetOrthographicFarClip(CameraNode["OrthographicFar"].as<float>());
					}
				}
			}

			const YAML::Node& RigidBody2DNode = EntityNode["LRigidBody2DComponent"];
			if (RigidBody2DNode)
			{
				LRigidBody2DComponent& RigidBodyComponent = DeserializedEntity.AddComponent<LRigidBody2DComponent>();
				RigidBodyComponent.BodyType = static_cast<LRigidBody2DComponent::Type>(RigidBody2DNode["BodyType"].as<int>());
				RigidBodyComponent.FixedRotation = RigidBody2DNode["FixedRotation"] ? RigidBody2DNode["FixedRotation"].as<bool>() : false;
			}

			const YAML::Node& BoxColliderNode = EntityNode["BoxCollider2DComponent"];
			if (BoxColliderNode)
			{
				LBoxCollider2DComponent& BoxColliderComponent = DeserializedEntity.AddComponent<LBoxCollider2DComponent>();
				BoxColliderComponent.Offset = BoxColliderNode["Offset"].as<glm::vec2>();
				BoxColliderComponent.Size = BoxColliderNode["Size"].as<glm::vec2>();
				BoxColliderComponent.Density = BoxColliderNode["Density"] ? BoxColliderNode["Density"].as<float>() : 1.0f;
				BoxColliderComponent.Friction = BoxColliderNode["Friction"] ? BoxColliderNode["Friction"].as<float>() : 1.0f;
			}

			SceneRef->SortEntities();
		}
	}

	void LSceneSerializer::SerializeEditorCamera(YAML::Emitter& Out, LEditorCamera& EditorCamera)
	{
		Out << YAML::Key << "EditorCamera" << YAML::Value;
		Out << YAML::BeginMap;
		{
			Out << YAML::Key << "Position"   << YAML::Value << EditorCamera.GetPosition();
			Out << YAML::Key << "Origin"     << YAML::Value << EditorCamera.GetOrigin();
			Out << YAML::Key << "Distance"   << YAML::Value << EditorCamera.GetDistance();
			Out << YAML::Key << "FocalPoint" << YAML::Value << EditorCamera.GetFocalPoint();

			/* Perspective */
			Out << YAML::Key << "Perspective" << YAML::Value;
			Out << YAML::Value << YAML::BeginMap;
			{
				Out << YAML::Key << "DegPerspectiveFOV" << YAML::Value << EditorCamera.GetPerspectiveDegFov();
				Out << YAML::Key << "PerspectiveNear"   << YAML::Value << EditorCamera.GetPerspectiveNearClip();
				Out << YAML::Key << "PerspectiveFar"    << YAML::Value << EditorCamera.GetPerspectiveFarClip();
			}
			Out << YAML::Value << YAML::EndMap;
			/* ~Perspective */

			/* Orthographic */
			Out << YAML::Key << "Orthographic" << YAML::Value;
			Out << YAML::Value << YAML::BeginMap;
			{
				Out << YAML::Key << "OrthographicSize" << YAML::Value << EditorCamera.GetOrthographicSize();
				Out << YAML::Key << "OrthographicNear" << YAML::Value << EditorCamera.GetOrthographicNearClip();
				Out << YAML::Key << "OrthographicFar"  << YAML::Value << EditorCamera.GetOrthographicFarClip();
			}
			Out << YAML::Value << YAML::EndMap;
			/* ~Orthographic */

			Out << YAML::Key << "Pitch"          << YAML::Value << EditorCamera.GetPitch();
			Out << YAML::Key << "Yaw"            << YAML::Value << EditorCamera.GetYaw();
			Out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(EditorCamera.GetProjectionType());
			Out << YAML::Key << "CameraMode"     << YAML::Value << static_cast<int>(EditorCamera.CameraMode);
		}
		Out << YAML::EndMap;
	}

	void LSceneSerializer::DeserializeEditorCamera(const YAML::Node& EditorCameraNode, LEditorCamera& EditorCamera)
	{
		if (EditorCameraNode)
		{
			/* Position, origin, distance and focalpoint. */
			EditorCamera.Position = EditorCameraNode["Position"].as<glm::vec3>(glm::vec3(0.0f));
			EditorCamera.Origin = EditorCameraNode["Origin"].as<glm::vec3>(glm::vec3(0.0f));
			EditorCamera.Distance = EditorCameraNode["Distance"].as<float>();
			EditorCamera.FocalPoint = EditorCameraNode["FocalPoint"].as<glm::vec3>(glm::vec3(0.0f));

			/* Perspective data. */
			const YAML::Node& PerspectiveCameraNode = EditorCameraNode["Perspective"];
			if (PerspectiveCameraNode.IsMap())
			{
				EditorCamera.DegPerspectiveFOV = PerspectiveCameraNode["DegPerspectiveFOV"].as<float>();
				EditorCamera.PerspectiveNear = PerspectiveCameraNode["PerspectiveNear"].as<float>();
				EditorCamera.PerspectiveFar = PerspectiveCameraNode["PerspectiveFar"].as<float>();

				EditorCamera.SetPerspectiveNearClip(EditorCamera.PerspectiveNear);
				EditorCamera.SetPerspectiveFarClip(EditorCamera.PerspectiveFar);

				EditorCamera.ViewportWidth = Scene->ViewportWidth;
				EditorCamera.ViewportHeight = Scene->ViewportHeight;

				EditorCamera.SetPerspectiveProjectionMatrix(
					glm::radians(EditorCamera.DegPerspectiveFOV), 
					EditorCamera.ViewportWidth,
					EditorCamera.ViewportHeight,
					EditorCamera.PerspectiveNear,
					EditorCamera.PerspectiveFar
				);

				EditorCamera.SetPerspective(EditorCamera.DegPerspectiveFOV, 
											EditorCamera.PerspectiveNear, 
											EditorCamera.PerspectiveFar);
			}

			/* Orthographic data. */
			const YAML::Node& OrthographicCameraNode = EditorCameraNode["Orthographic"];
			if (OrthographicCameraNode.IsMap())
			{
				EditorCamera.OrthographicSize = OrthographicCameraNode["OrthographicSize"].as<float>();
				EditorCamera.OrthographicNear = OrthographicCameraNode["OrthographicNear"].as<float>();
				EditorCamera.OrthographicFar  = OrthographicCameraNode["OrthographicFar"].as<float>();

				EditorCamera.SetOrthographicNearClip(EditorCamera.OrthographicNear);
				EditorCamera.SetOrthographicFarClip(EditorCamera.OrthographicFar);
			}

			/* Pitch and Yaw. */
			EditorCamera.Pitch = EditorCameraNode["Pitch"].as<float>();
			EditorCamera.Yaw = EditorCameraNode["Yaw"].as<float>();

			/* Projection Type. */
			const int ProjectionType = EditorCameraNode["ProjectionType"].as<int>();
			if (ProjectionType == static_cast<int>(ECameraProjection::Perspective))
			{
				EditorCamera.ProjectionType = ECameraProjection::Perspective;
			}
			else if (ProjectionType == static_cast<int>(ECameraProjection::Orthographic))
			{
				EditorCamera.ProjectionType = ECameraProjection::Orthographic;
			}

			/* Editor Camera Mode. */
			const int CameraMode = EditorCameraNode["CameraMode"].as<int>();
			if (CameraMode == static_cast<int>(EEditorCameraMode::Flycam))
			{
				EditorCamera.CameraMode = EEditorCameraMode::Flycam;
			}
			else if (CameraMode == static_cast<int>(EEditorCameraMode::Arcball))
			{
				EditorCamera.CameraMode = EEditorCameraMode::Arcball;
			}
			else
			{
				EditorCamera.CameraMode = EEditorCameraMode::None;
			}
		}
	}

}
