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
		LK_CORE_INFO_TAG("MeshSourceSerializer", "TryLoadData -> {}", Metadata.FilePath.string());
		const std::filesystem::path FilePath = LProject::GetRuntimeAssetManager()->GetFileSystemPath(Metadata);
		LAssimpMeshImporter Importer(FilePath.string());

		TObjectPtr<LMeshSource> MeshSource = Importer.ImportToMeshSource();
		if (!MeshSource)
		{
			LK_CORE_TRACE("Failed to import MeshSource from: {}", FilePath.string());
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
		LK_CORE_TRACE_TAG("MeshSerializer", "Serializing to {}", SerializePath.string());
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
		const std::filesystem::path FilePath = LProject::GetAssetDirectory() / Metadata.FilePath;
		std::ifstream Stream(FilePath);
		LK_CORE_ASSERT(Stream, "Inputstream failed for: {}", FilePath.string());
		std::stringstream StringStream;
		StringStream << Stream.rdbuf();

		TObjectPtr<LMesh> Mesh;
		if (!DeserializeFromYaml(StringStream.str(), Mesh))
		{
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
		YAML::Node Data = YAML::Load(YamlString);
		if (!Data["Mesh"])
		{
			return false;
		}

		const YAML::Node& RootNode = Data["Mesh"];
		if (!RootNode["MeshSource"])
		{
			return false;
		}

		const LUUID MeshSourceHandle = RootNode["MeshSource"].as<uint64_t>();
		TObjectPtr<LMeshSource> MeshSource = LAssetManager::GetAsset<LMeshSource>(MeshSourceHandle);
		if (!MeshSource)
		{
			return false; 
		}

		auto SubmeshIndices = RootNode["SubmeshIndices"].as<std::vector<uint32_t>>();
		TargetMesh = TObjectPtr<LMesh>::Create(MeshSource, SubmeshIndices);

		return true;
	}

}
