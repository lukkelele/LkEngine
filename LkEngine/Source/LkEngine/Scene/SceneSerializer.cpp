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

	LSceneSerializer::LSceneSerializer(LScene* Scene)
		: m_Scene(Scene)
	{
		if (m_Scene == nullptr)
		{
			/// Make shared pointer.
			m_Scene = new LScene;
		}
	}

	/// FIXME
	LSceneSerializer::LSceneSerializer(const TObjectPtr<LScene>& Scene)
		: m_Scene((LScene*)Scene.Get())
	{
	}

	void LSceneSerializer::SerializeEntity(YAML::Emitter& out, LEntity entity)
	{
		UUID uuid = entity.GetComponent<LIDComponent>().ID;
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity";
		out << YAML::Value << uuid;

		if (entity.HasComponent<LTagComponent>())
		{
			out << YAML::Key << "LTagComponent";
			out << YAML::BeginMap; // LTagComponent

			auto& tag = entity.GetComponent<LTagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // LTagComponent
		}

		if (entity.HasComponent<LTransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& transform = entity.GetComponent<LTransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transform.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.GetRotationEuler();
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<LSpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap; // SpriteComponent

			auto& sprite = entity.GetComponent<LSpriteComponent>();
			out << YAML::Key << "Size" << YAML::Value << sprite.GetSize();
			out << YAML::Key << "Color" << YAML::Value << sprite.GetColor();

			out << YAML::EndMap; // SpriteComponent
		}

		//if (entity.HasComponent<MaterialComponent>())
		//{
		//	out << YAML::Key << "MaterialComponent";
		//	out << YAML::BeginMap; // MaterialComponent

		//	auto& material = entity.GetComponent<MaterialComponent>();
		//	out << YAML::Key << "TextureName" << YAML::Value << material.GetTexture()->GetName();

		//	out << YAML::EndMap; // SpriteComponent
		//}

		if (entity.HasComponent<LCameraComponent>())
		{
			out << YAML::Key << "LCameraComponent";
			out << YAML::BeginMap; // LCameraComponent

			LCameraComponent& CameraComponent = entity.GetComponent<LCameraComponent>();
			LCamera& Camera = *CameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(Camera.GetProjectionType());
			out << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << Camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::EndMap; // LCameraComponent
		}

		if (entity.HasComponent<LRigidBody2DComponent>())
		{
			out << YAML::Key << "LRigidBody2DComponent";
			out << YAML::BeginMap; // LRigidBody2DComponent

			auto& rigidbody2DComponent = entity.GetComponent<LRigidBody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(rigidbody2DComponent.BodyType);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;

			out << YAML::EndMap; // LRigidBody2DComponent
		}

		if (entity.HasComponent<LBoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& boxCollider2DComponent = entity.GetComponent<LBoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		out << YAML::EndMap; // Entity
	}

	void LSceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		SerializeToYAML(out);

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void LSceneSerializer::SerializeToYAML(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Scene"       << YAML::Value << m_Scene->GetName();
		out << YAML::Key << "Active"      << YAML::Value << m_Scene->m_IsActiveScene;
		out << YAML::Key << "EditorScene" << YAML::Value << m_Scene->m_EditorScene;
		out << YAML::Key << "SceneHandle" << YAML::Value << static_cast<uint32_t>(m_Scene->m_SceneEntity);

		LEditorLayer* Editor = LEditorLayer::Get();
#if 0
		if (SELECTION::SelectedEntity)
		{
			out << YAML::Key << "SelectedEntity" << YAML::Value << SELECTION::SelectedEntity.GetUUID();
		}
		else
		{
			out << YAML::Key << "SelectedEntity" << YAML::Value << 0;
		}
#endif

		// EditorCamera
		if (Editor && Editor->GetEditorCamera())
		{
			SerializeEditorCamera(out, *Editor->GetEditorCamera());
		}

		// 2D Physics
		out << YAML::Key << "Physics2D";
		out << YAML::Value << YAML::BeginMap;
		{
			Box2DWorldComponent& World2DComponent = m_Scene->GetBox2DWorld();
			if (&World2DComponent != nullptr)
			{
				out << YAML::Key << "Paused" << YAML::Value << World2DComponent.IsPaused();
				out << YAML::Key << "Gravity" << YAML::Value << World2DComponent.GetGravity();
				out << YAML::Key << "DebugDrawer" << YAML::Value << World2DComponent.HasDebugDrawerAttached();
				out << YAML::Key << "BodyCount" << YAML::Value << World2DComponent.GetBodyCount();
				out << YAML::Key << "ContactCount" << YAML::Value << World2DComponent.GetContactCount();
			}
			else
			{
			}
		}
		out << YAML::Value << YAML::EndMap;

		/* Entities. */
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		{
			// Sort entities by UUID 
			std::map<UUID, entt::entity> sortedEntityMap;
			auto idComponentView = m_Scene->m_Registry.view<LIDComponent>();
			for (const entt::entity entity : idComponentView)
			{
				sortedEntityMap[idComponentView.get<LIDComponent>(entity).ID] = entity;
			}
	
			// Serialize sorted entities.
			for (const auto& [id, entity] : sortedEntityMap)
			{
				SerializeEntity(out, { entity, m_Scene->m_ActiveScene });
			}
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
	}

	bool LSceneSerializer::DeserializeFromYAML(const std::string& YamlString)
	{
		YAML::Node data = YAML::Load(YamlString);
		if (!data["Scene"])
		{
			return false;
		}

		/* Scene */
		/* Parse scene name, if its currently the active scene and if it's an editor scene or not. */
		std::string SceneName = data["Scene"].as<std::string>(); 
		m_Scene->SetName(SceneName);

		const bool bIsActiveScene = data["Active"].as<std::string>() == "true" ? true : false;
		m_Scene->SetAsActive(bIsActiveScene);

		const bool bIsEditorScene = data["EditorScene"].as<std::string>() == "true" ? true : false;
		m_Scene->SetAsEditorScene(bIsEditorScene);
		uint32_t sceneEntityHandle = data["SceneHandle"].as<uint32_t>();

		/* EditorCamera. */
		auto EditorCameraNode = data["EditorCamera"];
		if (EditorCameraNode)
		{
			TObjectPtr<LEditorCamera> EditorCamera = LEditorLayer::Get()->GetEditorCamera();
			DeserializeEditorCamera(EditorCameraNode, *EditorCamera);
		}

		/// FIXME
	#if 0
		// Physics2D
		auto physics2DNode = data["Physics2D"];
		if (physics2DNode)
		{
			/// @FIXME: WHAT IS THIS DOING HERE ???????????
			/// MOVE AWAY FROM HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//Physics2DSpecification physics2DSpec;
			//physics2DSpec.Paused = physics2DNode["Paused"].as<bool>();
			//physics2DSpec.Gravity = physics2DNode["Gravity"].as<glm::vec2>(glm::vec2(0.0f));
			//physics2DSpec.DebugDrawer = physics2DNode["DebugDrawer"].as<bool>();
			// <<<<<<< Set physics parameters for the existing 2D world here >>>>>>>>
			//m_Scene->Initiate2DPhysics(physics2DSpec);
		}
	#endif

		// Entities
		YAML::Node entities = data["Entities"];
		if (entities)
		{
			DeserializeEntities(entities, m_Scene);
		}


		// Sort LIDComponent by by entity handle (which is essentially the order in which they were created)
		m_Scene->m_Registry.sort<LIDComponent>([this](const auto Lhs, const auto Rhs)
		{
			auto LhsEntity = m_Scene->m_EntityIDMap.find(Lhs.ID);
			auto RhsEntity = m_Scene->m_EntityIDMap.find(Rhs.ID);
			return static_cast<uint32_t>(LhsEntity->second) < static_cast<uint32_t>(RhsEntity->second);
		});

		return true;
	}

	void LSceneSerializer::SerializeRuntime(FAssetHandle scene)
	{
		LK_CORE_ASSERT(false, "SerializeRuntime: Not implemented yet");
	}

	void LSceneSerializer::DeserializeEntities(YAML::Node& entitiesNode, TObjectPtr<LScene> scene)
	{
		DeserializeEntities(entitiesNode, scene);
	}

	void LSceneSerializer::DeserializeEntities(YAML::Node& entitiesNode, LScene* scene)
	{
		for (auto entity : entitiesNode)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponent = entity["LTagComponent"];
			if (tagComponent)
			{
				name = tagComponent["Tag"].as<std::string>();
			}

			LEntity deserializedEntity = scene->CreateEntityWithID(uuid, name);

			const YAML::Node transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				// Entities always have transforms
				//LK_CORE_DEBUG_TAG("SceneSerializer", "Parsing TransformComponent");
				auto& transform = deserializedEntity.AddComponent<LTransformComponent>();
				transform.Translation = transformComponent["Position"].as<glm::vec3>(glm::vec3(0.0f));
				auto rotationNode = transformComponent["Rotation"];

				if (rotationNode.size() == 4)
				{
					transform.SetRotation(transformComponent["Rotation"].as<glm::quat>(glm::quat()));
				}
				else
				{
					transform.SetRotationEuler(transformComponent["Rotation"].as<glm::vec3>(glm::vec3(0.0f)));
				}

				transform.Scale = transformComponent["Scale"].as<glm::vec3>();
			}

			auto spriteComponent = entity["SpriteComponent"];
			if (spriteComponent)
			{
				LSpriteComponent& SpriteComponent = deserializedEntity.AddComponent<LSpriteComponent>();
				SpriteComponent.Size = spriteComponent["Size"].as<glm::vec2>(glm::vec2(0.0f));
				SpriteComponent.Color = spriteComponent["Color"].as<glm::vec4>(glm::vec4(0.0f));
			}

			//auto materialComponent = entity["MaterialComponent"];
			//if (materialComponent)
			//{
			//	MaterialComponent& mc = deserializedEntity.AddComponent<MaterialComponent>();
			//	mc.m_Material->SetTexture(TextureLibrary::Get()->GetTexture2D(materialComponent["TextureName"].as<std::string>()));
			//	LK_CORE_DEBUG_TAG("SceneSerializer", "Deserialized material component, set texture to \"{}\"", mc.m_Material->GetTextureName());
			//}

			auto cameraComponent = entity["LCameraComponent"];
			if (cameraComponent)
			{
				LCameraComponent& component = deserializedEntity.AddComponent<LCameraComponent>();
				const YAML::Node& cameraNode = cameraComponent["Camera"];

				component.Camera = TObjectPtr<LSceneCamera>::Create();
				auto& camera = component.Camera;

				if (cameraNode.IsMap())
				{
					if (cameraNode["ProjectionType"])
					{
						camera->SetProjectionType((LSceneCamera::ProjectionType)cameraNode["ProjectionType"].as<int>());
					}

					if (cameraNode["OrthographicNear"])
					{
						camera->SetOrthographicNearClip(cameraNode["OrthographicNear"].as<float>());
					}

					if (cameraNode["OrthographicFar"])
					{
						camera->SetOrthographicFarClip(cameraNode["OrthographicFar"].as<float>());
					}
				}

			}

			auto rigidBody2DComponent = entity["LRigidBody2DComponent"];
			if (rigidBody2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<LRigidBody2DComponent>();
				component.BodyType = (LRigidBody2DComponent::Type)rigidBody2DComponent["BodyType"].as<int>();
				component.FixedRotation = rigidBody2DComponent["FixedRotation"] ? rigidBody2DComponent["FixedRotation"].as<bool>() : false;
			}

			const YAML::Node boxCollider2DComponent = entity["BoxCollider2DComponent"];
			if (boxCollider2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<LBoxCollider2DComponent>();
				component.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
				component.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
				component.Density = boxCollider2DComponent["Density"] ? boxCollider2DComponent["Density"].as<float>() : 1.0f;
				component.Friction = boxCollider2DComponent["Friction"] ? boxCollider2DComponent["Friction"].as<float>() : 1.0f;
			}

			scene->SortEntities();
		}
	}

	void LSceneSerializer::SerializeEditorCamera(YAML::Emitter& out, LEditorCamera& EditorCamera)
	{
		out << YAML::Key << "EditorCamera" << YAML::Value;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Position"   << YAML::Value << EditorCamera.GetPosition();
			out << YAML::Key << "Origin"     << YAML::Value << EditorCamera.GetOrigin();
			out << YAML::Key << "Distance"   << YAML::Value << EditorCamera.GetDistance();
			out << YAML::Key << "FocalPoint" << YAML::Value << EditorCamera.GetFocalPoint();

			//----------------------------------------------------------------
			// Perspective 
			out << YAML::Key << "Perspective" << YAML::Value;
			out << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "DegPerspectiveFOV" << YAML::Value << EditorCamera.GetPerspectiveDegFov();
				out << YAML::Key << "PerspectiveNear"   << YAML::Value << EditorCamera.GetPerspectiveNearClip();
				out << YAML::Key << "PerspectiveFar"    << YAML::Value << EditorCamera.GetPerspectiveFarClip();
			}
			out << YAML::Value << YAML::EndMap;
			//----------------------------------------------------------------

			//----------------------------------------------------------------
			// Orthographic
			out << YAML::Key << "Orthographic" << YAML::Value;
			out << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "OrthographicSize" << YAML::Value << EditorCamera.GetOrthographicSize();
				out << YAML::Key << "OrthographicNear" << YAML::Value << EditorCamera.GetOrthographicNearClip();
				out << YAML::Key << "OrthographicFar"  << YAML::Value << EditorCamera.GetOrthographicFarClip();
			}
			out << YAML::Value << YAML::EndMap;
			//----------------------------------------------------------------

			out << YAML::Key << "Pitch"          << YAML::Value << EditorCamera.GetPitch();
			out << YAML::Key << "Yaw"            << YAML::Value << EditorCamera.GetYaw();
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)EditorCamera.GetProjectionType();
			out << YAML::Key << "CameraMode"     << YAML::Value << (int)EditorCamera.m_CameraMode;
		}
		out << YAML::EndMap;
	}

	void LSceneSerializer::DeserializeEditorCamera(YAML::Node& EditorCameraNode, LEditorCamera& EditorCamera)
	{
		if (EditorCameraNode)
		{
			// Position, origin, distance and focalpoint
			EditorCamera.m_Position = EditorCameraNode["Position"].as<glm::vec3>(glm::vec3(0.0f));
			EditorCamera.m_Origin = EditorCameraNode["Origin"].as<glm::vec3>(glm::vec3(0.0f));
			EditorCamera.m_Distance = EditorCameraNode["Distance"].as<float>();
			EditorCamera.m_FocalPoint = EditorCameraNode["FocalPoint"].as<glm::vec3>(glm::vec3(0.0f));

			// Perspective data
			const auto perspectiveCameraNode = EditorCameraNode["Perspective"];
			if (perspectiveCameraNode.IsMap())
			{
				EditorCamera.m_DegPerspectiveFOV = perspectiveCameraNode["DegPerspectiveFOV"].as<float>();
				EditorCamera.m_PerspectiveNear = perspectiveCameraNode["PerspectiveNear"].as<float>();
				EditorCamera.m_PerspectiveFar = perspectiveCameraNode["PerspectiveFar"].as<float>();
				EditorCamera.SetPerspectiveNearClip(EditorCamera.m_PerspectiveNear);
				EditorCamera.SetPerspectiveFarClip(EditorCamera.m_PerspectiveFar);
				EditorCamera.SetPerspectiveProjectionMatrix(
					glm::radians(EditorCamera.m_DegPerspectiveFOV), 
					EditorCamera.m_ViewportWidth,
					EditorCamera.m_ViewportHeight,
					EditorCamera.m_PerspectiveNear,
					EditorCamera.m_PerspectiveFar);
				EditorCamera.SetPerspective(EditorCamera.m_DegPerspectiveFOV, EditorCamera.m_PerspectiveNear, EditorCamera.m_PerspectiveFar);
				//LK_CORE_DEBUG_TAG("SceneSerializer", "Set editor camera farP={} and nearP={}", EditorCamera.m_PerspectiveFar, EditorCamera.m_PerspectiveNear);
			}

			// Orthographic data
			const auto orthographicCameraNode = EditorCameraNode["Orthographic"];
			if (orthographicCameraNode.IsMap())
			{
				EditorCamera.m_OrthographicSize = orthographicCameraNode["OrthographicSize"].as<float>();
				EditorCamera.m_OrthographicNear = orthographicCameraNode["OrthographicNear"].as<float>();
				EditorCamera.m_OrthographicFar  = orthographicCameraNode["OrthographicFar"].as<float>();
				EditorCamera.SetOrthographicNearClip(EditorCamera.m_OrthographicNear);
				EditorCamera.SetOrthographicFarClip(EditorCamera.m_OrthographicFar);
			}

			// Pitch and yaw
			EditorCamera.m_Pitch = EditorCameraNode["Pitch"].as<float>();
			EditorCamera.m_Yaw = EditorCameraNode["Yaw"].as<float>();

			// Projection Type, Perspective or Orthographic
			const int projectionType = EditorCameraNode["ProjectionType"].as<int>();
			if (projectionType == (int)LCamera::ProjectionType::Perspective)
			{
				EditorCamera.m_ProjectionType = LCamera::ProjectionType::Perspective;
			}
			else if (projectionType == (int)LCamera::ProjectionType::Orthographic)
			{
				EditorCamera.m_ProjectionType = LCamera::ProjectionType::Orthographic;
			}

			// Camera Mode, Flycam or Arcball
			const int cameraMode = EditorCameraNode["CameraMode"].as<int>();
			if (cameraMode == static_cast<int>(LEditorCamera::Mode::Flycam))
			{
				EditorCamera.m_CameraMode = LEditorCamera::Mode::Flycam;
			}
			else if (cameraMode == static_cast<int>(LEditorCamera::Mode::Arcball))
			{
				EditorCamera.m_CameraMode = LEditorCamera::Mode::Arcball;
			}
			else
			{
				EditorCamera.m_CameraMode = LEditorCamera::Mode::None;
			}
		}
	}

	bool LSceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		m_Scene->Clear();

		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		try
		{
			DeserializeFromYAML(strStream.str());
		}
		catch (const YAML::Exception& e)
		{
			LK_CORE_ASSERT(false, "Failed to deserialize scene '{0}': {1}", filepath.string(), e.what());
		}

		return true;
	}

	bool LSceneSerializer::DeserializeRuntime(FAssetHandle scene)
	{
		// Not implemented
		LK_CORE_ASSERT(false, "DeserializeRuntime: Not implemented yet");
		return true;
	}

	TObjectPtr<LScene> LSceneSerializer::LoadScene()
	{
		LK_CORE_ASSERT(m_Scene, "LSceneSerializer::GetLoadedScene  m_Scene == nullptr");

		m_Scene->m_ViewportWidth = LWindow::Get().GetViewportWidth();
		m_Scene->m_ViewportHeight = LWindow::Get().GetViewportHeight();

#if 0
		///
		/// CRITICAL: FIX THIS AWFUL CODE!!!!!!!!!!!!
		/// 
		if (m_Scene->m_IsActiveScene)
		{
			LScene::m_ActiveScene = m_Scene;

			/// FIXME:
			Input::SetScene(TObjectPtr<LScene>(m_Scene));
			LEditorLayer::Get()->SetScene(m_Scene);
			LApplication::Get()->SetScene(TObjectPtr<LScene>(m_Scene));
		}
		
		if (m_Scene->m_EditorScene)
		{
			LScene::m_ActiveScene = m_Scene;

			m_Scene->EditorCamera = LEditorLayer::Get()->GetEditorCamera();
			LK_CORE_ASSERT(m_Scene->EditorCamera, "SceneSerializer: EditorCamera is nullptr");

			/// FIXME:
			LEditorLayer::Get()->SetScene(m_Scene);
			LApplication::Get()->SetScene(TObjectPtr<LScene>(m_Scene));
			Input::SetScene(TObjectPtr<LScene>(m_Scene));
		}

		return TObjectPtr<LScene>(m_Scene);
#endif

		return nullptr;
	}

}