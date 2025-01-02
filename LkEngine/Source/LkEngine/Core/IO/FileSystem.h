#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Log/Log.h"

#include <filesystem>

/* Remove microsoft's filesystem macros. */
#if defined(LK_PLATFORM_WINDOWS) && defined(CreateDirectory)
#	undef CreateDirectory
#	undef MoveFile
#	undef CopyFile
#	undef DeleteFile
#	undef GetEnvironmentVariable
#	undef SetEnvironmentVariable
#endif


namespace LkEngine {

	class LFileSystem
	{
	public:
		static bool Exists(const std::filesystem::path& Filepath);
		static bool DeleteFile(const std::filesystem::path& Filepath);

		static bool Move(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath);
		static bool Copy(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath);
		static bool Rename(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath);
		static bool RenameFilename(const std::filesystem::path& OldFilepath, const std::string& NewName);

		static bool CreateDirectory(const std::filesystem::path& Directory);
		static bool IsDirectory(const std::filesystem::path& Path);

		static bool ShowFileInExplorer(const std::filesystem::path& Path);
		static bool OpenDirectoryInExplorer(const std::filesystem::path& DirectoryPath);

		static bool MoveFile(const std::filesystem::path& Filepath, const std::filesystem::path& Destination)
		{
			return Move(Filepath, Destination / Filepath.filename());
		}

		static std::string ConvertPathToWindows(const std::filesystem::path& UnixPath);
		static std::string ConvertPathToUnix(const std::filesystem::path& WindowsPath);

		struct FFileDialogFilterItem
		{
			const char* Name;
			const char* Spec;
		};

		static std::filesystem::path OpenFileDialog(const std::initializer_list<FFileDialogFilterItem> InFilters = {});
		static std::filesystem::path SaveFileDialog(const std::initializer_list<FFileDialogFilterItem> InFilters = {});
		static std::filesystem::path OpenFolderDialog(const char* InitialFolder = "");

		FORCEINLINE static std::filesystem::path GetWorkingDir() { return WorkingDir; }
		FORCEINLINE static std::filesystem::path GetBinaryDir() { return BinaryDir; }
		FORCEINLINE static std::filesystem::path GetEngineDir() { return EngineDir; }
		FORCEINLINE static std::filesystem::path GetEngineConfig() { return EngineConfig; }
		FORCEINLINE static std::filesystem::path GetRuntimeDir() { return RuntimeDir; }
		FORCEINLINE static std::filesystem::path GetConfigDir() { return ConfigDir; }
		FORCEINLINE static std::filesystem::path GetAssetsDir() { return AssetsDir; }
		FORCEINLINE static std::filesystem::path GetEditorConfig() { return EditorConfig; }

	private:
		static std::filesystem::path WorkingDir;
		static std::filesystem::path BinaryDir;
		static std::filesystem::path EngineDir;
		static std::filesystem::path EngineConfig;
		static std::filesystem::path RuntimeDir;
		static std::filesystem::path ConfigDir;
		static std::filesystem::path AssetsDir;
		static std::filesystem::path ProjectsDir;
		static std::filesystem::path EditorConfig;

		friend struct Global;
	};

}
