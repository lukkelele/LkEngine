#include "LKpch.h"
#include "WindowsCrashHandler.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Core/Encoder.h"

#include <Windows.h>
#include <DbgHelp.h>


namespace LkEngine {

    static void* WindowsCrashFunctionPtr = nullptr;
    static std::string WindowsExceptionStringBuffer{};

	LWindowsCrashHandler::LWindowsCrashHandler(LApplication* ApplicationRef)
		: Application(std::shared_ptr<LApplication>(ApplicationRef))
	{
        static auto* Instance = this;

        static auto WindowsCrashFunc = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
        {
            LK_CORE_WARN_TAG("WindowsCrashHandler", "ENTERED EXCEPTION HANDLER");

            std::ostringstream oss;
            oss << "Exception Code: " << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << "\n";
            oss << "Exception Address: " << ExceptionInfo->ExceptionRecord->ExceptionAddress << "\n";

            // Capture stack trace
            const int MaxFrames = 128;
            void* Callstack[MaxFrames];
            USHORT frames = CaptureStackBackTrace(0, MaxFrames, Callstack, nullptr);
            oss << "Stack trace:\n";
            for (USHORT i = 0; i < frames; ++i)
            {
                oss << Callstack[i] << "\n";
            }

            oss << "\nApplication State:\n";
            LK_CORE_DEBUG_TAG("WindowsCrashHandler", "{}", oss.str());
            //oss << *LApplication::Get()->GenerateCrashDump();

            std::string CrashInfo = oss.str();
            std::string base64CrashInfo = LEncoder::Encode<EEncodingFormat::Base64>(
                reinterpret_cast<const unsigned char*>(CrashInfo.c_str()), CrashInfo.length()
            );

            WindowsExceptionStringBuffer += base64CrashInfo;

            return EXCEPTION_EXECUTE_HANDLER;
        };

        WindowsCrashFunctionPtr = &WindowsCrashFunc;
	}

	LWindowsCrashHandler::~LWindowsCrashHandler()
	{
        SetUnhandledExceptionFilter(nullptr);
	}

    std::string LWindowsCrashHandler::GenerateApplicationCrashDump()
    {
        /* Disabled for now. */
#if 0
        std::string DumpInfo = "APPLICATION_CRASH";
        if (!WindowsExceptionStringBuffer.empty())
        {
            DumpInfo += std::string::Format("\n{}", WindowsExceptionStringBuffer);
        }
        // Include stack backtrace.
        else
        {
            DumpInfo += std::string::Format("\n{}", *CaptureBackTraceOnStack());
        }
        //LK_CORE_DEBUG_TAG("WindowsCrashHandler", "Generated application crashdump, returning:\n\"\"\"\n{}\n\"\"\"", *DumpInfo);

        return DumpInfo;
#endif
        return "";
    }

    void LWindowsCrashHandler::LogCrashInformation(std::string_view CrashInformation)
    {
        LK_CORE_WARN("\n\n ***** Crash Information *****\n    {}\n *****************************\n\n", CrashInformation);
    }

    std::string LWindowsCrashHandler::CaptureBackTraceOnStack()
    {
		std::ostringstream oss;

        /* Disabled for now. */
    #if 0
		/* Capture stack trace. */
		const int MaxFrames = 128;
		void* Callstack[MaxFrames];
		USHORT frames = CaptureStackBackTrace(0, MaxFrames, Callstack, nullptr);
		oss << "\n[SECTION] Stack Trace:\n";
		for (USHORT i = 0; i < frames; ++i)
		{
            if (i < 10)
            {
			    oss << *std::string::Format(" {}.  ", i) << Callstack[i] << "\n";
            }
            else if (i >= 10)
            {
			    oss << *std::string::Format(" {}. ", i) << Callstack[i] << "\n";
            }
		}
    #endif

        return oss.str();
    }

}
