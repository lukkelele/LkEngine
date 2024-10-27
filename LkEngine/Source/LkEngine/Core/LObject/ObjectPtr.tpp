#pragma once


namespace LkEngine {

	void TObjectPtr_Internal::AddToLiveReferences(void* InObject)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(InObject, "AddToLiveReferences failed, object is nullptr");

		if (auto Iter = LiveReferences.insert(InObject); Iter.second == true)
		{
		#if LK_DEBUG_LOG_LIVE_REFERENCES 1
			const LClass* ClassInfo = static_cast<LObject*>(InObject)->GetClass();
			LK_CORE_DEBUG_TAG("ObjectPtr", "Adding reference to \"{}\", total: {}", 
							  ClassInfo->GetName(), static_cast<LObject*>(InObject)->GetReferenceCount());
		#endif
		}
	}

	void TObjectPtr_Internal::RemoveFromLiveReferences(void* InObject)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(InObject, "Failed to remove instance from live references, passed instance is nullptr");

		if (LiveReferences.find(InObject) != LiveReferences.end())
		{
			LiveReferences.erase(InObject);
		}
		else
		{
		#if LK_DEBUG_LOG_LIVE_REFERENCES 1
			LK_CORE_DEBUG_TAG("ObjectPtr", "Was not able to remove {} from live references with {} active references ", 
							  static_cast<LObject*>(InObject)->ClassName(), 
							  static_cast<LObject*>(InObject)->GetReferenceCount());
		#endif
		}
	}

	bool TObjectPtr_Internal::IsLive(void* InObject)
	{
		LK_CORE_ASSERT(InObject);
		return (LiveReferences.find(InObject) != LiveReferences.end());
	}

	template<typename T>
	template<typename ...TArgs>
	TObjectPtr<T> TObjectPtr<T>::Create(TArgs&&... Args)
	{
		return TObjectPtr<T>(new T(std::forward<TArgs>(Args)...));
	}

}