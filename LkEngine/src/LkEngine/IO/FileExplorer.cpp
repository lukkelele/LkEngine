#include "LKpch.h"
#include "LkEngine/IO/FileExplorer.h"



namespace LkEngine {

    FileExplorer::FileExplorer()
    {
    }

    FileExplorer::~FileExplorer()
    {
    }

    File FileExplorer::GetFile(const std::string& filePath)
    {
        bool filePathIsDirectory = std::filesystem::is_directory(filePath);
        if (!filePathIsDirectory)
            return File(filePath);
        return NULL;
    }

    std::vector<File> FileExplorer::GetFilesInDirectory(const std::string& directory)
    {
        std::vector<File> files;
        for (const auto& fileEntry : std::filesystem::directory_iterator(directory))
        {
            if (fileEntry.exists() && !fileEntry.is_symlink() && !fileEntry.is_directory())
            {
                std::filesystem::path fileEntryPath = fileEntry.path();
                std::string filePath = fileEntryPath.root_path().string();
                std::string fileName = ExtractFileName(filePath);
                File file(filePath + fileName);
                //files.push_back(File(filePath.substr(1, filePath.size() - 1)));
                files.push_back(file);
                LOG_DEBUG("Pushing back: {0} ({1})", file.GetName(), file.GetPath());
            }
        }
        return files;
    }

    std::string FileExplorer::ExtractFileName(const std::string& filePath)
    {
        size_t pos = filePath.find_last_of("/\\");
        if (pos != std::string::npos)
            return filePath.substr(pos + 1);
        return NULL;
    }

}