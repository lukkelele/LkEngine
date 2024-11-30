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
		LSceneSerializer(LScene* InScene);
		LSceneSerializer(const TObjectPtr<LScene>& InScene);
		LSceneSerializer() = delete;

		void Serialize(const std::filesystem::path& Filepath);
		bool Deserialize(const std::filesystem::path& Filepath);

		void SerializeToYAML(YAML::Emitter& out);
		bool DeserializeFromYAML(const std::string& YamlString);

		/** FIXME: Move this to LProject, should not exist in here. */
		TObjectPtr<LScene> LoadScene();

		void SerializeRuntime(const FAssetHandle InSceneHandle);
		bool DeserializeRuntime(const FAssetHandle InSceneHandle);

	private:
		static void SerializeEntity(YAML::Emitter& Out, LEntity Entity);
		static void DeserializeEntities(YAML::Node& EntitiesNode, TObjectPtr<LScene> SceneRef);

		static void SerializeEditorCamera(YAML::Emitter& Out, LEditorCamera& EditorCamera);
		static void DeserializeEditorCamera(const YAML::Node& EditorCameraNode, LEditorCamera& EditorCamera);

	public:
		static std::string FILE_FILTER;
	private:
		LScene* Scene = nullptr;
		bool bIsLoaded = false;

		LCLASS(LSceneSerializer);
    };

}
