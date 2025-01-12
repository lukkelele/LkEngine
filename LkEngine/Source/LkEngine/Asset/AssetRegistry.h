#pragma once

#include <filesystem>

#include "LkEngine/Asset/Asset.h"


namespace LkEngine {

	class LAssetRegistry
	{
	public:
		FAssetMetadata& operator[](const LUUID AssetHandle);
		FAssetMetadata& Get(const LUUID AssetHandle);
		const FAssetMetadata& Get(const LUUID AssetHandle) const;

		size_t Count() const { return m_AssetRegistry.size(); }
		bool Contains(const LUUID AssetHandle) const;
		size_t Remove(const LUUID AssetHandle);
		void Clear();

		auto begin() { return m_AssetRegistry.begin(); }
		auto end() { return m_AssetRegistry.end(); }
		auto begin() const { return m_AssetRegistry.cbegin(); }
		auto end() const { return m_AssetRegistry.cend(); }

	private:
		std::unordered_map<LUUID, FAssetMetadata> m_AssetRegistry;
	};

}