#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/Log.h"

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

		static bool Move(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath);
		static bool Copy(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath);
		static bool Rename(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath);
		static bool RenameFilename(const std::filesystem::path& OldFilepath, const std::string& NewName);

		static bool CreateDirectory(const std::filesystem::path& Directory);
		static bool IsDirectory(const std::filesystem::path& Path);

		static bool ShowFileInExplorer(const std::filesystem::path& Path);
		static bool OpenDirectoryInExplorer(const std::filesystem::path& DirectoryPath);

		struct FFileDialogFilterItem
		{
			const char* Name;
			const char* Spec;
		};

		static std::filesystem::path OpenFileDialog(const std::initializer_list<FFileDialogFilterItem> InFilters = {});
		static std::filesystem::path SaveFileDialog(const std::initializer_list<FFileDialogFilterItem> InFilters = {});
		static std::filesystem::path OpenFolderDialog(const char* InitialFolder = "");
	};

}
