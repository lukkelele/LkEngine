#pragma once
#ifdef LK_PLATFORM_WINDOWS

#include "LkEngine/Core/CrashHandler.h"


namespace LkEngine {

	class LApplication;

	class LWindowsCrashHandler : public ICrashHandler
	{
	public:
		LWindowsCrashHandler(LApplication* ApplicationRef);
		LWindowsCrashHandler() = delete;
		~LWindowsCrashHandler();

		virtual std::string GenerateApplicationCrashDump() override;
		virtual void LogCrashInformation(std::string_view CrashInformation) override;
		virtual std::string CaptureBackTraceOnStack() override;

	private:
		/* Reference to application instance. */
		std::shared_ptr<LApplication> Application{};
	};

}


#endif /* LK_PLATFORM_WINDOWS */
