#include "LKpch.h"
#include "LkEngine/Asset/AssetRegistry.h"


namespace LkEngine {
    
	static std::mutex s_AssetRegistryMutex;

	AssetMetadata& AssetRegistry::operator[](const AssetHandle handle)
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Retrieving handle {}", handle);
		return m_AssetRegistry[handle];
	}

	const AssetMetadata& AssetRegistry::Get(const AssetHandle handle) const
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LK_ASSERT(m_AssetRegistry.find(handle) != m_AssetRegistry.end());
		LOG_ASSET("Retrieving const handle {}", handle);
		return m_AssetRegistry.at(handle);
	}

	AssetMetadata& AssetRegistry::Get(const AssetHandle handle)
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Retrieving handle {}", handle);
		return m_AssetRegistry[handle];
	}

	bool AssetRegistry::Contains(const AssetHandle handle) const
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Contains handle {}", handle);
		return m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	size_t AssetRegistry::Remove(const AssetHandle handle)
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Removing handle", handle);
		return m_AssetRegistry.erase(handle);
	}

	void AssetRegistry::Clear()
	{
		std::scoped_lock<std::mutex> lock(s_AssetRegistryMutex);

		LOG_ASSET("Clearing registry");
		m_AssetRegistry.clear();
	}



}