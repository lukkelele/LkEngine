#pragma once

#include "LkEngine/Asset/Asset.h"
#include "LkEngine/Asset/AssetRegistry.h"
#include "LkEngine/Asset/AssetSerializer.h"


namespace LkEngine {

	class LAssetImporter
	{
	public:
		static void Initialize();
		static void Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset);
		static void Serialize(const TObjectPtr<LAsset>& Asset);
		static bool TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset);

	private:
		static std::unordered_map<EAssetType, std::unique_ptr<IAssetSerializer>> SerializerMap;

		friend struct FAssetImporterValidator;
	};

	struct FAssetImporterValidator
	{
		static void ValidateSerializer(const FAssetMetadata& Metadata)
		{
			if (!LAssetImporter::SerializerMap.contains(Metadata.Type))
			{
				LK_CORE_ASSERT(false, "No asset importer exists for: {}", Enum::ToString(Metadata.Type));
			}
		}
	};

}
