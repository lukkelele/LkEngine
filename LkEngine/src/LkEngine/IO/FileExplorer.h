#pragma once

#include "LkEngine/IO/File.h"


namespace LkEngine {


    class FileExplorer
    {
    public:
        FileExplorer();
        ~FileExplorer();

        static File GetFile(const std::string& filePath);
        static std::vector<File> GetFilesInDirectory(const std::string& directory);
    };

}