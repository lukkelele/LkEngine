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
                std::string filePath = fileEntry.path().parent_path().string();
                std::string fileName = fileEntry.path().filename().string();
                File file(filePath + "/" + fileName);
                files.push_back(file);
            }
        }
        return files;
    }


}