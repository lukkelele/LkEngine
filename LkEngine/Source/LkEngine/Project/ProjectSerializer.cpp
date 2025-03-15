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
		LK_CORE_INFO_TAG("ProjectSerializer", "Serializing to file: {}", OutFile);

		/* Create the project directory if it does not exist. */
		if (!fs::exists(OutFile.parent_path()))
		{
			LK_CORE_CONSOLE_DEBUG("Creating parent directories for project: {}", OutFile.parent_path());
			fs::create_directories(OutFile.parent_path());
		}

		YAML::Emitter Out;
		SerializeToYaml(Out);

		/* Add file extension to the saved file. */
		const std::string ProjectSave = LK_FMT_LIB::format("{}/{}.{}", OutFile.string(), Project->GetName(), LProject::FILE_EXTENSION);
		LK_CORE_TRACE("Project Save: {}", ProjectSave);

		std::ofstream FileOut(ProjectSave);
		if (FileOut.is_open() && FileOut.good())
		{
			LK_CORE_INFO_TAG("ProjectSerializer", "Saving: {}", ProjectSave);
			FileOut << Out.c_str();
		}
		else
		{
			LK_CORE_ERROR_TAG("ProjectSerializer", "Failed to serialize: '{}'", OutFile.string());
		}
	}

	bool FProjectSerializer::Deserialize(const std::filesystem::path& InFile)
	{
		LK_CORE_VERIFY(Project);
		if (!fs::exists(InFile))
		{
			LK_CORE_ERROR_TAG("ProjectSerializer", "Invalid filepath: {}", InFile.string());
			return false;
		}

		std::ifstream InputStream(InFile);
		std::stringstream StrStream;
		StrStream << InputStream.rdbuf();
		try
		{
			LK_CORE_DEBUG_TAG("ProjectSerializer", "Deserializing: {}", InFile.string());
			DeserializeFromYaml(StrStream.str(), Project->GetConfiguration());
		}
		catch (const YAML::Exception& Exception)
		{
			LK_CORE_ASSERT(false, "Deserialization failed for '{}', error: '{}'", InFile.string(), Exception.what());
			return false;
		}

		return true;
	}

	void FProjectSerializer::SerializeToYaml(YAML::Emitter& Out)
	{
		LK_CORE_VERIFY(Project);

		Out << YAML::BeginMap;
		Out << YAML::Key << "Project" << YAML::Value;
		{
			const FProjectConfiguration& Config = Project->GetConfiguration();
			const std::filesystem::path AssetDirectory = std::filesystem::relative(Config.AssetDirectory, LFileSystem::GetRuntimeDir());
			const std::filesystem::path AssetRegistryPath = std::filesystem::relative(Config.AssetRegistryPath, LFileSystem::GetRuntimeDir());

			Out << YAML::BeginMap;
			Out << YAML::Key << "Name" << YAML::Value << Project->GetName();
			Out << YAML::Key << "AssetDirectory" << YAML::Value << LFileSystem::ConvertToUnixPath(AssetDirectory);
			Out << YAML::Key << "AssetRegistryPath" << YAML::Value << LFileSystem::ConvertToUnixPath(AssetRegistryPath);
			Out << YAML::Key << "MeshDirectory" << YAML::Value << Config.MeshDirectory;
			Out << YAML::Key << "StartScene" << YAML::Value << Config.StartScene;
			Out << YAML::Key << "AutoSave" << YAML::Value << Config.bAutoSave;
			Out << YAML::Key << "AutoSaveInterval" << YAML::Value << Config.AutoSaveInterval.count();
			Out << YAML::EndMap;
		}
		Out << YAML::EndMap;
	}

	bool FProjectSerializer::DeserializeFromYaml(const std::string& YamlString, FProjectConfiguration& ProjectConfig)
	{
		LK_CORE_TRACE_TAG("ProjectSerializer", "Deserializing YAML");
		YAML::Node RootNode = YAML::Load(YamlString)["Project"];
		if (!RootNode.IsDefined())
		{
			LK_CORE_ERROR_TAG("ProjectSerializer", "Yaml node is missing 'Project' node\n\n{}", YamlString);
			return false;
		}

		LK_CORE_TRACE("\n{}", YamlString);

		LK_DESERIALIZE_PROPERTY(Name, ProjectConfig.Name, RootNode, "Unknown");
		LK_DESERIALIZE_PROPERTY(AssetDirectory, ProjectConfig.AssetDirectory, RootNode, "");
		LK_DESERIALIZE_PROPERTY(AssetRegistryPath, ProjectConfig.AssetRegistryPath, RootNode, "");
		LK_DESERIALIZE_PROPERTY(MeshDirectory, ProjectConfig.MeshDirectory, RootNode, "");
		LK_DESERIALIZE_PROPERTY(StartScene, ProjectConfig.StartScene, RootNode, "");
		LK_DESERIALIZE_PROPERTY(AutoSave, ProjectConfig.bAutoSave, RootNode, true);
		LK_DESERIALIZE_PROPERTY(AutoSaveInterval, ProjectConfig.AutoSaveInterval, RootNode, 150s);

	#if defined(LK_PLATFORM_LINUX)
		LK_CORE_WARN("WorkingDir: {}", LFileSystem::GetWorkingDir());
		LK_CORE_WARN("RuntimeDir: {}", LFileSystem::GetRuntimeDir());
		LK_CORE_DEBUG("ProjectConfig.Name: {}", ProjectConfig.Name);
		LK_CORE_DEBUG("ProjectConfig.AssetDirectory: {}", ProjectConfig.AssetDirectory);
		LK_CORE_DEBUG("ProjectConfig.AssetRegistryPath: {}", ProjectConfig.AssetRegistryPath);
		LK_CORE_DEBUG("ProjectConfig.MeshDirectory: {}", ProjectConfig.MeshDirectory);
	#endif

		return true;
	}

}
