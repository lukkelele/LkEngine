#include "LKpch.h"
#include "MeshSerializer.h"

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
		const std::filesystem::path FilePath = LProject::GetRuntimeAssetManager()->GetFileSystemPath(Metadata);
		LAssimpMeshImporter Importer(FilePath.string());

		TObjectPtr<LMeshSource> MeshSource = Importer.ImportToMeshSource();
		if (!MeshSource)
		{
			return false;
		}

		Asset = MeshSource;
		Asset->Handle = Metadata.Handle;

		return true;
	}

	/*************************************************************************************************/

	void LMeshSerializer::Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const
	{
	}

	bool LMeshSerializer::TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const
	{
		return false;
	}

	std::string LMeshSerializer::SerializeToYaml(TObjectPtr<LMesh> Mesh) const
	{
		return std::string();
	}

	bool LMeshSerializer::DeserializeFromYaml(const std::string& YamlString, TObjectPtr<LMesh>& TargetMesh) const
	{
		return false;
	}

}
