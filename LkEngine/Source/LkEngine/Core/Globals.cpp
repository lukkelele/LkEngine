#include "LKpch.h"
#include "Globals.h"


namespace LkEngine::Global {

	static FRuntimeArguments RuntimeArguments;
	static std::filesystem::path WorkingDirectory{};

	void SetRuntimeArguments(const int InArgc, char* InArgv[])
	{
		static bool bArgumentsSet = false;
		LK_VERIFY(bArgumentsSet == false, "SetRuntimeArguments incorrectly called more than once");

		RuntimeArguments.Argc = InArgc;
		RuntimeArguments.Argv = InArgv;
		if (RuntimeArguments.Argc >= 1)
		{
			WorkingDirectory = std::filesystem::path(RuntimeArguments.Argv[0]);
		}

		LK_CORE_TRACE("RuntimeArgs   Argc={} Argv=\"{}\" WorkingDirectory=\"{}\"", 
					  RuntimeArguments.Argc, *RuntimeArguments.Argv, WorkingDirectory.string());
		bArgumentsSet = true;
	}

	const FRuntimeArguments& GetRuntimeArguments()
	{
		return RuntimeArguments;
	}

	std::filesystem::path GetWorkingDirectory()
	{
		return WorkingDirectory;
	}

}
