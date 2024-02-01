#pragma once

#include <string>
#include <filesystem>

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_resize2.h>

#include "LkEngine/Utilities/MemoryUtils.h"


namespace LkEngine {

    enum class FileExtension
    {
        Unknown = -1,
        PNG     = 3,
        JPG     = 3,
        LUA     = 3,
        PY      = 2
    };

    class File
    {
    public:
        File(const std::string& filePath);
        ~File() = default;

        static bool DoesFileExist(const std::string& filePath);
        static bool HasFileExtension(const std::string& filename);
        static std::string ExtractFilenameWithoutExtension(const std::string& filename, const FileExtension& fileExtension = FileExtension::PNG);
        static std::string ExtractFilename(const std::string& filepath);
        static File GetFile(const std::string& filePath);
        static std::vector<File> GetFilesInDirectory(const std::string& directory);

        std::string GetName() const { return m_Name; }
        std::string GetPath() const { return m_Path; }
        std::string GetParentPath() const;
        void SetPath(const std::string& filePath) { m_Path = filePath; }
        void PrintInformation() const;

    private:
        std::string m_Name;
        std::string m_Path;
        std::filesystem::path m_PathObject;
    };

}