#include "LKpch.h"
#include "Globals.h"

#include "LkEngine/Core/IO/FileSystem.h"

/**
 * Allow logging when running automated tests. 
 * This is done to provide better context to potential errors that might occur
 * when running the tests in the github action runner.
 */
#if defined(LK_GLOBALS_PRINTING_ENABLED) || defined(LK_ENGINE_AUTOMATION_TEST)
#	define LK_GLOBALS_PRINT(...)   LK_PRINT(__VA_ARGS__)
#	define LK_GLOBALS_PRINTLN(...) LK_PRINTLN(__VA_ARGS__)
#else
#	define LK_GLOBALS_PRINT(...)
#	define LK_GLOBALS_PRINTLN(...)
#endif


namespace LkEngine {

	static FRuntimeArguments RuntimeArguments;

	int Global::Argc;
	std::vector<std::string> Global::Argv;

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

		std::filesystem::path Path = LFileSystem::WorkingDir;
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

	#if defined(LK_ENGINE_EDITOR)
		LFileSystem::EngineDir = Path / "LkEditor";
	#elif defined(LK_ENGINE_AUTOMATION_TEST)
		/* TODO: Needs to be fixed so the github action runner doesn't crash here. */
		LFileSystem::EngineDir = Path / "LkEngine"/ "LkEditor";
	#endif
		LFileSystem::RuntimeDir = LFileSystem::EngineDir;
		LK_GLOBALS_PRINTLN("WorkingDir: {}", LFileSystem::WorkingDir.string());
		LK_GLOBALS_PRINTLN("EngineDir:  {}", LFileSystem::EngineDir.string());
		LK_GLOBALS_PRINTLN("RuntimeDir: {}", LFileSystem::RuntimeDir.string());
		LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::EngineDir), "Engine directory is not valid: '{}'", LFileSystem::EngineDir.string());

		LFileSystem::ConfigDir = LFileSystem::EngineDir / "Configuration";

		if (!LFileSystem::Exists(LFileSystem::ConfigDir))
		{
			LFileSystem::CreateDirectory(LFileSystem::ConfigDir);
			LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::ConfigDir), "Configuration directory is not valid");
		}

		LFileSystem::EngineConfig = LFileSystem::ConfigDir / "LkEngine.lkconf";
		LFileSystem::EditorConfig = LFileSystem::ConfigDir / "EditorSettings.yaml";
		LFileSystem::ResourcesDir = std::filesystem::absolute(LFileSystem::EngineDir / "Resources");
		LFileSystem::AssetsDir    = std::filesystem::absolute(LFileSystem::EngineDir / "Assets");
		LK_GLOBALS_PRINTLN("EngineConfig: {}", LFileSystem::EngineConfig.string());
		LK_GLOBALS_PRINTLN("EditorConfig: {}", LFileSystem::EditorConfig.string());
		LK_GLOBALS_PRINTLN("ResourcesDir:  {}", LFileSystem::ResourcesDir.string());
		LK_GLOBALS_PRINTLN("AssetsDir: {}", LFileSystem::AssetsDir.string());

		bArgumentsSet = true;
	}

	const FRuntimeArguments& Global::GetRuntimeArguments()
	{
		return RuntimeArguments;
	}

}
