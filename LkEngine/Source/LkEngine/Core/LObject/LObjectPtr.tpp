#pragma once


namespace LkEngine {

	void TObjectPtr_Internal::AddToLiveReferences(void* Instance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(Instance);
		//LK_CORE_DEBUG_TAG("LObjectPtr", "Adding to live references: {}", static_cast<LObject*>(Instance)->GetStaticClass());
		//std::printf("[LObjectPtr] Adding to live references: %s\n", static_cast<LObject*>(Instance)->GetStaticClass().c_str());
		LiveReferences.insert(Instance);
	}

	void TObjectPtr_Internal::RemoveFromLiveReferences(void* Instance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(Instance, "Failed to remove instance from live references, passed instance is nullptr");
		//LK_CORE_DEBUG_TAG("LObjectPtr", "Removing from live references: {}", static_cast<LObject*>(Instance)->GetStaticClass());

		//LK_CORE_ASSERT(LiveReferences.find(Instance) != LiveReferences.end());
		if (LiveReferences.find(Instance) != LiveReferences.end())
		{
			LiveReferences.erase(Instance);
		}
		else
		{
			LK_CORE_DEBUG_TAG("TObjectPtr", "Was not able to remove {} from live references with {} active references ", 
							  static_cast<LObject*>(Instance)->GetStaticClass(), static_cast<LObject*>(Instance)->GetReferenceCount());
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
	#if 0
		TObjectPtr<T> NewObject(new T(std::forward<TArgs>(Args)...));
		// TODO: Handle initialization of object - assign to garbage collector.
		return NewObject;
	#endif

		return TObjectPtr<T>(new T(std::forward<TArgs>(Args)...));
	}

}