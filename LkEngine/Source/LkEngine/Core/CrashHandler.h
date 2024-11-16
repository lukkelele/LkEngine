/******************************************************************
 * CrashHandler
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/String.h"


namespace LkEngine {

	class LCrashHandler;
	class LApplication;

	class ICrashHandler
	{
	public:
		virtual ~ICrashHandler() = default;

	protected:
		virtual LString GenerateApplicationCrashDump() = 0;
		virtual void LogCrashInformation(LStringView CrashInformation) = 0;
		virtual LString CaptureBackTraceOnStack() = 0;

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
