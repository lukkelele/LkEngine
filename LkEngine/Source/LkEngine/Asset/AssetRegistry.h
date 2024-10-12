#pragma once

#include <filesystem>

#include "LkEngine/Asset/Asset.h"


namespace LkEngine {

	class LAssetRegistry
	{
	public:
		FAssetMetadata& operator[](const FAssetHandle AssetHandle);
		FAssetMetadata& Get(const FAssetHandle AssetHandle);
		const FAssetMetadata& Get(const FAssetHandle AssetHandle) const;

		size_t Count() const { return m_AssetRegistry.size(); }
		bool Contains(const FAssetHandle AssetHandle) const;
		size_t Remove(const FAssetHandle AssetHandle);
		void Clear();

		auto begin() { return m_AssetRegistry.begin(); }
		auto end() { return m_AssetRegistry.end(); }
		auto begin() const { return m_AssetRegistry.cbegin(); }
		auto end() const { return m_AssetRegistry.cend(); }

	private:
		std::unordered_map<FAssetHandle, FAssetMetadata> m_AssetRegistry;
	};

}