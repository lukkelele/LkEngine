#include "LKpch.h"
#include "FileSystem.h"

#include "nfd.hpp"


namespace LkEngine {

	/**
	 * The following members are set during the evaluation of the runtime arguments in 'Globals.h'.
	 */
	std::filesystem::path LFileSystem::WorkingDir{};
	std::filesystem::path LFileSystem::BinaryDir{};
	std::filesystem::path LFileSystem::EngineDir{};
	std::filesystem::path LFileSystem::EngineConfig{};
	std::filesystem::path LFileSystem::RuntimeDir{};
	std::filesystem::path LFileSystem::ConfigDir{};
	std::filesystem::path LFileSystem::AssetsDir{};
	std::filesystem::path LFileSystem::ProjectsDir{};
	std::filesystem::path LFileSystem::EditorConfig{};

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
		return std::filesystem::exists(Filepath);
	}

	bool LFileSystem::DeleteFile(const std::filesystem::path& Filepath)
	{
		LK_CORE_ASSERT(!Filepath.empty(), "Cannot delete an empty file");
		if (!LFileSystem::Exists(Filepath))
		{ 
			return false;
		}

		if (std::filesystem::is_directory(Filepath))
		{
			return (std::filesystem::remove_all(Filepath) > 0);
		}

		return std::filesystem::remove(Filepath);
	}

	bool LFileSystem::Rename(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath)
	{
		return Move(OldFilepath, NewFilepath);
	}

	bool LFileSystem::RenameFilename(const std::filesystem::path& OldFilepath, const std::string& NewName)
	{
		const fs::path NewPath = (OldFilepath.parent_path() / fs::path(NewName + OldFilepath.extension().string()));

		return Rename(OldFilepath, NewPath);
	}

	bool LFileSystem::CreateDirectory(const std::filesystem::path& Directory)
	{
		LK_CORE_VERIFY(!Directory.empty(), "Directory is empty");
		if (Directory.has_extension())
		{
			LK_CORE_WARN_TAG("FileSystem", "The name of directory '{}' contains a file extension, cannot create as directory", Directory.filename());
			return false;
		}

		return std::filesystem::create_directories(Directory);
	}

	bool LFileSystem::IsDirectory(const std::filesystem::path& Path)
	{
		return std::filesystem::is_directory(Path);
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
		const std::filesystem::path AbsolutePath = std::filesystem::canonical(DirectoryPath);
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

	std::string LFileSystem::ConvertPathToWindows(const std::filesystem::path& UnixPath)
	{
		if (UnixPath.empty())
		{
			return {};
		}

		std::string UnixPathStr = UnixPath.string();
		std::replace(UnixPathStr.begin(), UnixPathStr.end(), '/', '\\');

		return UnixPathStr;
	}

	std::string LFileSystem::ConvertPathToUnix(const std::filesystem::path& WindowsPath)
	{
		if (WindowsPath.empty())
		{
			return {};
		}

		std::string WindowsPathStr = WindowsPath.string();
		std::replace(WindowsPathStr.begin(), WindowsPathStr.end(), '/', '\\');

		return WindowsPathStr;
	}

	std::filesystem::path LFileSystem::OpenFileDialog(const std::initializer_list<FFileDialogFilterItem> InFilters)
	{
		NFD::UniquePath FilePath;
		const nfdresult_t Result = NFD::OpenDialog(FilePath, (const nfdfilteritem_t*)InFilters.begin(), InFilters.size());

		switch (Result)
		{
			case NFD_OKAY: return FilePath.get();
			case NFD_CANCEL: return "";
			case NFD_ERROR:
			{
				LK_CORE_VERIFY(false, "[NFD-Extended] Error: {}", NFD::GetError());
				return "";
			}
		}

		LK_CORE_ASSERT(false);
		return std::filesystem::path();
	}

	std::filesystem::path LFileSystem::SaveFileDialog(const std::initializer_list<FFileDialogFilterItem> InFilters)
	{
		NFD::UniquePath FilePath;
		const nfdresult_t Result = NFD::SaveDialog(FilePath, (const nfdfilteritem_t*)InFilters.begin(), InFilters.size());

		switch (Result)
		{
			case NFD_OKAY:	 return FilePath.get();
			case NFD_CANCEL: return "";
			case NFD_ERROR:
			{
				LK_CORE_VERIFY(false, "[NFD-Extended] Error: {}", NFD::GetError());
				return "";
			}
		}

		LK_CORE_ASSERT(false);
		return std::filesystem::path();
	}

	std::filesystem::path LFileSystem::OpenFolderDialog(const char* InitialFolder)
	{
		NFD::UniquePath FilePath;
		const nfdresult_t Result = NFD::PickFolder(FilePath, InitialFolder);

		switch (Result)
		{
			case NFD_OKAY:	 return FilePath.get();
			case NFD_CANCEL: return "";

			case NFD_ERROR:
			{
				LK_CORE_VERIFY(false, "[NFD-Extended] Error: {}", NFD::GetError());
				return "";
			}
		}

		LK_CORE_ASSERT(false);
		return std::filesystem::path();
	}

}
