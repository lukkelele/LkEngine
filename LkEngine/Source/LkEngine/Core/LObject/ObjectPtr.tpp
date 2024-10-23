#pragma once


namespace LkEngine {

	void TObjectPtr_Internal::AddToLiveReferences(void* Instance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(Instance);

		if (!LiveReferences.contains(Instance))
		{
			LiveReferences.insert(Instance);
			LK_CORE_DEBUG_TAG("ObjectPtr", "Adding to live references: {}", static_cast<LObject*>(Instance)->StaticClass());
		}
	}

	void TObjectPtr_Internal::RemoveFromLiveReferences(void* Instance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(Instance, "Failed to remove instance from live references, passed instance is nullptr");

		if (LiveReferences.find(Instance) != LiveReferences.end())
		{
			LiveReferences.erase(Instance);
		}
		else
		{
		#if 1
			LK_CORE_DEBUG_TAG("ObjectPtr", "Was not able to remove {} from live references with {} active references ", 
							  static_cast<LObject*>(Instance)->StaticClass(), 
							  static_cast<LObject*>(Instance)->GetReferenceCount());
		#endif
		}
	}

	bool TObjectPtr_Internal::IsLive(void* Instance)
	{
		LK_CORE_ASSERT(Instance);
		return (LiveReferences.find(Instance) != LiveReferences.end());
	}

	template<typename T>
	template<typename ...TArgs>
	TObjectPtr<T> TObjectPtr<T>::Create(TArgs&&... Args)
	{
		return TObjectPtr<T>(new T(std::forward<TArgs>(Args)...));
	}

}