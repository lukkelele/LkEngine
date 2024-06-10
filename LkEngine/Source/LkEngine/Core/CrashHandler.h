#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/String.h"


namespace LkEngine {

	class LApplication;

	class LCrashHandler
	{
	public:
		virtual ~LCrashHandler() = default;

		virtual LString GenerateApplicationCrashDump() = 0;
		virtual void LogCrashInformation(LStringView CrashInformation) = 0;

		static TUniquePtr<LCrashHandler> Create(LApplication* ApplicationRef);

	protected:
		inline static LCrashHandler* Instance = nullptr;
	};

}