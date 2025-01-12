#include "LKpch.h"
#include "LkEngine/Asset/AssetRegistry.h"


namespace LkEngine {
    
	static std::mutex AssetRegistryMutex;

	FAssetMetadata& LAssetRegistry::operator[](const LUUID AssetHandle)
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_ASSET_TRACE_TAG("AssetRegistry", "Retrieving asset handle: {}", AssetHandle);
		return m_AssetRegistry[AssetHandle];
	}

	const FAssetMetadata& LAssetRegistry::Get(const LUUID AssetHandle) const
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_ASSERT(m_AssetRegistry.find(AssetHandle) != m_AssetRegistry.end());
		return m_AssetRegistry.at(AssetHandle);
	}

	FAssetMetadata& LAssetRegistry::Get(const LUUID AssetHandle)
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		return m_AssetRegistry[AssetHandle];
	}

	bool LAssetRegistry::Contains(const LUUID AssetHandle) const
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		return m_AssetRegistry.find(AssetHandle) != m_AssetRegistry.end();
	}

	size_t LAssetRegistry::Remove(const LUUID AssetHandle)
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_ASSET_TRACE_TAG("AssetRegistry", "Removing AssetHandle", AssetHandle);
		return m_AssetRegistry.erase(AssetHandle);
	}

	void LAssetRegistry::Clear()
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_ASSET_DEBUG_TAG("AssetRegistry", "Clearing registry");
		m_AssetRegistry.clear();
	}

}