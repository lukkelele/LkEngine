#include "LKpch.h"
#include "ProjectSerializer.h"

#include "LkEngine/Core/IO/FileSystem.h"

#include "LkEngine/Scene/Scene.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	FProjectSerializer::FProjectSerializer(TObjectPtr<LProject> InProject)
		: Project(InProject)
	{
	}

	void FProjectSerializer::Serialize(const std::filesystem::path& OutFile)
	{
		LK_CORE_VERIFY(!OutFile.empty(), "Cannot serialize an empty file");
		LK_CORE_DEBUG_TAG("ProjectSerializer", "Serializing: \"{}\"", OutFile.string());

		/* Create the project directory if it does not exist. */
		if (!fs::exists(OutFile.parent_path()))
		{
			fs::create_directories(OutFile.parent_path());
		}

		YAML::Emitter Out;
		SerializeToYaml(Out);

		/* Add file extension to the saved file. */
		const std::string ProjectSave = LK_FORMAT_STRING("{}.{}", OutFile.string(), LProject::FILE_EXTENSION);

		std::ofstream FileOut(ProjectSave);
		if (FileOut.is_open() && FileOut.good())
		{
			LK_CORE_DEBUG_TAG("ProjectSerializer", "Serializing to: \"{}\"", ProjectSave);
			FileOut << Out.c_str();
		}
		else
		{
			LK_CORE_ERROR_TAG("ProjectSerializer", "Failed to serialize: '{}'", OutFile.string());
		}
	}

	bool FProjectSerializer::Deserialize(const std::filesystem::path& InFile)
	{
		LK_CORE_VERIFY(Project, "Invalid project reference");
		if (!fs::exists(InFile))
		{
			LK_CORE_ERROR_TAG("ProjectSerializer", "Invalid filepath: {}", InFile.string());
			return false;
		}

		const std::filesystem::path ParentDir = InFile.parent_path();
		if (!fs::exists(ParentDir))
		{
			LK_CORE_DEBUG_TAG("ProjectSerializer", "Creating directory: {}", ParentDir.string());
			if (!fs::create_directories(ParentDir))
			{
				LK_CORE_ASSERT(false, "Failed to create directory: {}", ParentDir.string());
			}

			return false;
		}

		std::ifstream InputStream(InFile);
		std::stringstream StrStream;
		StrStream << InputStream.rdbuf();
		try
		{
			DeserializeFromYaml(StrStream.str(), Project->GetConfiguration());
		}
		catch (const YAML::Exception& Exception)
		{
			LK_CORE_ASSERT(false, "Failed to deserialize \"{}\", error: '{}'", InFile.string(), Exception.what());
			return false;
		}

		return true;
	}

	void FProjectSerializer::SerializeToYaml(YAML::Emitter& Out)
	{
		LK_CORE_VERIFY(Project);

		/* TODO: Add Scene name here. */

		Out << YAML::BeginMap;
		Out << YAML::Key << "Project" << YAML::Value;
		{
			const FProjectConfiguration& Config = Project->GetConfiguration();
			Out << YAML::BeginMap;
			Out << YAML::Key << "Name" << YAML::Value << Project->GetName();
			Out << YAML::Key << "AutoSave" << YAML::Value << Config.bAutoSave;
			Out << YAML::EndMap;
		}
		Out << YAML::EndMap;
	}

	bool FProjectSerializer::DeserializeFromYaml(const std::string& YamlString, FProjectConfiguration& ProjectConfig)
	{
		YAML::Node RootNode = YAML::Load(YamlString)["Project"];
		if (!RootNode.IsDefined())
		{
			LK_CORE_DEBUG_TAG("ProjectSerializer", "Yaml node is missing 'Project' node");
			return false;
		}

		LK_CORE_DEBUG_TAG("ProjectSerializer", "Deserializing project data");

		LK_DESERIALIZE_PROPERTY(Name, ProjectConfig.Name, RootNode, "Unknown");
		LK_DESERIALIZE_PROPERTY(AutoSave, ProjectConfig.bAutoSave, RootNode, true);

		LK_CORE_FATAL_TAG("ProjectSerializer", "Read name: {}", ProjectConfig.Name);


		return true;
	}

}
