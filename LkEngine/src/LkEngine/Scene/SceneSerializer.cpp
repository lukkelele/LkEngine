#include "LKpch.h"
#include "SceneSerializer.h"

#include "LkEngine/Serialization/YAMLSerialization.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "SceneCamera.h"

#include "LkEngine/Renderer/Camera.h"

#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/Editor.h"


namespace LkEngine {

	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{
		if (m_Scene == nullptr)
			m_Scene = new Scene;

		//Box2DWorldComponent& box2dWorld = m_Scene->m_Registry.emplace<Box2DWorldComponent>(m_Scene->m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		//box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		//Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene((Scene*)scene.Raw())
	{
		//Box2DWorldComponent& box2dWorld = m_Scene->m_Registry.emplace<Box2DWorldComponent>(m_Scene->m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		//box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		//Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		UUID uuid = entity.GetComponent<IDComponent>().ID;
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity";
		out << YAML::Value << uuid;

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transform.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.GetRotationEuler();
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap; // SpriteComponent

			auto& sprite = entity.GetComponent<SpriteComponent>();
			out << YAML::Key << "Size" << YAML::Value << sprite.GetSize();
			out << YAML::Key << "Color" << YAML::Value << sprite.GetColor();

			out << YAML::EndMap; // SpriteComponent
		}

		if (entity.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent";
			out << YAML::BeginMap; // MaterialComponent

			auto& material = entity.GetComponent<MaterialComponent>();
			out << YAML::Key << "TextureName" << YAML::Value << material.GetTexture()->GetName();

			out << YAML::EndMap; // SpriteComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = *cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<RigidBody2DComponent>())
		{
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap; // RigidBody2DComponent

			auto& rigidbody2DComponent = entity.GetComponent<RigidBody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << (int)rigidbody2DComponent.BodyType;
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;

			out << YAML::EndMap; // RigidBody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		SerializeToYAML(out);

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeToYAML(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Scene"       << YAML::Value << m_Scene->GetName();
		out << YAML::Key << "Active"      << YAML::Value << m_Scene->m_IsActiveScene;
		out << YAML::Key << "EditorScene" << YAML::Value << m_Scene->m_EditorScene;
		out << YAML::Key << "SceneHandle" << YAML::Value << (uint32_t)m_Scene->m_SceneEntity;
		auto* editor = Editor::Get();
		if (editor->SelectedEntity)
			out << YAML::Key << "SelectedEntity" << YAML::Value << Editor::Get()->SelectedEntity.UUID();
		else
			out << YAML::Key << "SelectedEntity" << YAML::Value << 0;

		// EditorCamera
		if (editor && editor->GetEditorCamera())
			SerializeEditorCamera(out, *editor->GetEditorCamera());

		// 2D Physics
		out << YAML::Key << "Physics2D";
		out << YAML::Value << YAML::BeginMap;
		{
			auto& world2D = m_Scene->GetBox2DWorld();
			if (&world2D != nullptr)
			{
				out << YAML::Key << "Paused" << YAML::Value << world2D.IsPaused();
				out << YAML::Key << "Gravity" << YAML::Value << world2D.GetGravity();
				out << YAML::Key << "DebugDrawer" << YAML::Value << world2D.HasDebugDrawerAttached();
				out << YAML::Key << "BodyCount" << YAML::Value << world2D.GetBodyCount();
				out << YAML::Key << "ContactCount" << YAML::Value << world2D.GetContactCount();
			}
			else
			{
			}
		}
		out << YAML::Value << YAML::EndMap;

		// Entities
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		{
			// Sort entities by UUID 
			std::map<UUID, entt::entity> sortedEntityMap;
			auto idComponentView = m_Scene->m_Registry.view<IDComponent>();
			for (auto entity : idComponentView)
				sortedEntityMap[idComponentView.get<IDComponent>(entity).ID] = entity;
	
			// Serialize sorted entities
			for (auto [id, entity] : sortedEntityMap)
				SerializeEntity(out, { entity, m_Scene->m_ActiveScene });
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
	}

	bool SceneSerializer::DeserializeFromYAML(const std::string& yamlString)
	{
		YAML::Node data = YAML::Load(yamlString);
		if (!data["Scene"])
			return false;

		// Scene
		// Parse scene name, if its currently the active scene and if it's an editor scene or not
		std::string sceneName = data["Scene"].as<std::string>(); 
		m_Scene->SetName(sceneName);
		bool isActiveScene = data["Active"].as<std::string>() == "true" ? true : false;
		m_Scene->SetAsActive(isActiveScene);
		bool editorScene = data["EditorScene"].as<std::string>() == "true" ? true : false;
		m_Scene->SetAsEditorScene(editorScene);
		uint32_t sceneEntityHandle = data["SceneHandle"].as<uint32_t>();

		// EditorCamera
		auto& editorCamera = *Editor::Get()->GetEditorCamera();
		auto editorCameraNode = data["EditorCamera"];
		if (editorCameraNode)
			DeserializeEditorCamera(editorCameraNode, editorCamera);

		// Physics2D
		auto physics2DNode = data["Physics2D"];
		if (physics2DNode)
		{
			Physics2DSpecification physics2DSpec;
			
			physics2DSpec.Paused = physics2DNode["Paused"].as<bool>();
			physics2DSpec.Gravity = physics2DNode["Gravity"].as<glm::vec2>(glm::vec2(0.0f));
			physics2DSpec.DebugDrawer = physics2DNode["DebugDrawer"].as<bool>();
			// <<<<<<< Set physics parameters for the existing 2D world here >>>>>>>>
			//m_Scene->Initiate2DPhysics(physics2DSpec);
		}

		// Entities
		auto entities = data["Entities"];
		if (entities)
			DeserializeEntities(entities, m_Scene);


		// Sort IDComponent by by entity handle (which is essentially the order in which they were created)
		m_Scene->m_Registry.sort<IDComponent>([this](const auto lhs, const auto rhs)
		{
			auto lhsEntity = m_Scene->m_EntityMap.find(lhs.ID);
			auto rhsEntity = m_Scene->m_EntityMap.find(rhs.ID);
			return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
		});

		return true;
	}

	void SceneSerializer::SerializeRuntime(AssetHandle scene)
	{
		LK_CORE_ASSERT(false, "SerializeRuntime: Not implemented yet");
	}

	void SceneSerializer::DeserializeEntities(YAML::Node& entitiesNode, Ref<Scene> scene)
	{
		DeserializeEntities(entitiesNode, scene);
	}

	void SceneSerializer::DeserializeEntities(YAML::Node& entitiesNode, Scene* scene)
	{
		for (auto entity : entitiesNode)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			Entity deserializedEntity = scene->CreateEntityWithID(uuid, name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				// Entities always have transforms
				//LK_CORE_DEBUG_TAG("SceneSerializer", "Parsing TransformComponent");
				auto& transform = deserializedEntity.AddComponent<TransformComponent>();
				transform.Translation = transformComponent["Position"].as<glm::vec3>(glm::vec3(0.0f));
				auto rotationNode = transformComponent["Rotation"];
				if (rotationNode.size() == 4)
					transform.SetRotation(transformComponent["Rotation"].as<glm::quat>(glm::quat()));
				else
					transform.SetRotationEuler(transformComponent["Rotation"].as<glm::vec3>(glm::vec3(0.0f)));
				transform.Scale = transformComponent["Scale"].as<glm::vec3>();
			}

			auto spriteComponent = entity["SpriteComponent"];
			if (spriteComponent)
			{
				auto& sprite = deserializedEntity.AddComponent<SpriteComponent>();
				sprite.Size = spriteComponent["Size"].as<glm::vec2>(glm::vec2(0.0f));
				sprite.Color = spriteComponent["Color"].as<glm::vec4>(glm::vec4(0.0f));
			}

			auto materialComponent = entity["MaterialComponent"];
			if (materialComponent)
			{
				MaterialComponent& material = deserializedEntity.AddComponent<MaterialComponent>();
				material.m_Material->SetTexture(TextureLibrary::Get()->GetTexture2D(materialComponent["TextureName"].as<std::string>()));
				LK_CORE_DEBUG_TAG("SceneSerializer", "Deserialized material component, set texture to \"{}\"", material.GetTextureName());
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				CameraComponent& component = deserializedEntity.AddComponent<CameraComponent>();
				const auto& cameraNode = cameraComponent["Camera"];

				component.Camera = Ref<SceneCamera>::Create();
				auto& camera = component.Camera;

				if (cameraNode.IsMap())
				{
					if (cameraNode["ProjectionType"])
						camera->SetProjectionType((SceneCamera::ProjectionType)cameraNode["ProjectionType"].as<int>());
					if (cameraNode["OrthographicNear"])
						camera->SetOrthographicNearClip(cameraNode["OrthographicNear"].as<float>());
					if (cameraNode["OrthographicFar"])
						camera->SetOrthographicFarClip(cameraNode["OrthographicFar"].as<float>());
				}

			}

			auto rigidBody2DComponent = entity["RigidBody2DComponent"];
			if (rigidBody2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<RigidBody2DComponent>();
				component.BodyType = (RigidBody2DComponent::Type)rigidBody2DComponent["BodyType"].as<int>();
				component.FixedRotation = rigidBody2DComponent["FixedRotation"] ? rigidBody2DComponent["FixedRotation"].as<bool>() : false;
			}

			auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
			if (boxCollider2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				component.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
				component.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
				component.Density = boxCollider2DComponent["Density"] ? boxCollider2DComponent["Density"].as<float>() : 1.0f;
				component.Friction = boxCollider2DComponent["Friction"] ? boxCollider2DComponent["Friction"].as<float>() : 1.0f;
			}

			scene->SortEntities();
		}
	}

	void SceneSerializer::SerializeEditorCamera(YAML::Emitter& out, EditorCamera& editorCamera)
	{
		out << YAML::Key << "EditorCamera" << YAML::Value;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Position"   << YAML::Value << editorCamera.GetPosition();
			out << YAML::Key << "Origin"     << YAML::Value << editorCamera.GetOrigin();
			out << YAML::Key << "Distance"   << YAML::Value << editorCamera.GetDistance();
			out << YAML::Key << "FocalPoint" << YAML::Value << editorCamera.GetFocalPoint();

			//----------------------------------------------------------------
			// Perspective 
			out << YAML::Key << "Perspective" << YAML::Value;
			out << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "DegPerspectiveFOV" << YAML::Value << editorCamera.GetPerspectiveDegFov();
				out << YAML::Key << "PerspectiveNear"   << YAML::Value << editorCamera.GetPerspectiveNearClip();
				out << YAML::Key << "PerspectiveFar"    << YAML::Value << editorCamera.GetPerspectiveFarClip();
			}
			out << YAML::Value << YAML::EndMap;
			//----------------------------------------------------------------

			//----------------------------------------------------------------
			// Orthographic
			out << YAML::Key << "Orthographic" << YAML::Value;
			out << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "OrthographicSize" << YAML::Value << editorCamera.GetOrthographicSize();
				out << YAML::Key << "OrthographicNear" << YAML::Value << editorCamera.GetOrthographicNearClip();
				out << YAML::Key << "OrthographicFar"  << YAML::Value << editorCamera.GetOrthographicFarClip();
			}
			out << YAML::Value << YAML::EndMap;
			//----------------------------------------------------------------

			out << YAML::Key << "Pitch"          << YAML::Value << editorCamera.GetPitch();
			out << YAML::Key << "Yaw"            << YAML::Value << editorCamera.GetYaw();
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)editorCamera.GetProjectionType();
			out << YAML::Key << "CameraMode"     << YAML::Value << (int)editorCamera.m_CameraMode;
		}
		out << YAML::EndMap;
	}

	void SceneSerializer::DeserializeEditorCamera(YAML::Node& editorCameraNode, EditorCamera& editorCamera)
	{
		if (editorCameraNode)
		{
			// Position, origin, distance and focalpoint
			editorCamera.m_Position = editorCameraNode["Position"].as<glm::vec3>(glm::vec3(0.0f));
			editorCamera.m_Origin = editorCameraNode["Origin"].as<glm::vec3>(glm::vec3(0.0f));
			editorCamera.m_Distance = editorCameraNode["Distance"].as<float>();
			editorCamera.m_FocalPoint = editorCameraNode["FocalPoint"].as<glm::vec3>(glm::vec3(0.0f));

			// Perspective data
			const auto perspectiveCameraNode = editorCameraNode["Perspective"];
			if (perspectiveCameraNode.IsMap())
			{
				editorCamera.m_DegPerspectiveFOV = perspectiveCameraNode["DegPerspectiveFOV"].as<float>();
				editorCamera.m_PerspectiveNear = perspectiveCameraNode["PerspectiveNear"].as<float>();
				editorCamera.m_PerspectiveFar = perspectiveCameraNode["PerspectiveFar"].as<float>();
				editorCamera.SetPerspectiveNearClip(editorCamera.m_PerspectiveNear);
				editorCamera.SetPerspectiveFarClip(editorCamera.m_PerspectiveFar);
				editorCamera.SetPerspectiveProjectionMatrix(
					glm::radians(editorCamera.m_DegPerspectiveFOV), 
					editorCamera.m_ViewportWidth,
					editorCamera.m_ViewportHeight,
					editorCamera.m_PerspectiveNear,
					editorCamera.m_PerspectiveFar);
				editorCamera.SetPerspective(editorCamera.m_DegPerspectiveFOV, editorCamera.m_PerspectiveNear, editorCamera.m_PerspectiveFar);
				//LK_CORE_DEBUG_TAG("SceneSerializer", "Set editor camera farP={} and nearP={}", editorCamera.m_PerspectiveFar, editorCamera.m_PerspectiveNear);
			}

			// Orthographic data
			const auto orthographicCameraNode = editorCameraNode["Orthographic"];
			if (orthographicCameraNode.IsMap())
			{
				editorCamera.m_OrthographicSize = orthographicCameraNode["OrthographicSize"].as<float>();
				editorCamera.m_OrthographicNear = orthographicCameraNode["OrthographicNear"].as<float>();
				editorCamera.m_OrthographicFar  = orthographicCameraNode["OrthographicFar"].as<float>();
				editorCamera.SetOrthographicNearClip(editorCamera.m_OrthographicNear);
				editorCamera.SetOrthographicFarClip(editorCamera.m_OrthographicFar);
			}

			// Pitch and yaw
			editorCamera.m_Pitch = editorCameraNode["Pitch"].as<float>();
			editorCamera.m_Yaw = editorCameraNode["Yaw"].as<float>();

			// Projection Type, Perspective or Orthographic
			int projectionType = editorCameraNode["ProjectionType"].as<int>();
			if (projectionType == (int)Camera::ProjectionType::Perspective)
				editorCamera.m_ProjectionType = Camera::ProjectionType::Perspective;
			else if (projectionType == (int)Camera::ProjectionType::Orthographic)
				editorCamera.m_ProjectionType = Camera::ProjectionType::Orthographic;

			// Camera Mode, Flycam or Arcball
			int cameraMode = editorCameraNode["CameraMode"].as<int>();
			if (cameraMode == (int)EditorCamera::Mode::Flycam)
				editorCamera.m_CameraMode = EditorCamera::Mode::Flycam;
			else if (cameraMode == (int)EditorCamera::Mode::Arcball)
				editorCamera.m_CameraMode = EditorCamera::Mode::Arcball;
			else
				editorCamera.m_CameraMode = EditorCamera::Mode::None;
		}
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
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

	bool SceneSerializer::DeserializeRuntime(AssetHandle scene)
	{
		// Not implemented
		LK_CORE_ASSERT(false, "DeserializeRuntime: Not implemented yet");
		return true;
	}

	Ref<Scene> SceneSerializer::LoadScene()
	{
		if (m_Scene == nullptr)
			LK_CORE_ASSERT(false, "SceneSerializer::GetLoadedScene  m_Scene == nullptr");

		m_Scene->m_ViewportWidth = Window::Get().GetViewportWidth();
		m_Scene->m_ViewportHeight = Window::Get().GetViewportHeight();

		if (m_Scene->m_IsActiveScene)
		{
			Scene::m_ActiveScene = m_Scene;

			Input::SetScene(Ref<Scene>(m_Scene));
			Editor::Get()->SetScene(*m_Scene);
			Application::Get()->SetScene(Ref<Scene>(m_Scene));
		}
		
		if (m_Scene->m_EditorScene)
		{
			m_Scene->m_EditorCamera = Editor::Get()->GetEditorCamera();
			LK_CORE_ASSERT(m_Scene->m_EditorCamera, "SceneSerializer: EditorCamera is nullptr");

			Editor::Get()->SetScene(*m_Scene);
			Application::Get()->SetScene(Ref<Scene>(m_Scene));
			Input::SetScene(Ref<Scene>(m_Scene));
		}

		// Initiate 2D physics
		//Box2DWorldComponent& box2dWorld = m_Scene->m_Registry.emplace<Box2DWorldComponent>(m_Scene->m_SceneEntity, std::make_unique<b2World>(b2Vec2{ 0.0f, -9.8f }));
		//box2dWorld.World->SetContactListener(&box2dWorld.ContactListener);
		//Debugger::AttachDebugDrawer2D(&box2dWorld, Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints);

		return Ref<Scene>(m_Scene);
	}

}