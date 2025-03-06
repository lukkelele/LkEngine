#include "LKpch.h"
#include "Globals.h"

#include "LkEngine/Core/IO/FileSystem.h"

#if defined(LK_ENGINE_AUTOMATION_TEST) && defined(LK_ENGINE_EDITOR)
#error "LK_ENGINE_EDITOR is not allowed to be defined when running tests"
#endif

/**
 * Allow logging in certain scenarios.
 * This is done to provide better context to potential errors that might occur
 * during engine startup or when running the github action runner.
 */
#if defined(LK_GLOBALS_PRINTING_ENABLED) || defined(LK_ENGINE_AUTOMATION_TEST)
#	define LK_GLOBALS_PRINT(...)   LK_PRINT(__VA_ARGS__)
#	define LK_GLOBALS_PRINTLN(...) LK_PRINTLN(__VA_ARGS__)
#else
#	define LK_GLOBALS_PRINT(...)
#	define LK_GLOBALS_PRINTLN(...)
#endif

namespace LkEngine {

	static FRuntimeArguments RuntimeArgs;

	void Global::SetRuntimeArguments(const int Argc, char* Argv[])
	{
		LK_GLOBALS_PRINTLN("Setting runtime arguments");
		static bool bArgumentsSet = false;
		LK_CORE_VERIFY(bArgumentsSet == false && "SetRuntimeArguments incorrectly called more than once");

		RuntimeArgs.Args.clear();
		RuntimeArgs.Args.reserve(Argc);

		RuntimeArgs.Argc = Argc;
		RuntimeArgs.Argv = Argv;
		if (RuntimeArgs.Argc >= 1)
		{
			LFileSystem::BinaryDir = std::filesystem::path(RuntimeArgs.Argv[0]).parent_path();
		}

		LFileSystem::WorkingDir = std::filesystem::current_path();

	#if defined(LK_ENGINE_AUTOMATION_TEST)
		/* Print for the hosted action runner. */
		LK_GLOBALS_PRINTLN("Binary Directory:  {}", LFileSystem::BinaryDir.string());
		LK_GLOBALS_PRINTLN("Working Directory: {}", LFileSystem::WorkingDir.string());
	#endif

		/* Find the engine root. */
		std::filesystem::path Path = LFileSystem::BinaryDir;
		{
			int Traversed = 0;
			while (Path.filename() != "LkEngine")
			{
				Path = Path.parent_path();
				Traversed++;
				LK_GLOBALS_PRINTLN("Path: {}   Traversed: {}", Path.string(), Traversed);
				LK_CORE_VERIFY(Traversed <= 5, "Cannot find LkEngine root directory");
			}
		}

		LK_GLOBALS_PRINTLN("Path: {}", Path);
		LFileSystem::EngineDir = Path;

	#if defined(LK_ENGINE_EDITOR)
		LFileSystem::EditorDir = LFileSystem::EngineDir / "LkEditor";
		LFileSystem::RuntimeDir = LFileSystem::EditorDir;
	#elif defined(LK_ENGINE_AUTOMATION_TEST)
		LFileSystem::RuntimeDir = LFileSystem::BinaryDir;
	#else
		LFileSystem::RuntimeDir = LFileSystem::EngineDir;
	#endif

		LK_GLOBALS_PRINTLN("EngineDir:  {}", LFileSystem::EngineDir.string());
		LK_GLOBALS_PRINTLN("EditorDir:  {}", LFileSystem::EditorDir.string());
		LK_GLOBALS_PRINTLN("RuntimeDir: {}", LFileSystem::RuntimeDir.string());
		LK_CORE_VERIFY(LFileSystem::IsDirectory(LFileSystem::EngineDir), "Engine directory is not valid: '{}'", LFileSystem::EngineDir.string());

		LFileSystem::ConfigDir = LFileSystem::RuntimeDir / "Configuration";
		if (!LFileSystem::Exists(LFileSystem::ConfigDir))
		{
			LK_GLOBALS_PRINTLN("Creating configuration directory: {}", LFileSystem::ConfigDir);
			LFileSystem::CreateDirectory(LFileSystem::ConfigDir);
		}

		LFileSystem::EngineConfig = LFileSystem::ConfigDir / "LkEngine.lkconf";
		LFileSystem::EditorConfig = LFileSystem::ConfigDir / "EditorSettings.yaml";
		LFileSystem::ResourcesDir = std::filesystem::absolute(LFileSystem::RuntimeDir / "Resources");
		LFileSystem::AssetsDir    = std::filesystem::absolute(LFileSystem::RuntimeDir / "Assets");
		LK_GLOBALS_PRINTLN("EngineConfig: {}", LFileSystem::EngineConfig.string());
		LK_GLOBALS_PRINTLN("EditorConfig: {}", LFileSystem::EditorConfig.string());
		LK_GLOBALS_PRINTLN("ResourcesDir:  {}", LFileSystem::ResourcesDir.string());
		LK_GLOBALS_PRINTLN("AssetsDir: {}", LFileSystem::AssetsDir.string());

		bArgumentsSet = true;
	}

	const FRuntimeArguments& Global::GetRuntimeArguments()
	{
		return RuntimeArgs;
	}

}
