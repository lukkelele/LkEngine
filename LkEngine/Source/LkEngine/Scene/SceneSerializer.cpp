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

	std::string LSceneSerializer::FILE_FILTER = LK_FORMAT_STRING("LkEngine Scene (*.{})\0*.{}\0", 
																 LScene::FILE_EXTENSION, LScene::FILE_EXTENSION);

	LSceneSerializer::LSceneSerializer(LScene* InScene)
		: Scene(InScene)
	{
		LCLASS_REGISTER();
		LK_CORE_VERIFY(Scene, "Invalid LScene reference");
	}

	LSceneSerializer::LSceneSerializer(const TObjectPtr<LScene>& InScene)
		: LSceneSerializer((LScene*)InScene.Get())
	{
	}

	void LSceneSerializer::SerializeEntity(YAML::Emitter& Out, LEntity Entity)
	{
		UUID uuid = Entity.GetComponent<LIDComponent>().ID;
		Out << YAML::BeginMap; // Entity
		Out << YAML::Key << "Entity";
		Out << YAML::Value << uuid;

		if (Entity.HasComponent<LTagComponent>())
		{
			Out << YAML::Key << "TagComponent";
			Out << YAML::BeginMap; // TagComponent

			const LTagComponent& Tag = Entity.GetComponent<LTagComponent>().Tag;
			Out << YAML::Key << "Tag" << YAML::Value << Tag;

			Out << YAML::EndMap; // TagComponent
		}

		if (Entity.HasComponent<LTransformComponent>())
		{
			Out << YAML::Key << "TransformComponent";
			Out << YAML::BeginMap; // TransformComponent

			const LTransformComponent& Transform = Entity.GetComponent<LTransformComponent>();
			Out << YAML::Key << "Position" << YAML::Value << Transform.Translation;
			Out << YAML::Key << "Rotation" << YAML::Value << Transform.GetRotationEuler();
			Out << YAML::Key << "Scale" << YAML::Value << Transform.Scale;

			Out << YAML::EndMap; // TransformComponent
		}

		if (Entity.HasComponent<LSpriteComponent>())
		{
			Out << YAML::Key << "SpriteComponent";
			Out << YAML::BeginMap; // SpriteComponent

			const LSpriteComponent& Sprite = Entity.GetComponent<LSpriteComponent>();
			Out << YAML::Key << "Size" << YAML::Value << Sprite.GetSize();
			Out << YAML::Key << "Color" << YAML::Value << Sprite.GetColor();

			Out << YAML::EndMap; // SpriteComponent
		}

		//if (Entity.HasComponent<MaterialComponent>())
		//{
		//	Out << YAML::Key << "MaterialComponent";
		//	Out << YAML::BeginMap; // MaterialComponent
		//	auto& material = Entity.GetComponent<MaterialComponent>();
		//	Out << YAML::Key << "TextureName" << YAML::Value << material.GetTexture()->GetName();
		//	Out << YAML::EndMap; // SpriteComponent
		//}

		if (Entity.HasComponent<LCameraComponent>())
		{
			Out << YAML::Key << "CameraComponent";
			Out << YAML::BeginMap; // LCameraComponent

			const LCameraComponent& CameraComponent = Entity.GetComponent<LCameraComponent>();
			const LCamera& Camera = *CameraComponent.Camera;

			Out << YAML::Key << "Camera" << YAML::Value;
			Out << YAML::BeginMap; // Camera
			Out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(Camera.GetProjectionType());
			Out << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
			Out << YAML::Key << "OrthographicFar" << YAML::Value << Camera.GetOrthographicFarClip();
			Out << YAML::EndMap; // Camera

			Out << YAML::EndMap; // LCameraComponent
		}

		if (Entity.HasComponent<LRigidBody2DComponent>())
		{
			Out << YAML::Key << "RigidBody2DComponent";
			Out << YAML::BeginMap; // LRigidBody2DComponent

			const LRigidBody2DComponent& Rigidbody2DComponent = Entity.GetComponent<LRigidBody2DComponent>();
			Out << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(Rigidbody2DComponent.BodyType);
			Out << YAML::Key << "FixedRotation" << YAML::Value << Rigidbody2DComponent.FixedRotation;

			Out << YAML::EndMap; // LRigidBody2DComponent
		}

		if (Entity.HasComponent<LBoxCollider2DComponent>())
		{
			Out << YAML::Key << "BoxCollider2DComponent";
			Out << YAML::BeginMap; // BoxCollider2DComponent

			const LBoxCollider2DComponent& BoxCollider2DComponent = Entity.GetComponent<LBoxCollider2DComponent>();
			Out << YAML::Key << "Offset" << YAML::Value << BoxCollider2DComponent.Offset;
			Out << YAML::Key << "Size" << YAML::Value << BoxCollider2DComponent.Size;
			Out << YAML::Key << "Density" << YAML::Value << BoxCollider2DComponent.Density;
			Out << YAML::Key << "Friction" << YAML::Value << BoxCollider2DComponent.Friction;

			Out << YAML::EndMap; // BoxCollider2DComponent
		}

		Out << YAML::EndMap; // Entity
	}

	void LSceneSerializer::Serialize(const std::filesystem::path& Filepath)
	{
		YAML::Emitter Out;
		SerializeToYAML(Out);

		std::ofstream FileOut(Filepath);
		FileOut << Out.c_str();
	}

	void LSceneSerializer::SerializeToYAML(YAML::Emitter& Out)
	{
		Out << YAML::BeginMap;
		Out << YAML::Key << "Scene"       << YAML::Value << Scene->GetName();
		Out << YAML::Key << "Active"      << YAML::Value << Scene->IsActiveScene();
		Out << YAML::Key << "EditorScene" << YAML::Value << Scene->bIsEditorScene;
		Out << YAML::Key << "SceneHandle" << YAML::Value << static_cast<uint32_t>(Scene->SceneEntity);

		LEditorLayer* Editor = LEditorLayer::Get();

		/* EditorCamera. */
		if (Editor && Editor->GetEditorCamera())
		{
			SerializeEditorCamera(Out, *Editor->GetEditorCamera());
		}

		#if 0 /// DISABLED
		// 2D Physics
		Out << YAML::Key << "Physics2D";
		Out << YAML::Value << YAML::BeginMap;
		{
			Box2DWorldComponent& World2DComponent = Scene->GetBox2DWorld();
			if (&World2DComponent != nullptr)
			{
				Out << YAML::Key << "Paused"		<< YAML::Value << World2DComponent.IsPaused();
				Out << YAML::Key << "Gravity"		<< YAML::Value << World2DComponent.GetGravity();
				Out << YAML::Key << "DebugDrawer"	<< YAML::Value << World2DComponent.HasDebugDrawerAttached();
				Out << YAML::Key << "BodyCount"		<< YAML::Value << World2DComponent.GetBodyCount();
				Out << YAML::Key << "ContactCount"	<< YAML::Value << World2DComponent.GetContactCount();
			}
			else
			{
			}
		}
		Out << YAML::Value << YAML::EndMap;
		#endif

		/* Entities. */
		Out << YAML::Key << "Entities";
		Out << YAML::Value << YAML::BeginSeq;
		{
			/* Sort entities by UUID. */
			std::map<UUID, entt::entity> SortedEntityMap;
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

	/// FIXME: Refactor
	bool LSceneSerializer::DeserializeFromYAML(const std::string& YamlString)
	{
		YAML::Node data = YAML::Load(YamlString);
		if (!data["Scene"])
		{
			return false;
		}

		/* Scene. */
		std::string SceneName = data["Scene"].as<std::string>(); 
		Scene->SetName(SceneName);

		const bool bIsActiveScene = (data["Active"].as<std::string>() == "true" ? true : false);
		Scene->SetActive(bIsActiveScene);

		const bool bIsEditorScene = data["EditorScene"].as<std::string>() == "true" ? true : false;
		Scene->bIsEditorScene = bIsEditorScene;
		const uint32_t SceneEntityHandle = data["SceneHandle"].as<uint32_t>();

		/* EditorCamera. */
		const YAML::Node& EditorCameraNode = data["EditorCamera"];
		if (EditorCameraNode)
		{
			TObjectPtr<LEditorCamera> EditorCamera = LEditorLayer::Get()->GetEditorCamera();
			DeserializeEditorCamera(EditorCameraNode, *EditorCamera);
		}

		/* Scene entities. */
		YAML::Node EntitiesNode = data["Entities"];
		if (EntitiesNode)
		{
			DeserializeEntities(EntitiesNode, Scene);
		}

		/* Sort LIDComponent by by Entity handle(which is essentially the order in which they were created). */
		Scene->Registry.sort<LIDComponent>([this](const auto Lhs, const auto Rhs)
		{
			auto LhsEntity = Scene->m_EntityIDMap.find(Lhs.ID);
			auto RhsEntity = Scene->m_EntityIDMap.find(Rhs.ID);
			return (static_cast<uint32_t>(LhsEntity->second) < static_cast<uint32_t>(RhsEntity->second));
		});

		return true;
	}

	void LSceneSerializer::SerializeRuntime(const FAssetHandle InSceneHandle)
	{
		LK_UNUSED(InSceneHandle);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LSceneSerializer::DeserializeEntities(YAML::Node& EntitiesNode, TObjectPtr<LScene> SceneRef)
	{
		LK_VERIFY(SceneRef && SceneRef->IsValid());
		for (const YAML::Node& Entity : EntitiesNode)
		{
			uint64_t uuid = Entity["Entity"].as<uint64_t>();

			std::string EntityName{};
			const YAML::Node& TagComponentNode = Entity["TagComponent"];
			if (TagComponentNode)
			{
				EntityName = TagComponentNode["Tag"].as<std::string>();
			}

			LEntity DeserializedEntity = SceneRef->CreateEntityWithID(uuid, EntityName);

			const YAML::Node& TransformComponentNode = Entity["TransformComponent"];
			if (TransformComponentNode)
			{
				/* Entities always have Transforms. */
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

			const YAML::Node& SpriteComponentNode = Entity["SpriteComponent"];
			if (SpriteComponentNode)
			{
				LSpriteComponent& SpriteComponent = DeserializedEntity.AddComponent<LSpriteComponent>();
				SpriteComponent.Size = SpriteComponentNode["Size"].as<glm::vec2>(glm::vec2(0.0f));
				SpriteComponent.Color = SpriteComponentNode["Color"].as<glm::vec4>(glm::vec4(0.0f));
			}

			const YAML::Node& CameraComponentNode = Entity["CameraComponent"];
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

			const YAML::Node& RigidBody2DNode = Entity["LRigidBody2DComponent"];
			if (RigidBody2DNode)
			{
				LRigidBody2DComponent& RigidBodyComponent = DeserializedEntity.AddComponent<LRigidBody2DComponent>();
				RigidBodyComponent.BodyType = static_cast<LRigidBody2DComponent::Type>(RigidBody2DNode["BodyType"].as<int>());

				RigidBodyComponent.FixedRotation = RigidBody2DNode["FixedRotation"] 
					? RigidBody2DNode["FixedRotation"].as<bool>() 
					: false;
			}

			const YAML::Node& BoxColliderNode = Entity["BoxCollider2DComponent"];
			if (BoxColliderNode)
			{
				LBoxCollider2DComponent& BoxColliderComponent = DeserializedEntity.AddComponent<LBoxCollider2DComponent>();
				BoxColliderComponent.Offset = BoxColliderNode["Offset"].as<glm::vec2>();
				BoxColliderComponent.Size = BoxColliderNode["Size"].as<glm::vec2>();

				BoxColliderComponent.Density = BoxColliderNode["Density"] 
					? BoxColliderNode["Density"].as<float>() 
					: 1.0f;

				BoxColliderComponent.Friction = BoxColliderNode["Friction"] 
					? BoxColliderNode["Friction"].as<float>() 
					: 1.0f;
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

			// Perspective 
			Out << YAML::Key << "Perspective" << YAML::Value;
			Out << YAML::Value << YAML::BeginMap;
			{
				Out << YAML::Key << "DegPerspectiveFOV" << YAML::Value << EditorCamera.GetPerspectiveDegFov();
				Out << YAML::Key << "PerspectiveNear"   << YAML::Value << EditorCamera.GetPerspectiveNearClip();
				Out << YAML::Key << "PerspectiveFar"    << YAML::Value << EditorCamera.GetPerspectiveFarClip();
			}
			Out << YAML::Value << YAML::EndMap;
			// ~Perspective

			// Orthographic
			Out << YAML::Key << "Orthographic" << YAML::Value;
			Out << YAML::Value << YAML::BeginMap;
			{
				Out << YAML::Key << "OrthographicSize" << YAML::Value << EditorCamera.GetOrthographicSize();
				Out << YAML::Key << "OrthographicNear" << YAML::Value << EditorCamera.GetOrthographicNearClip();
				Out << YAML::Key << "OrthographicFar"  << YAML::Value << EditorCamera.GetOrthographicFarClip();
			}
			Out << YAML::Value << YAML::EndMap;
			// ~Orthographic

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
			EditorCamera.m_Position = EditorCameraNode["Position"].as<glm::vec3>(glm::vec3(0.0f));
			EditorCamera.m_Origin = EditorCameraNode["Origin"].as<glm::vec3>(glm::vec3(0.0f));
			EditorCamera.m_Distance = EditorCameraNode["Distance"].as<float>();
			EditorCamera.m_FocalPoint = EditorCameraNode["FocalPoint"].as<glm::vec3>(glm::vec3(0.0f));

			/* Perspective data. */
			const YAML::Node& PerspectiveCameraNode = EditorCameraNode["Perspective"];
			if (PerspectiveCameraNode.IsMap())
			{
				EditorCamera.m_DegPerspectiveFOV = PerspectiveCameraNode["DegPerspectiveFOV"].as<float>();
				EditorCamera.m_PerspectiveNear = PerspectiveCameraNode["PerspectiveNear"].as<float>();
				EditorCamera.m_PerspectiveFar = PerspectiveCameraNode["PerspectiveFar"].as<float>();

				EditorCamera.SetPerspectiveNearClip(EditorCamera.m_PerspectiveNear);
				EditorCamera.SetPerspectiveFarClip(EditorCamera.m_PerspectiveFar);

				EditorCamera.SetPerspectiveProjectionMatrix(
					glm::radians(EditorCamera.m_DegPerspectiveFOV), 
					EditorCamera.m_ViewportWidth,
					EditorCamera.m_ViewportHeight,
					EditorCamera.m_PerspectiveNear,
					EditorCamera.m_PerspectiveFar
				);

				EditorCamera.SetPerspective(EditorCamera.m_DegPerspectiveFOV, 
											EditorCamera.m_PerspectiveNear, 
											EditorCamera.m_PerspectiveFar);
			}

			/* Orthographic data. */
			const YAML::Node& OrthographicCameraNode = EditorCameraNode["Orthographic"];
			if (OrthographicCameraNode.IsMap())
			{
				EditorCamera.m_OrthographicSize = OrthographicCameraNode["OrthographicSize"].as<float>();
				EditorCamera.m_OrthographicNear = OrthographicCameraNode["OrthographicNear"].as<float>();
				EditorCamera.m_OrthographicFar  = OrthographicCameraNode["OrthographicFar"].as<float>();

				EditorCamera.SetOrthographicNearClip(EditorCamera.m_OrthographicNear);
				EditorCamera.SetOrthographicFarClip(EditorCamera.m_OrthographicFar);
			}

			/* Pitch and Yaw. */
			EditorCamera.m_Pitch = EditorCameraNode["Pitch"].as<float>();
			EditorCamera.m_Yaw = EditorCameraNode["Yaw"].as<float>();

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

	bool LSceneSerializer::Deserialize(const std::filesystem::path& Filepath)
	{
		LK_CORE_VERIFY(Scene, "Invalid scene reference");
		Scene->Clear();

		bool bOperationSuccess = false;

		std::ifstream InputStream(Filepath);
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();
		try
		{
			bOperationSuccess = DeserializeFromYAML(StringStream.str());
		}
		catch (const YAML::Exception& Exception)
		{
			LK_ASSERT(false, "Failed to deserialize scene '{}': {}", Filepath.string(), Exception.what());
			bOperationSuccess = false;
		}

		return bOperationSuccess;
	}

	bool LSceneSerializer::DeserializeRuntime(const FAssetHandle InSceneHandle)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		return true;
	}

	TObjectPtr<LScene> LSceneSerializer::LoadScene()
	{
		if (Scene)
		{
			Scene->m_ViewportWidth = LWindow::Get().GetViewportWidth();
			Scene->m_ViewportHeight = LWindow::Get().GetViewportHeight();
		}
		else
		{
			LK_CORE_ERROR_TAG("SceneSerializer", "LoadScene failed, no scene available");
		}

		return Scene;
	}

}
