#include "LKpch.h"
#include "SceneSerializer.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Camera.h"

#include "LkEngine/Serialization/YAMLSerialization.h"


namespace LkEngine {

	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{
	}

	SceneSerializer::SceneSerializer(const s_ptr<Scene>& scene)
		: m_Scene(scene.get())
	{
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

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = *cameraComponent.CameraRef;
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
		out << YAML::Key << "Scene";
		out << YAML::Value << m_Scene->GetName();

		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;

		// Sort entities by UUID 
		std::map<UUID, entt::entity> sortedEntityMap;
		auto idComponentView = m_Scene->m_Registry.view<IDComponent>();
		for (auto entity : idComponentView)
			sortedEntityMap[idComponentView.get<IDComponent>(entity).ID] = entity;

		// Serialize sorted entities
		for (auto [id, entity] : sortedEntityMap)
			SerializeEntity(out, { entity, m_Scene->s_ActiveScene });

		out << YAML::EndSeq;


		out << YAML::EndMap;
	}

	bool SceneSerializer::DeserializeFromYAML(const std::string& yamlString)
	{
		YAML::Node data = YAML::Load(yamlString);
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		m_Scene->SetName(sceneName);

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
		// Not implemented
		throw std::runtime_error("SerializeRuntime: Not implemented yet");
	}

	void SceneSerializer::DeserializeEntities(YAML::Node& entitiesNode, s_ptr<Scene> scene)
	{
		DeserializeEntities(entitiesNode, scene.get());
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
				auto& transform = deserializedEntity.GetComponent<TransformComponent>();
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

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& component = deserializedEntity.AddComponent<CameraComponent>();
				const auto& cameraNode = cameraComponent["Camera"];

				component.CameraRef = new SceneCamera();
				auto& camera = component.CameraRef;

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
			throw std::runtime_error(fmt::format("Failed to deserialize scene '{0}': {1}", filepath.string(), e.what()).c_str());
			return false;
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(AssetHandle scene)
	{
		// Not implemented
		throw std::runtime_error("DeserializeRuntime: Not implemented yet");
		return false;
	}

}