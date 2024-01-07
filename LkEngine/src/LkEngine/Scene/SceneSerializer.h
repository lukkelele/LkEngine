#pragma once

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Serialization/FileStream.h"

namespace YAML {
	class Emitter;
	class Node;
}

namespace LkEngine {

    class SceneSerializer
    {
    public:
		SceneSerializer(Scene* scene);
		SceneSerializer(const s_ptr<Scene>& scene);
		~SceneSerializer() = default;

		static void SerializeEntity(YAML::Emitter& out, Entity entity);
		static void DeserializeEntities(YAML::Node& entitiesNode, s_ptr<Scene> scene);
		static void DeserializeEntities(YAML::Node& entitiesNode, Scene* scene);

		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(AssetHandle scene);
		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(AssetHandle scene);

		void SerializeToYAML(YAML::Emitter& out);
		bool DeserializeFromYAML(const std::string& yamlString);

	public:
		inline static std::string_view FileFilter = "LkEngine Scene (*.lukkelele)\0*.lukkelele\0";
		inline static std::string_view DefaultExtension = ".lukkelele";
	private:
		Scene* m_Scene = nullptr;
    };

}