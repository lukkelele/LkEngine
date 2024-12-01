#include "LKpch.h"
#include "Globals.h"


namespace LkEngine::Global {

	static FRuntimeArguments RuntimeArguments;

	static std::filesystem::path WorkingDir{};
	static std::filesystem::path BinaryDir{};
	static std::filesystem::path EngineConfig{};

	void SetRuntimeArguments(const int InArgc, char* InArgv[])
	{
		static bool bArgumentsSet = false;
		assert(bArgumentsSet == false && "SetRuntimeArguments incorrectly called more than once");

		RuntimeArguments.Argc = InArgc;
		RuntimeArguments.Argv = InArgv;
		if (RuntimeArguments.Argc >= 1)
		{
			BinaryDir = std::filesystem::path(RuntimeArguments.Argv[0]).parent_path().string() + PathSeparator;
		}

		WorkingDir = std::filesystem::current_path();

	#if defined(LK_ENGINE_CORE)
		EngineConfig = WorkingDir;
		EngineConfig += PathSeparator + std::string("LkEngine.lkconf");
	#endif
		std::printf("Working Directory: %S\n", WorkingDir.c_str());

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

	std::filesystem::path GetEngineConfig()
	{
		return EngineConfig;
	}

}
