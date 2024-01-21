#include "LKpch.h"
#include "SceneLoader.h"

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	template<typename T>
	T SceneLoader::ParseComponent(const YAML::Node& node)
	{
		LK_UNUSED(node);
	}

	template<>
	TransformComponent SceneLoader::ParseComponent<TransformComponent>(const YAML::Node& node)
	{
		LK_CORE_DEBUG_TAG("SceneLoader", "Parsing TransformComponent");

		auto position = node["Position"].as<std::vector<float>>();
		auto scale = node["Scale"].as<std::vector<float>>();
		auto rotation = node["Rotation"].as<std::vector<float>>();

		TransformComponent tc;
		tc.Translation = { position[0], position[1], position[2] };
		tc.Scale = { scale[0], scale[1], scale[2] };
		tc.SetRotation(glm::quat(rotation[0], rotation[1], rotation[2], 0));

		LK_CORE_DEBUG_TAG("SceneLoader", "Translation = ({}, {}, {})", tc.Translation.x, tc.Translation.y, tc.Translation.z);

		return tc;
	}

	Ref<Scene> SceneLoader::LoadScene(const std::string& filepath)
	{
	    YAML::Node scene = YAML::LoadFile(filepath);
		std::string sceneName = scene["Scene"].as<std::string>(); 

		Ref<Scene> loadedScene = Ref<Scene>::Create("Loaded Scene");

		const YAML::Node& entities = scene["Entities"];
		for (const auto& entity : entities)
		{
			//Entity e = loadedScene->CreateEntity(entity.as<std::string>().c_str());
			Entity e = loadedScene->CreateEntity(entity.Scalar());
			LK_CORE_DEBUG_TAG("SceneLoader", "Loading entity: {}", e.Name());

			IDComponent& id = e.GetComponent<IDComponent>();
			TagComponent& tagComponent = e.Tag();
			TransformComponent& tc = e.AddComponent<TransformComponent>();

			id.ID = entity["Entity"].as<uint64_t>();

			if (entity["TagComponent"])
				tagComponent.Tag = entity["TagComponent"]["Tag"].as<std::string>();
			if (entity["TransformComponent"])
				tc = ParseComponent<TransformComponent>(entity["TransformComponent"]);
		}
		return loadedScene;
	}

	Ref<Scene> SceneLoader::LoadScene(AssetHandle handle)
	{
		return nullptr;
	}

}