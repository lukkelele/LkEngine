/******************************************************************
 * FProjectSerializer
 * 
 ******************************************************************/
#pragma once

#include "Project.h"

#include "LkEngine/Serialization/FileStream.h"
#include "LkEngine/Serialization/Serializer.h"

namespace LkEngine {

	class FProjectSerializer
	{
	public:
		FProjectSerializer(TObjectPtr<LProject> InProject);
		FProjectSerializer() = delete;
		~FProjectSerializer() = default;

        ESerializeResult Serialize(const std::filesystem::path& OutFile);
		bool Deserialize(const std::filesystem::path& InFile);

	private:
        void SerializeToYaml(YAML::Emitter& Out);
        bool DeserializeFromYaml(const std::string& YamlString, FProjectConfiguration& ProjectConfig);

	private:
		TObjectPtr<LProject> Project{};
	};

}
