#include "LKpch.h"
#if defined(LK_PLATFORM_WINDOWS)
#include "WindowsCrashHandler.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Core/Encoder.h"

#include <Windows.h>
#include <DbgHelp.h>

#define LK_CRASH_INCLUDE_STACKBACKTRACE 0

namespace LkEngine {

    static void* WindowsCrashFunctionPtr = nullptr;
    static std::string WindowsExceptionStringBuffer{};

	LWindowsCrashHandler::LWindowsCrashHandler(LApplication* ApplicationRef)
		: Application(std::shared_ptr<LApplication>(ApplicationRef))
	{
        static auto* Instance = this;

        static auto WindowsCrashFunc = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
        {
            LK_CORE_WARN_TAG("WindowsCrashHandler", "EXCEPTION HANDLER");

            std::ostringstream OStringStream;
            OStringStream << "Exception Code: " << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << "\n";
            OStringStream << "Exception Address: " << ExceptionInfo->ExceptionRecord->ExceptionAddress << "\n";

            const int MaxFrames = 128;
            void* Callstack[MaxFrames];
            USHORT Frames = CaptureStackBackTrace(0, MaxFrames, Callstack, nullptr);
            OStringStream << "Stack BackTrace:\n";
            for (USHORT i = 0; i < Frames; ++i)
            {
                OStringStream << Callstack[i] << "\n";
            }

            OStringStream << "\nApplication State:\n";
            LK_CORE_INFO_TAG("WindowsCrashHandler", "{}", OStringStream.str());
            OStringStream << LApplication::Get().GenerateCrashDump();

            std::string CrashInfo = OStringStream.str();
            const std::string CrashInfoBase64 = LEncoder::Encode<EEncodingFormat::Base64>(
                reinterpret_cast<const unsigned char*>(CrashInfo.c_str()), CrashInfo.length()
            );

            WindowsExceptionStringBuffer += CrashInfoBase64;

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
		std::string DumpInfo = WindowsExceptionStringBuffer;
        if (!WindowsExceptionStringBuffer.empty())
        {
            DumpInfo += LK_FMT_LIB::format("\n{}", WindowsExceptionStringBuffer);
        }
        else
        {
			/* Include stack backtrace. */
            DumpInfo += LK_FMT_LIB::format("\n{}", CaptureBackTraceOnStack());
        }

		DumpInfo += LK_FMT_LIB::format("\n{}", LApplication::Get().GenerateCrashDump());

        return DumpInfo;
    }

    void LWindowsCrashHandler::LogCrashInformation(std::string_view CrashInformation)
    {
        LK_CORE_WARN("\n\n--- LkEngine Crash Report ---\n\t{}\n---------------------------\n", CrashInformation);
    }

    std::string LWindowsCrashHandler::CaptureBackTraceOnStack()
    {
		std::ostringstream OStringStream;

    #if LK_CRASH_INCLUDE_STACKBACKTRACE
        /* Disabled for now. */
		/* Capture stack trace. */
		const int MaxFrames = 128;
		void* Callstack[MaxFrames];
		USHORT frames = CaptureStackBackTrace(0, MaxFrames, Callstack, nullptr);
		OStringStream << "\n[SECTION] Stack Trace:\n";
		for (USHORT i = 0; i < frames; ++i)
		{
            if (i < 10)
            {
			    OStringStream << LK_FMT_LIB::format(" {}.  ", i) << Callstack[i] << "\n";
            }
            else if (i >= 10)
            {
			    OStringStream << LK_FMT_LIB::format(" {}. ", i) << Callstack[i] << "\n";
            }
		}
    #endif

        return OStringStream.str();
    }
}

#endif