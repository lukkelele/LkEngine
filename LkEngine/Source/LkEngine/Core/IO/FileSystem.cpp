#include "LKpch.h"
#include "FileSystem.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	bool LFileSystem::Move(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath)
	{
		if (LFileSystem::Exists(NewFilepath))
		{
			return false;
		}

		std::filesystem::rename(OldFilepath, NewFilepath);

		return true;
	}

	bool LFileSystem::Copy(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath)
	{
		if (LFileSystem::Exists(NewFilepath))
		{
			return false;
		}

		std::filesystem::copy(OldFilepath, NewFilepath);

		return true;
	}

	bool LFileSystem::Exists(const std::filesystem::path& Filepath)
	{
		return fs::exists(Filepath);
	}

	bool LFileSystem::Rename(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath)
	{
		return Move(OldFilepath, NewFilepath);
	}

	bool LFileSystem::RenameFilename(const std::filesystem::path& OldFilepath, const std::string& NewName)
	{
		const fs::path NewPath = OldFilepath.parent_path() / fs::path(NewName + OldFilepath.extension().string());

		return Rename(OldFilepath, NewPath);
	}

	bool LFileSystem::CreateDirectory(const std::filesystem::path& Directory)
	{
		LK_CORE_VERIFY(Directory.empty(), "Directory is empty");
		return fs::create_directories(Directory);
	}

	bool LFileSystem::ShowFileInExplorer(const std::filesystem::path& Filepath)
	{
		const fs::path AbsolutePath = std::filesystem::canonical(Filepath);
		if (!Exists(AbsolutePath))
		{
			return false;
		}

	#if defined(LK_PLATFORM_WINDOWS)
		const std::string Command = LK_FORMAT_STRING("explorer.exe /select,\"{0}\"", AbsolutePath.string());
	#elif defined(LK_PLATFORM_LINUX)
		const std::string Command = LK_FORMAT_STRING("xdg-open \"{0}\"", dirname(AbsolutePath.string().data()));
	#endif
		system(Command.c_str());

		return true;
	}

	bool LFileSystem::OpenDirectoryInExplorer(const std::filesystem::path& DirectoryPath)
	{
	#if defined(LK_PLATFORM_WINDOWS)
		const fs::path AbsolutePath = std::filesystem::canonical(DirectoryPath);
		if (!Exists(AbsolutePath))
		{
			return false;
		}

		ShellExecute(NULL, LK_TEXT("explore"), AbsolutePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		return true;

	#elif defined(LK_PLATFORM_LINUX)
		return ShowFileInExplorer(path);
	#endif	
	}

}
