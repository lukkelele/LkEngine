#include "LKpch.h"
//#ifdef LK_PLATFORM_WINDOWS
#include "WindowsCrashHandler.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Core/Encoder.h"

#include <Windows.h>
#include <DbgHelp.h>


namespace LkEngine {

	LWindowsCrashHandler::LWindowsCrashHandler(LApplication* ApplicationRef)
		: Application(TSharedPtr<LApplication>(ApplicationRef))
	{
        Instance = this;

        static auto WindowsCrashFunc = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
            {
                LK_CORE_WARN("<< ENTERED EXCEPTION HANDLER>> \n\n");

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
                oss << *LApplication::Get()->GenerateCrashDump();

                LString CrashInfo = oss.str();
                std::string base64CrashInfo = LEncoder::Encode<EncodingFormat::Base64>(
                    reinterpret_cast<const unsigned char*>(CrashInfo.CStr()), CrashInfo.Length()
                );

                Instance->LogCrashInformation(base64CrashInfo);

                return EXCEPTION_EXECUTE_HANDLER;
            };

        SetUnhandledExceptionFilter(WindowsCrashFunc);
	}

	LWindowsCrashHandler::~LWindowsCrashHandler()
	{
	}

    LString LWindowsCrashHandler::GenerateApplicationCrashDump()
    {
        return "APPLCIATION_CRASHDUMP";
    }

    void LWindowsCrashHandler::LogCrashInformation(LStringView CrashInformation)
    {
        LK_CORE_WARN("\n\n ***** Crash Information *****\n        {}\n *****************************\n\n", CrashInformation);
    }

}


//#endif /* LK_PLATFORM_WINDOWS */
