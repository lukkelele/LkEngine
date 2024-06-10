#pragma once
#ifdef LK_PLATFORM_WINDOWS

#include "LkEngine/Core/CrashHandler.h"


namespace LkEngine {

	class LApplication;

	class LWindowsCrashHandler : public LCrashHandler
	{
	public:
		LWindowsCrashHandler(LApplication* ApplicationRef);
		LWindowsCrashHandler() = delete;
		~LWindowsCrashHandler();

		virtual LString GenerateApplicationCrashDump() override;

		virtual void LogCrashInformation(LStringView CrashInformation) override;

	private:
		/* Reference to application instance. */
		TSharedPtr<LApplication> Application{};
	};

}


#endif /* LK_PLATFORM_WINDOWS */