#pragma once

#include <filesystem>

#include "LkEngine/Asset/Asset.h"


namespace LkEngine {

	class LAssetRegistry
	{
	public:
		FAssetMetadata& operator[](const FAssetHandle Handle);
		const FAssetMetadata& operator[](const FAssetHandle Handle) const;
		FAssetMetadata& Get(const FAssetHandle Handle);
		const FAssetMetadata& Get(const FAssetHandle Handle) const;

		std::size_t Count() const { return RegistryMap.size(); }
		bool Contains(const FAssetHandle Handle) const;
		std::size_t Remove(const FAssetHandle Handle);
		void Clear();

		auto begin() { return RegistryMap.begin(); }
		auto end() { return RegistryMap.end(); }
		auto begin() const { return RegistryMap.cbegin(); }
		auto end() const { return RegistryMap.cend(); }

	private:
		std::unordered_map<LUUID, FAssetMetadata> RegistryMap;
	};

}
