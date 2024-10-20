#include "LKpch.h"
#include "Globals.h"


namespace LkEngine::Global {

	static FRuntimeArguments RuntimeArguments;

	static std::filesystem::path WorkingDir{};
	static std::filesystem::path BinaryDir{};

	void SetRuntimeArguments(const int InArgc, char* InArgv[])
	{
		static bool bArgumentsSet = false;
		LK_VERIFY(bArgumentsSet == false, "SetRuntimeArguments incorrectly called more than once");

		RuntimeArguments.Argc = InArgc;
		RuntimeArguments.Argv = InArgv;
		if (RuntimeArguments.Argc >= 1)
		{
			BinaryDir = std::filesystem::path(RuntimeArguments.Argv[0]);
		}

		WorkingDir = std::filesystem::current_path();

		LK_CORE_TRACE("RuntimeArgs  Argc={} Argv=\"{}\" WorkingDir=\"{}\" BinaryDir=\"{}\"", 
					  RuntimeArguments.Argc, *RuntimeArguments.Argv, 
					  WorkingDir.string(), BinaryDir.string());
		bArgumentsSet = true;
	}

	const FRuntimeArguments& GetRuntimeArguments()
	{
		return RuntimeArguments;
	}

	std::filesystem::path GetWorkingDir()
	{
		return WorkingDir;
	}

	std::filesystem::path GetBinaryDir()
	{
		return BinaryDir;
	}

}
