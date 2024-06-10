#include "LKpch.h"
#include "CrashHandler.h"

#ifdef LK_PLATFORM_WINDOWS
#include "LkEngine/Platform/Windows/WindowsCrashHandler.h"
#elif defined(LK_PLATFORM_LINUX)
#include "LkEngine/Platform/Linux/LinuxCrashHandler.h"
#endif


namespace LkEngine {

	TUniquePtr<LCrashHandler> LCrashHandler::Create(LApplication* ApplicationRef)
	{
	#ifdef LK_PLATFORM_WINDOWS
		return MakeUnique<LWindowsCrashHandler>(ApplicationRef);
	#elif defined(LK_PLATFORM_LINUX)
		LK_CORE_ASSERT(false, "Linux not supported as of yet");
		return nullptr;
	#endif
	}

}