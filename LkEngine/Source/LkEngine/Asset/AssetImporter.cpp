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
		SerializerMap[EAssetType::StaticMesh] = std::make_unique<LStaticMeshSerializer>();
		LK_CORE_DEBUG_TAG("AssetImporter", "Serializers: {}", SerializerMap.size());
	}

	void LAssetImporter::Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset)
	{
		FAssetImporterValidator::ValidateSerializer(Metadata);
		SerializerMap[Asset->GetAssetType()]->Serialize(Metadata, Asset);
	}

	void LAssetImporter::Serialize(const TObjectPtr<LAsset>& Asset)
	{
		const FAssetMetadata& Metadata = LProject::GetRuntimeAssetManager()->GetMetadata(Asset->Handle);
		Serialize(Metadata, Asset);
	}

	bool LAssetImporter::TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset)
	{
		FAssetImporterValidator::ValidateSerializer(Metadata);
		LK_CORE_TRACE_TAG("AssetImporter", "TryLoadData: {}", Metadata.ToString());
		return SerializerMap[Metadata.Type]->TryLoadData(Metadata, Asset);
	}

}
