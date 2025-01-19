#include "LKpch.h"
#include "LkEngine/Asset/AssetRegistry.h"


namespace LkEngine {
    
	static std::mutex AssetRegistryMutex;

	FAssetMetadata& LAssetRegistry::operator[](const FAssetHandle Handle)
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		return RegistryMap[Handle];
	}

	const FAssetMetadata& LAssetRegistry::operator[](const FAssetHandle Handle) const
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_ASSERT(RegistryMap.find(Handle) != RegistryMap.end());
		return RegistryMap.at(Handle);
	}

	const FAssetMetadata& LAssetRegistry::Get(const FAssetHandle Handle) const
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_ASSERT(RegistryMap.find(Handle) != RegistryMap.end());
		return RegistryMap.at(Handle);
	}

	FAssetMetadata& LAssetRegistry::Get(const FAssetHandle Handle)
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		return RegistryMap[Handle];
	}

	bool LAssetRegistry::Contains(const FAssetHandle Handle) const
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		return RegistryMap.find(Handle) != RegistryMap.end();
	}

	std::size_t LAssetRegistry::Remove(const FAssetHandle Handle)
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		return RegistryMap.erase(Handle);
	}

	void LAssetRegistry::Clear()
	{
		std::scoped_lock<std::mutex> ScopedLock(AssetRegistryMutex);
		LK_CORE_DEBUG_TAG("AssetRegistry", "Clearing registry");
		RegistryMap.clear();
	}

}
