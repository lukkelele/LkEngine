#include "LKpch.h"
#include "GarbageCollector.h"

#include "LkEngine/Core/Application.h"

namespace LkEngine {

	std::unordered_map<FObjectHandle, TObjectPtr<LObject>> GLiveObjectMap{};

	LGarbageCollector& LGarbageCollector::Get()
	{
		static LGarbageCollector GarbageCollector;
		return GarbageCollector;
	}

	void LGarbageCollector::Initialize()
	{
		LK_CORE_DEBUG_TAG("GarbageCollector", "Initializing");
		GLiveObjectMap.clear();

		LK_CORE_TRACE_TAG("GarbageCollector", "Adding LApplication as first tracked LObject");
		GLiveObjectMap.insert(
			{ LApplication::Get().GetObjectHandle(), TObjectPtr<LObject>(LApplication::Get()) }
		);
	}

}
