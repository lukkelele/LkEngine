#pragma once

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Asset/Asset.h"


namespace YAML {
	class Emitter;
	class Node;
}

namespace LkEngine {

    class SceneSerializer
    {
    public:
		SceneSerializer(Scene* scene = nullptr);
		SceneSerializer(const Ref<Scene>& scene);
		~SceneSerializer() = default;

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

		void SerializeToYAML(YAML::Emitter& out);
		bool DeserializeFromYAML(const std::string& yamlString);

		// Retreive the most recent deserialized scene
		Ref<Scene> LoadScene();

		void SerializeRuntime(AssetHandle scene);
		bool DeserializeRuntime(AssetHandle scene);

	private:
		static void SerializeEntity(YAML::Emitter& out, Entity entity);
		static void DeserializeEntities(YAML::Node& entitiesNode, Ref<Scene> scene);
		static void DeserializeEntities(YAML::Node& entitiesNode, Scene* scene);

		static void SerializeEditorCamera(YAML::Emitter& out, EditorCamera& editorCamera);
		static void DeserializeEditorCamera(YAML::Node& editorCameraNode, EditorCamera& scene);

	public:
		inline static std::string_view FileFilter = "LkEngine Scene (*.lukkelele)\0*.lukkelele\0";
		inline static std::string_view DefaultExtension = ".lukkelele";
	private:
		bool m_IsLoaded = false;
		Scene* m_Scene = nullptr;
    };

}