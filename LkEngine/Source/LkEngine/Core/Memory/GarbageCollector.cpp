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
		GLiveObjectMap.clear();

		LK_CORE_DEBUG_TAG("GarbageCollector", "Adding Application as first tracked LObject");
		GLiveObjectMap.insert(
			{ LApplication::Get()->GetHandle(), TObjectPtr<LObject>(LApplication::Get()) }
		);

		LK_CORE_DEBUG_TAG("GarbageCollector", "Initializiation complete");
	}

}
