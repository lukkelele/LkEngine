#pragma once


namespace LkEngine {

	void TObjectPtr_Internal::AddToLiveReferences(void* Instance)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(Instance);
	#if 0 // REQUIRE LOG INITIALIZATION IN EARLIER STAGES OF STARTUP
		LK_CORE_DEBUG_TAG("LObjectPtr", "Adding to live references: {}", static_cast<LObject*>(Instance)->GetStaticClass());
	#endif
		LiveReferences.insert(Instance);
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
	#if 0 // DISABLED RIGHT NOW
			LK_CORE_DEBUG_TAG("TObjectPtr", "Was not able to remove {} from live references with {} active references ", 
							  static_cast<LObject*>(Instance)->GetStaticClass(), static_cast<LObject*>(Instance)->GetReferenceCount());
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
	#if LK_USE_GC // Garbage Collector not used for now
		TObjectPtr<T> NewObject(new T(std::forward<TArgs>(Args)...));
		// TODO: Handle initialization of object - assign to garbage collector.
		return NewObject;
	#endif

		return TObjectPtr<T>(new T(std::forward<TArgs>(Args)...));
	}

}