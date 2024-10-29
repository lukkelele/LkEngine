#include "LKpch.h"
#include "ObjectPtr.h"


namespace LkEngine {

	namespace TObjectPtr_Internal
	{
		std::unordered_set<void*> LiveReferences;
		std::mutex LiveReferenceMutex;
	}

	void TObjectPtr_Internal::AddToLiveReferences(void* InObject)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		LK_CORE_ASSERT(InObject, "AddToLiveReferences failed, object is nullptr");
		LObject* Object = static_cast<LObject*>(InObject);

		if (auto Iter = LiveReferences.insert(InObject); Iter.second == true)
		{
		#if (LK_DEBUG_LOG_LIVE_REFERENCES == 1)
			LClass* ClassInfo = const_cast<LClass*>(Object->GetClass());
			LK_CORE_DEBUG_TAG("ObjectPtr", "Adding reference to \"{}\" ({}), total: {}", 
							  ClassInfo->GetName(), reinterpret_cast<uint64_t>(InObject), Object->GetReferenceCount());
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
		#if (LK_DEBUG_LOG_LIVE_REFERENCES == 1)
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

	int TObjectPtr_Internal::GetLiveReferences(std::unordered_set<void*>& InSet)
	{
		std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
		InSet.clear();
		InSet = LiveReferences;

		return static_cast<int>(InSet.size());
	}

	int TObjectPtr_Internal::GetLiveObjects(std::vector<TObjectPtr<LObject>>& ObjectArray, 
											const bool FilterByStaticClass)
	{
		ObjectArray.clear();
		std::unordered_set<void*> CurrentLiveReferences;
		{
			std::scoped_lock<std::mutex> ScopedLock(LiveReferenceMutex);
			CurrentLiveReferences = LiveReferences;
		}

		auto ContainsObjectClass = [&ObjectArray](const LObject* InObject) -> bool
		{
			if (InObject)
			{
				return (std::find_if(ObjectArray.begin(), ObjectArray.end(), [&](const TObjectPtr<LObject>& Object)
				{
					return (InObject->GetClass() == Object->GetClass());
				})) != ObjectArray.end();
			}
			return false;
		};

		int Index = 0;
		for (void* LiveReference : CurrentLiveReferences)
		{
			LK_ASSERT(LiveReference, "Object is nullptr"); /* Should not be needed, but is more of a failsafe. */
			LObject* LiveObject = static_cast<LObject*>(LiveReference);

			if (FilterByStaticClass)
			{
				if (!ContainsObjectClass(LiveObject))
				{
					ObjectArray.push_back(TObjectPtr<LObject>(LiveObject));
				}
			}
			else
			{
				ObjectArray.push_back(TObjectPtr<LObject>(LiveObject));
			}

			Index++;
		}

		return static_cast<int>(ObjectArray.size());
	}

}
