#include "LKpch.h"
#include "CrashHandler.h"

#include <csignal>
#include <signal.h>

#ifdef LK_PLATFORM_WINDOWS
#include "LkEngine/Platform/Windows/WindowsCrashHandler.h"
#elif defined(LK_PLATFORM_LINUX)
#include "LkEngine/Platform/Linux/LinuxCrashHandler.h"
#endif

static std::unordered_map<int, std::string> SignalToStringMap =
{
	{ SIGINT,  "SIGINT"  },
	{ SIGABRT, "SIGABRT" },
	{ SIGSEGV, "SIGSEGV" },
};

namespace LkEngine {

	//TUniquePtr<LCrashHandler> LCrashHandler::AttachInstance(LApplication* ApplicationRef)
	void LCrashHandler::AttachInstance(LApplication* ApplicationRef)
	{
		LK_CORE_VERIFY(CrashHandler == nullptr, "CrashHandler already assigned");
		LK_CORE_DEBUG_TAG("CrashHandler", "Attaching application instance");

	#ifdef LK_PLATFORM_WINDOWS
		CrashHandler = MakeUnique<LWindowsCrashHandler>(ApplicationRef);
	#elif defined(LK_PLATFORM_LINUX)
		LK_CORE_ASSERT(false, "Linux not supported as of yet");
		return;
	#endif

		// Attach signals.
		std::signal(SIGINT,  &LCrashHandler::SignalHandler);
		std::signal(SIGSEGV, &LCrashHandler::SignalHandler);
		std::signal(SIGABRT, &LCrashHandler::SignalHandler);
	}

	void LCrashHandler::SignalHandler(const int Signal)
	{
		LK_CORE_ASSERT(SignalToStringMap.contains(Signal), "Signal does not exist in SignalToStringMap!");
		if (CrashHandler && !bIsShuttingDown)
		{
			bIsShuttingDown = true;
			LK_CORE_DEBUG_TAG("CrashHandler", "[SignalHandler] Handling signal: {} ({})", SignalToStringMap[Signal], Signal);

			const std::string InfoDump = CrashHandler->GenerateApplicationCrashDump();
			//CrashHandler->LogCrashInformation(InfoDump);

			CrashHandler.release();
		}
	}

}