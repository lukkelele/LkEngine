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

		bool Serialize(const std::filesystem::path& Filepath);
		bool Deserialize(const std::filesystem::path& Filepath);

		/** FIXME: Move this to LProject, should not exist in here. */
		TObjectPtr<LScene> LoadScene();

		void SerializeRuntime(const LUUID InSceneHandle);
		bool DeserializeRuntime(const LUUID InSceneHandle);

	private:
		bool SerializeToYaml(YAML::Emitter& out);
		bool DeserializeFromYaml(const std::string& YamlString);

		void SerializeEntity(YAML::Emitter& Out, LEntity Entity);
		void DeserializeEntities(YAML::Node& EntitiesNode, TObjectPtr<LScene> SceneRef);

		void SerializeEditorCamera(YAML::Emitter& Out, LEditorCamera& EditorCamera);
		void DeserializeEditorCamera(const YAML::Node& EditorCameraNode, LEditorCamera& EditorCamera);

	public:
		static std::string FILE_FILTER;
	private:
		LScene* Scene = nullptr;
		bool bIsLoaded = false;

		LCLASS(LSceneSerializer);
    };

}
