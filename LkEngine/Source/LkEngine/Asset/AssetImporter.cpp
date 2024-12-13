#include "LKpch.h"
#include "AssetImporter.h"

#include "MeshSerializer.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	std::unordered_map<EAssetType, std::unique_ptr<IAssetSerializer>> LAssetImporter::SerializerMap;

	void LAssetImporter::Initialize()
	{
		SerializerMap.clear();
		SerializerMap[EAssetType::MeshSource] = std::make_unique<LMeshSourceSerializer>();
		SerializerMap[EAssetType::Mesh] = std::make_unique<LMeshSerializer>();
	}

	void LAssetImporter::Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset)
	{
		if (!SerializerMap.contains(Metadata.Type))
		{
			LK_CORE_WARN_TAG("AssetImporter", "No importer exists for {}", Enum::ToString(Metadata.Type));
			return;
		}

		SerializerMap[Asset->GetAssetType()]->Serialize(Metadata, Asset);
	}

	void LAssetImporter::Serialize(const TObjectPtr<LAsset>& Asset)
	{
		const FAssetMetadata& Metadata = LProject::GetRuntimeAssetManager()->GetMetadata(Asset->Handle);
		Serialize(Metadata, Asset);
	}

	bool LAssetImporter::TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset)
	{
		if (!SerializerMap.contains(Metadata.Type))
		{
			LK_CORE_WARN_TAG("AssetImporter", "No importer exists for {}", Enum::ToString(Metadata.Type));
			return false;
		}

		return SerializerMap[Asset->GetAssetType()]->TryLoadData(Metadata, Asset);
	}

}
