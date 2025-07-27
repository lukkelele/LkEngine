#pragma once

#include <set>

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

namespace LkEngine {

	extern std::unordered_map<FObjectHandle, TObjectPtr<LObject>> GLiveObjectMap;

	/**
	 * @class LGarbageCollector
	 * @warning Unused.
	 */
	class LGarbageCollector
	{
	public:
		LGarbageCollector() = default;
		~LGarbageCollector() = default;

		static LGarbageCollector& Get();

		void Initialize();
	};

}
