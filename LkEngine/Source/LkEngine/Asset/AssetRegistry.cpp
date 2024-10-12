#include "LKpch.h"
#include "LkEngine/Asset/AssetRegistry.h"


namespace LkEngine {
    
	static std::mutex s_AssetRegistryMutex;

	FAssetMetadata& LAssetRegistry::operator[](const FAssetHandle AssetHandle)
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Retrieving AssetHandle {}", AssetHandle);
		return m_AssetRegistry[AssetHandle];
	}

	const FAssetMetadata& LAssetRegistry::Get(const FAssetHandle AssetHandle) const
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);
		LK_ASSERT(m_AssetRegistry.find(AssetHandle) != m_AssetRegistry.end());

		return m_AssetRegistry.at(AssetHandle);
	}

	FAssetMetadata& LAssetRegistry::Get(const FAssetHandle AssetHandle)
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		return m_AssetRegistry[AssetHandle];
	}

	bool LAssetRegistry::Contains(const FAssetHandle AssetHandle) const
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		return m_AssetRegistry.find(AssetHandle) != m_AssetRegistry.end();
	}

	size_t LAssetRegistry::Remove(const FAssetHandle AssetHandle)
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Removing AssetHandle", AssetHandle);
		return m_AssetRegistry.erase(AssetHandle);
	}

	void LAssetRegistry::Clear()
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Clearing registry");
		m_AssetRegistry.clear();
	}



}