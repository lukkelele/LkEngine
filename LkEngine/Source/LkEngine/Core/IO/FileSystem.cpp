#include "LKpch.h"
#include "FileSystem.h"

#include "nfd.hpp"


namespace LkEngine {

	/**
	 * The following members are set during the evaluation of 
	 * the runtime arguments in 'Globals.h'.
	 */
	std::filesystem::path LFileSystem::WorkingDir{};
	std::filesystem::path LFileSystem::BinaryDir{};
	std::filesystem::path LFileSystem::EngineDir{};
	std::filesystem::path LFileSystem::RuntimeDir{};
	std::filesystem::path LFileSystem::ConfigDir{};
	std::filesystem::path LFileSystem::ResourcesDir{};
	std::filesystem::path LFileSystem::AssetsDir{};
	std::filesystem::path LFileSystem::ProjectsDir{};

	std::filesystem::path LFileSystem::EngineConfig{};
	std::filesystem::path LFileSystem::EditorConfig{};

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

		/* TODO: Some type of verification so not entire directories are removed accidentally. */
		if (std::filesystem::is_directory(Filepath))
		{
		#if 1
			LK_CORE_ERROR_TAG("FileSystem", "DeleteFile failed, the passed argument is a directory: {}", Filepath.string());
			LK_CORE_VERIFY(false, "LFileSystem::DeleteFile failed, the passed argument is a directory: {}", Filepath.string());
			return false;
		#else
			return (std::filesystem::remove_all(Filepath) > 0);
		#endif
		}

		return std::filesystem::remove(Filepath);
	}

	bool LFileSystem::Rename(const std::filesystem::path& OldFilepath, const std::filesystem::path& NewFilepath)
	{
		return Move(OldFilepath, NewFilepath);
	}

	bool LFileSystem::RenameFilename(const std::filesystem::path& OldFilepath, const std::string& NewName)
	{
		namespace fs = std::filesystem;
		const fs::path NewPath = (OldFilepath.parent_path() / fs::path(NewName + OldFilepath.extension().string()));

		return Rename(OldFilepath, NewPath);
	}

	bool LFileSystem::CreateDirectory(const std::filesystem::path& Directory)
	{
		LK_CORE_VERIFY(!Directory.empty(), "Directory is empty");
		if (Directory.has_extension())
		{
			LK_CORE_ERROR_TAG("FileSystem", "The name of directory '{}' contains a file extension, cannot create as directory", Directory.filename());
			return false;
		}

		return std::filesystem::create_directories(Directory);
	}

	bool LFileSystem::IsDirectory(const std::filesystem::path& Path)
	{
		return std::filesystem::is_directory(Path);
	}

	bool LFileSystem::IsNewer(const std::filesystem::path& EntryA, const std::filesystem::path& EntryB)
	{
		return std::filesystem::last_write_time(EntryA) > std::filesystem::last_write_time(EntryB);
	}

	bool LFileSystem::ShowFileInExplorer(const std::filesystem::path& Filepath)
	{
		const std::filesystem::path AbsolutePath = std::filesystem::canonical(Filepath);
		if (!Exists(AbsolutePath))
		{
			return false;
		}

	#if defined(LK_PLATFORM_WINDOWS)
		const std::string Command = std::format("explorer.exe /select,\"{0}\"", AbsolutePath.string());
	#elif defined(LK_PLATFORM_LINUX)
		const std::string Command = std::format("xdg-open \"{0}\"", dirname(AbsolutePath.string().data()));
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

	bool LFileSystem::FindSimilarFiles(const std::string& FilePattern, const std::filesystem::path& Directory, std::vector<std::filesystem::path>& Found)
	{
		LK_CORE_ASSERT(!FilePattern.empty());
		Found.clear();
		if (Directory.empty())
		{
			LK_CORE_ERROR_TAG("FileSystem", "Cannot find files similar to '{}' in an empty directory", FilePattern);
			return false;
		}

		for (const auto& FileEntry : std::filesystem::directory_iterator(Directory))
		{
			if (FileEntry.exists() && FileEntry.path().has_filename())
			{
				if (const std::filesystem::path File = FileEntry.path(); File.has_filename())
				{
					const std::string FileNameLower = StringUtils::ToLower(File.string());
					if (FileNameLower.find(StringUtils::ToLower(FilePattern)) != std::string::npos)
					{
						LK_CORE_INFO_TAG("FileSystem", "Found similar file to '{}': {}", FilePattern, File);
						Found.push_back(FileEntry);
					}
				}
			}
		}

		return true;
	}

	void LFileSystem::ConvertToPlatformPath(std::string& Path)
	{
	#if defined(LK_PLATFORM_WINDOWS)
		ConvertToWindowsPath(Path);
	#elif defined(LK_PLATFORM_LINUX)
		ConvertToUnixPath(Path);
	#endif
	}

	void LFileSystem::ConvertToUnixPath(std::string& WindowsPath)
	{
		if (WindowsPath.empty())
		{
			return;
		}

		std::replace(WindowsPath.begin(), WindowsPath.end(), '\\', '/');
	}

	void LFileSystem::ConvertToWindowsPath(std::string& UnixPath)
	{
		if (UnixPath.empty())
		{
			return;
		}

		std::replace(UnixPath.begin(), UnixPath.end(), '/', '\\');
	}

	std::string LFileSystem::ConvertToPlatformPath(const std::filesystem::path& Path)
	{
	#if defined(LK_PLATFORM_WINDOWS)
		return ConvertToWindowsPath(Path);
	#elif defined(LK_PLATFORM_LINUX)
		return ConvertToUnixPath(Path);
	#endif
	}

	std::string LFileSystem::ConvertToUnixPath(const std::filesystem::path& WindowsPath)
	{
		if (WindowsPath.empty())
		{
			return {};
		}

		std::string WindowsPathStr = WindowsPath.string();
		std::replace(WindowsPathStr.begin(), WindowsPathStr.end(), '\\', '/');

		return WindowsPathStr;
	}

	std::string LFileSystem::ConvertToWindowsPath(const std::filesystem::path& UnixPath)
	{
		if (UnixPath.empty())
		{
			return {};
		}

		std::string UnixPathStr = UnixPath.string();
		std::replace(UnixPathStr.begin(), UnixPathStr.end(), '/', '\\');

		return UnixPathStr;
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
