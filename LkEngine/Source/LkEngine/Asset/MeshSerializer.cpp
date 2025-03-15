#include "LKpch.h"
#include "MeshSerializer.h"

#include "AssetManager.h"
#include "AssimpMeshImporter.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	void LMeshSourceSerializer::Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const
	{
		/* Empty */
		LK_UNUSED(Metadata);
		LK_UNUSED(Asset);
	}

	bool LMeshSourceSerializer::TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const
	{
		LK_CORE_DEBUG_TAG("MeshSourceSerializer", "TryLoadData: {}", Metadata.FilePath.string());
	#if defined(LK_PLATFORM_WINDOWS)
		const std::filesystem::path FilePath = LProject::GetEditorAssetManager()->GetFileSystemPath(Metadata);
	#elif defined(LK_PLATFORM_LINUX)
		const std::filesystem::path FilePath = LProject::GetEditorAssetManager()->GetFileSystemPath(Metadata);
	#endif
		LAssimpMeshImporter Importer(FilePath.string());

		TObjectPtr<LMeshSource> MeshSource = Importer.ImportToMeshSource();
		if (!MeshSource)
		{
			LK_CORE_ERROR_TAG("MeshSerializer", "Failed to import MeshSource from: {}", FilePath.string());
			return false;
		}

		Asset = MeshSource;
		Asset->Handle = Metadata.Handle;

		return true;
	}

	/*************************************************************************************************/

	void LMeshSerializer::Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const
	{
		TObjectPtr<LMesh> Mesh = Asset.As<LMesh>();
		const std::string YamlString = SerializeToYaml(Mesh);

		std::filesystem::path SerializePath = LProject::Current()->GetAssetDirectory() / Metadata.FilePath;
		LK_CORE_TRACE_TAG("MeshSerializer", "Serializing: {}", Metadata.ToString());
		std::ofstream FileOut(SerializePath);
		if (!FileOut.is_open())
		{
			LK_CORE_ERROR_TAG("MeshSerializer", "Failed to serialize mesh to file '{}'", SerializePath);
			return;
		}

		FileOut << YamlString;
		FileOut.flush();
		FileOut.close();
	}

	bool LMeshSerializer::TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const
	{
		LK_CORE_TRACE_TAG("MeshSerializer", "TryLoadData: {}", Metadata.FilePath.string());
	#if defined(LK_PLATFORM_WINDOWS)
		const std::filesystem::path FilePath = LProject::GetAssetDirectory() / Metadata.FilePath;
	#elif defined(LK_PLATFORM_LINUX)
		const std::filesystem::path FilePath = LFileSystem::GetRuntimeDir() / LProject::GetAssetDirectory() / Metadata.FilePath;
		LK_CORE_DEBUG_TAG("MeshSerializer", "TryLoadData  Filepath: {}", FilePath);
	#endif
		std::ifstream Stream(FilePath);
		LK_CORE_ASSERT(Stream, "Inputstream failed for: {}", FilePath.string());
		std::stringstream StringStream;
		StringStream << Stream.rdbuf();

		TObjectPtr<LMesh> Mesh;
		if (!DeserializeFromYaml(StringStream.str(), Mesh))
		{
			LK_CORE_ERROR_TAG("MeshSerializer", "Deserialization failed for: {}", FilePath.string());
			return false;
		}

		Mesh->Handle = Metadata.Handle;
		Asset = Mesh;

		return true;
	}

	std::string LMeshSerializer::SerializeToYaml(TObjectPtr<LMesh> Mesh) const
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << "Mesh";
		{
			Out << YAML::BeginMap;
			Out << YAML::Key << "MeshSource";
			Out << YAML::Value << Mesh->GetMeshSource()->Handle;
			Out << YAML::Key << "SubmeshIndices";
			Out << YAML::Flow;
			if (Mesh->GetSubmeshes().size() == Mesh->GetMeshSource()->GetSubmeshes().size())
			{
				Out << YAML::Value << std::vector<uint32_t>();
			}
			else
			{
				Out << YAML::Value << Mesh->GetSubmeshes();
			}
			Out << YAML::EndMap;
		}
		Out << YAML::EndMap;

		return std::string(Out.c_str());
	}

	bool LMeshSerializer::DeserializeFromYaml(const std::string& YamlString, TObjectPtr<LMesh>& TargetMesh) const
	{
		LK_CORE_INFO("YAML:\n{}", YamlString);
		YAML::Node Data = YAML::Load(YamlString);
		if (!Data["Mesh"])
		{
			LK_CORE_ERROR_TAG("MeshSerializer", "The node 'Mesh' is missing from the root node");
			return false;
		}

		const YAML::Node& RootNode = Data["Mesh"];
		if (!RootNode["MeshSource"])
		{
			LK_CORE_ERROR_TAG("MeshSerializer", "The node 'MeshSource' is missing from the root node");
			return false;
		}

		const FAssetHandle MeshSourceHandle = RootNode["MeshSource"].as<uint64_t>();
		TObjectPtr<LMeshSource> MeshSource = LAssetManager::GetAsset<LMeshSource>(MeshSourceHandle);
		if (!MeshSource)
		{
			LK_CORE_ERROR_TAG("MeshSerializer", "Failed to get asset as a LMeshSource with: '{}'", MeshSourceHandle);
			return false; 
		}

		auto SubmeshIndices = RootNode["SubmeshIndices"].as<std::vector<uint32_t>>();
		TargetMesh = TObjectPtr<LMesh>::Create(MeshSource, SubmeshIndices);

		return true;
	}

	/*************************************************************************************************/

	void LStaticMeshSerializer::Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const
	{
		TObjectPtr<LStaticMesh> Mesh = Asset.As<LStaticMesh>();
		const std::string YamlString = SerializeToYaml(Mesh);

		const std::filesystem::path SerializePath = LProject::Current()->GetAssetDirectory() / Metadata.FilePath;
		LK_CORE_TRACE_TAG("StaticMeshSerializer", "Serializing: {}", Metadata.ToString());
		std::ofstream FileOut(SerializePath);
		if (!FileOut.is_open())
		{
			LK_CORE_ERROR_TAG("StaticMeshSerializer", "Failed to serialize LStaticMesh to: '{}'", SerializePath);
			return;
		}

		FileOut << YamlString;
		FileOut.flush();
		FileOut.close();
	}

	bool LStaticMeshSerializer::TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const
	{
		LK_CORE_TRACE_TAG("StaticMeshSerializer", "TryLoadData: {}", Metadata.FilePath.string());
	#if defined(LK_PLATFORM_WINDOWS)
		const std::filesystem::path FilePath = LProject::GetAssetDirectory() / Metadata.FilePath;
	#elif defined(LK_PLATFORM_LINUX)
		const std::filesystem::path FilePath = LFileSystem::GetRuntimeDir() / LProject::GetAssetDirectory() / Metadata.FilePath;
		LK_CORE_DEBUG_TAG("StaticMeshSerializer", "TryLoadData  Filepath: {}", FilePath);
	#endif
		std::ifstream Stream(FilePath);
		LK_CORE_ASSERT(Stream, "Inputstream failed for: {}", FilePath.string());
		std::stringstream StringStream;
		StringStream << Stream.rdbuf();

		TObjectPtr<LStaticMesh> Mesh;
		if (!DeserializeFromYaml(StringStream.str(), Mesh))
		{
			LK_CORE_ERROR_TAG("StaticMeshSerializer", "Deserialization failed for: {}", FilePath.string());
			return false;
		}

		Mesh->Handle = Metadata.Handle;
		Asset = Mesh;
		LK_CORE_TRACE_TAG("StaticMeshSerializer", "Loaded data for: {}", Metadata.ToString());

		return true;
	}

	std::string LStaticMeshSerializer::SerializeToYaml(TObjectPtr<LStaticMesh> Mesh) const
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << "Mesh";
		{
			Out << YAML::BeginMap;
			Out << YAML::Key << "MeshSource";
			Out << YAML::Value << Mesh->GetMeshSource()->Handle;
			Out << YAML::Key << "SubmeshIndices";
			Out << YAML::Flow;
			if (Mesh->GetSubmeshes().size() == Mesh->GetMeshSource()->GetSubmeshes().size())
			{
				Out << YAML::Value << std::vector<uint32_t>();
			}
			else
			{
				Out << YAML::Value << Mesh->GetSubmeshes();
			}
			Out << YAML::EndMap;
		}
		Out << YAML::EndMap;

		return std::string(Out.c_str());
	}

	bool LStaticMeshSerializer::DeserializeFromYaml(const std::string& YamlString, TObjectPtr<LStaticMesh>& TargetMesh) const
	{
		YAML::Node Data = YAML::Load(YamlString);
		if (!Data["Mesh"])
		{
			LK_CORE_ERROR_TAG("StaticMeshSerializer", "The node 'Mesh' is missing from the root node");
			return false;
		}

		const YAML::Node& RootNode = Data["Mesh"];
		if (!RootNode["MeshSource"])
		{
			LK_CORE_ERROR_TAG("StaticMeshSerializer", "The node 'MeshSource' is missing from the root node");
			return false;
		}

		static_assert(std::is_same_v<FAssetHandle::SizeType, uint64_t>);
		const FAssetHandle MeshSourceHandle = RootNode["MeshSource"].as<uint64_t>();
		TObjectPtr<LMeshSource> MeshSource = LAssetManager::GetAsset<LMeshSource>(MeshSourceHandle);
		if (!MeshSource)
		{
			LK_CORE_ERROR_TAG("StaticMeshSerializer", "Failed to get asset as a 'MeshSource' using: '{}'", MeshSourceHandle);
			return false; 
		}

		auto SubmeshIndices = RootNode["SubmeshIndices"].as<std::vector<uint32_t>>();
		TargetMesh = TObjectPtr<LStaticMesh>::Create(MeshSource, SubmeshIndices);

		return true;
	}

}
