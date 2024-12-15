#include "LKpch.h"
#include "Globals.h"

#include "LkEngine/Core/IO/FileSystem.h"

/**
 * Allow logging when running tests. 
 * This is done to provide better context to potential errors that might occur
 * when running the tests in the github action runner.
 */
#if defined(LK_ENGINE_AUTOMATION_TEST)
#	define LK_GLOBALS_PRINT(...)     LK_PRINT(__VA_ARGS__)
#	define LK_GLOBALS_PRINTLN(...)   LK_PRINTLN(__VA_ARGS__)
#else
#	define LK_GLOBALS_PRINT(...)
#	define LK_GLOBALS_PRINTLN(...)
#endif


namespace LkEngine {

	static FRuntimeArguments RuntimeArguments;

	int Global::Argc;
	std::vector<std::string> Global::Argv;

	namespace fs = std::filesystem;

	void Global::SetRuntimeArguments(const int Argc, char* Argv[])
	{
		LK_GLOBALS_PRINTLN("Setting runtime arguments");
		static bool bArgumentsSet = false;
		LK_CORE_VERIFY(bArgumentsSet == false && "SetRuntimeArguments incorrectly called more than once");

		RuntimeArguments.Argc = Argc;
		RuntimeArguments.Argv = Argv;
		if (RuntimeArguments.Argc >= 1)
		{
			LFileSystem::BinaryDir = std::filesystem::path(RuntimeArguments.Argv[0]).parent_path().string() + PathSeparator;
		}

		LFileSystem::WorkingDir = std::filesystem::current_path();
		LK_GLOBALS_PRINTLN("Working Directory: {}", LFileSystem::WorkingDir.string());

		fs::path Path = LFileSystem::WorkingDir;
		{
			int Traversed = 0;
			while (Path.filename() != "LkEngine")
			{
				Path = Path.parent_path();
				Traversed++;
				LK_GLOBALS_PRINTLN("Path: {}   Traversed: {}", Path.string(), Traversed);
				LK_CORE_VERIFY(Traversed <= 4, "Cannot find LkEngine root directory");
			}
		}

		bool bFoundEngineConfig = false;
		LK_CORE_VERIFY(Path.filename() == "LkEngine", "Path is not LkEngine");
		/* The engine config is placed in the 'LkEngine/LkRuntime' directory. */

		/**
		 * TODO: Evaluate how to best solve this.
		 *       Should just find the root engine directory for all build configurations.
		 */
		int Traversed = 0;
		while (Path.parent_path().filename() == "LkEngine")
		{
			Path = Path.parent_path();
			Traversed++;
			LK_GLOBALS_PRINTLN("Path: {}   Traversed: {}", Path.string(), Traversed);
			LK_CORE_VERIFY(Traversed <= 4, "Traversal to find LkEngine root directory failed");
		}

		LFileSystem::EngineDir = Path;
		LFileSystem::EngineDir += PathSeparator + std::string("LkRuntime");
		LFileSystem::RuntimeDir = LFileSystem::EngineDir;
		LK_GLOBALS_PRINTLN("WorkingDir:  {}", LFileSystem::WorkingDir.string());
		LK_GLOBALS_PRINTLN("EngineDir:   {}", LFileSystem::EngineDir.string());
		LK_GLOBALS_PRINTLN("RuntimeDir:  {}", LFileSystem::RuntimeDir.string());
		LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::EngineDir), "Engine directory is not valid: '{}'", LFileSystem::EngineDir.string());
		LFileSystem::ConfigDir = LFileSystem::WorkingDir;

		LFileSystem::ConfigDir = LFileSystem::EngineDir;
		LFileSystem::ConfigDir += PathSeparator + std::string("Configuration");
		if (!LFileSystem::Exists(LFileSystem::ConfigDir))
		{
			LFileSystem::CreateDirectory(LFileSystem::ConfigDir);
			LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::ConfigDir), "Configuration directory is not valid");
		}

		LFileSystem::EngineConfig = LFileSystem::ConfigDir;
		LFileSystem::EngineConfig += PathSeparator + std::string("LkEngine.lkconf");

		LFileSystem::AssetsDir = fs::absolute(LFileSystem::EngineDir / "Assets");

		bArgumentsSet = true;
	}

	const FRuntimeArguments& Global::GetRuntimeArguments()
	{
		return RuntimeArguments;
	}

}
