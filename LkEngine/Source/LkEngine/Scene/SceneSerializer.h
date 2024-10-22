#pragma once

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Asset/Asset.h"


namespace YAML
{
	class Emitter;
	class Node;
}

namespace LkEngine {

    class LSceneSerializer : public LObject
    {
    public:
		LSceneSerializer(LScene* Scene = nullptr); /// @FIXME
		LSceneSerializer(const TObjectPtr<LScene>& Scene);

		void Serialize(const std::filesystem::path& Filepath);
		bool Deserialize(const std::filesystem::path& Filepath);

		void SerializeToYAML(YAML::Emitter& out);
		bool DeserializeFromYAML(const std::string& YamlString);

		/** Retreive the most recent deserialized scene. */
		TObjectPtr<LScene> LoadScene();

		void SerializeRuntime(const FAssetHandle InSceneHandle);
		bool DeserializeRuntime(const FAssetHandle InSceneHandle);

	private:
		static void SerializeEntity(YAML::Emitter& out, LEntity Entity);
		static void DeserializeEntities(YAML::Node& EntitiesNode, TObjectPtr<LScene> Scene);

		static void SerializeEditorCamera(YAML::Emitter& out, LEditorCamera& editorCamera);
		static void DeserializeEditorCamera(const YAML::Node& EditorCameraNode, LEditorCamera& Scene);

	public:
		inline static std::string_view FileFilter = "LkEngine Scene (*.lukkelele)\0*.lukkelele\0";
		inline static std::string_view DefaultExtension = ".lukkelele";

	private:
		bool m_IsLoaded = false;
		LScene* m_Scene = nullptr;

		LCLASS(LSceneSerializer);
    };

}