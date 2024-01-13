#include "LKpch.h"

#include <unordered_set>

namespace LkEngine {

	static std::unordered_set<void*> m_LiveReferences;
	static std::mutex m_LiveReferenceMutex;

	namespace RefUtils {

		void AddToLiveReferences(void* instance)
		{
			std::scoped_lock<std::mutex> lock(m_LiveReferenceMutex);
			LK_ASSERT(instance != nullptr);
			m_LiveReferences.insert(instance);
		}

		void RemoveFromLiveReferences(void* instance)
		{
			std::scoped_lock<std::mutex> lock(m_LiveReferenceMutex);
			LK_ASSERT(instance != nullptr);
			LK_ASSERT(m_LiveReferences.find(instance) != m_LiveReferences.end());
			m_LiveReferences.erase(instance);
		}

		bool IsLive(void* instance)
		{
			LK_ASSERT(instance != nullptr);
			return m_LiveReferences.find(instance) != m_LiveReferences.end();
		}
	}


}
