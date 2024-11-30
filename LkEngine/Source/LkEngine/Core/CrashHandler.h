/******************************************************************
 * CrashHandler
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	class LCrashHandler;
	class LApplication;

	class ICrashHandler
	{
	public:
		virtual ~ICrashHandler() = default;

	protected:
		virtual std::string GenerateApplicationCrashDump() = 0;
		virtual void LogCrashInformation(std::string_view CrashInformation) = 0;
		virtual std::string CaptureBackTraceOnStack() = 0;

		friend class LCrashHandler;
	};

	class LCrashHandler
	{
	public:
		static void AttachInstance(LApplication* ApplicationRef);

	private:
		static void SignalHandler(const int Signal);

	private:
		/** CrashHandler reference, type depends on platform. */
		inline static TUniquePtr<ICrashHandler> CrashHandler = nullptr;

		inline static bool bIsShuttingDown = false;
	};

}
