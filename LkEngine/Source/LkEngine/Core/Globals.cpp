#include "LKpch.h"
#include "Globals.h"

#include "LkEngine/Core/IO/FileSystem.h"


namespace LkEngine::Global {

	static FRuntimeArguments RuntimeArguments;

	static std::filesystem::path WorkingDir{};
	static std::filesystem::path BinaryDir{};
	static std::filesystem::path EngineDir{};
	static std::filesystem::path EngineConfig{};
	static std::filesystem::path ConfigDir{};

	void SetRuntimeArguments(const int Argc, char* Argv[])
	{
		static bool bArgumentsSet = false;
		LK_CORE_ASSERT(bArgumentsSet == false && "SetRuntimeArguments incorrectly called more than once");

		RuntimeArguments.Argc = Argc;
		RuntimeArguments.Argv = Argv;
		if (RuntimeArguments.Argc >= 1)
		{
			BinaryDir = std::filesystem::path(RuntimeArguments.Argv[0]).parent_path().string() + PathSeparator;
		}

		namespace fs = std::filesystem;

		WorkingDir = std::filesystem::current_path();
		//LK_PRINTLN("Working Directory: {}", WorkingDir);

		int Traversed = 0;
		fs::path Path = WorkingDir;
		while (Path.filename() != "LkEngine")
		{
			//LK_PRINTLN("Current Path: {}", Path);
			Path = Path.parent_path();
			Traversed++;
			LK_CORE_VERIFY(Traversed <= 4, "Cannot find LkEngine configuration file");
		}

		bool bFoundEngineConfig = false;

		LK_CORE_ASSERT(Path.filename() == "LkEngine", "Path is not LkEngine");
		/* The engine config is placed in the 'LkEngine/LkRuntime' directory. */
		EngineDir = Path;
		EngineDir += PathSeparator + std::string("LkRuntime");
		LK_CORE_VERIFY(LFileSystem::IsDirectory(EngineDir), "Engine directory is not valid");

		ConfigDir = EngineDir;
		ConfigDir += PathSeparator + std::string("Configuration");
		//LK_PRINTLN("ConfigDir: {}", ConfigDir);
		if (!LFileSystem::Exists(ConfigDir))
		{
			LFileSystem::CreateDirectory(ConfigDir);
			LK_CORE_VERIFY(LFileSystem::IsDirectory(ConfigDir), "Configuration directory is not valid");
		}

		EngineConfig = ConfigDir;
		EngineConfig += PathSeparator + std::string("LkEngine.lkconf");
		//LK_PRINTLN("EngineDir: {}     EngineConfig: {}", std::filesystem::absolute(EngineDir), std::filesystem::absolute(EngineConfig));

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
