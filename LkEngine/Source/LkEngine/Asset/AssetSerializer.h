#pragma once

#include "LkEngine/Asset/Asset.h"
#include "LkEngine/Asset/AssetRegistry.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	struct FAssetSerializationInfo
	{
		uint64_t Offset = 0;
		uint64_t Size = 0;
	};

	class IAssetSerializer
	{
	public:
		virtual void Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const = 0;
		virtual bool TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const = 0;
	};

}
