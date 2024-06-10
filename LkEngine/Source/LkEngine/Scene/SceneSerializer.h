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
		SceneSerializer(Scene* Scene = nullptr); /// @FIXME
		SceneSerializer(const Ref<Scene>& Scene);

		void Serialize(const std::filesystem::path& Filepath);
		bool Deserialize(const std::filesystem::path& Filepath);

		void SerializeToYAML(YAML::Emitter& out);
		bool DeserializeFromYAML(const std::string& YamlString);

		// Retreive the most recent deserialized Scene
		Ref<Scene> LoadScene();

		void SerializeRuntime(AssetHandle Scene);
		bool DeserializeRuntime(AssetHandle Scene);

	private:
		static void SerializeEntity(YAML::Emitter& out, LEntity Entity);
		static void DeserializeEntities(YAML::Node& EntitiesNode, Ref<Scene> Scene);
		static void DeserializeEntities(YAML::Node& EntitiesNode, Scene* Scene);

		static void SerializeEditorCamera(YAML::Emitter& out, LEditorCamera& editorCamera);
		static void DeserializeEditorCamera(YAML::Node& editorCameraNode, LEditorCamera& Scene);

	public:
		inline static std::string_view FileFilter = "LkEngine Scene (*.lukkelele)\0*.lukkelele\0";
		inline static std::string_view DefaultExtension = ".lukkelele";

	private:
		bool m_IsLoaded = false;
		Scene* m_Scene = nullptr;
    };

}