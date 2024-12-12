#include "LKpch.h"
#include "Globals.h"

#include "LkEngine/Core/IO/FileSystem.h"


namespace LkEngine {

	static FRuntimeArguments RuntimeArguments;

	int Global::Argc;
	std::vector<std::string> Global::Argv;

	void Global::SetRuntimeArguments(const int Argc, char* Argv[])
	{
		static bool bArgumentsSet = false;
		LK_CORE_ASSERT(bArgumentsSet == false && "SetRuntimeArguments incorrectly called more than once");

		RuntimeArguments.Argc = Argc;
		RuntimeArguments.Argv = Argv;
		if (RuntimeArguments.Argc >= 1)
		{
			LFileSystem::BinaryDir = std::filesystem::path(RuntimeArguments.Argv[0]).parent_path().string() + PathSeparator;
		}

		namespace fs = std::filesystem;

		LFileSystem::WorkingDir = std::filesystem::current_path();

		int Traversed = 0;
		fs::path Path = LFileSystem::WorkingDir;
		while (Path.filename() != "LkEngine")
		{
			Path = Path.parent_path();
			Traversed++;
			LK_CORE_VERIFY(Traversed <= 4, "Cannot find LkEngine configuration file");
		}

		bool bFoundEngineConfig = false;

		LK_CORE_ASSERT(Path.filename() == "LkEngine", "Path is not LkEngine");
		/* The engine config is placed in the 'LkEngine/LkRuntime' directory. */
		LFileSystem::EngineDir = Path;
		LFileSystem::EngineDir += PathSeparator + std::string("LkRuntime");
		LFileSystem::RuntimeDir = LFileSystem::EngineDir;
		LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::EngineDir), "Engine directory is not valid");
		LFileSystem::ConfigDir = LFileSystem::WorkingDir;

		LFileSystem::ConfigDir = LFileSystem::EngineDir;
		LFileSystem::ConfigDir += PathSeparator + std::string("Configuration");
		//LK_PRINTLN("ConfigDir: {}", ConfigDir);
		if (!LFileSystem::Exists(LFileSystem::ConfigDir))
		{
			LFileSystem::CreateDirectory(LFileSystem::ConfigDir);
			LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::ConfigDir), "Configuration directory is not valid");
		}

		LFileSystem::EngineConfig = LFileSystem::ConfigDir;
		LFileSystem::EngineConfig += PathSeparator + std::string("LkEngine.lkconf");
		//LK_PRINTLN("EngineDir: {}     EngineConfig: {}", std::filesystem::absolute(EngineDir), std::filesystem::absolute(EngineConfig));

		LFileSystem::AssetsDir = fs::absolute(LFileSystem::EngineDir / "Assets");

		bArgumentsSet = true;
	}

	const FRuntimeArguments& Global::GetRuntimeArguments()
	{
		return RuntimeArguments;
	}

}
