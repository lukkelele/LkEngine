#include "LKpch.h"
#include "LkEngine/IO/File.h"


namespace LkEngine {

    File::File(const std::string& filePath)
        : m_Path(filePath)
    {
        std::filesystem::path filePathObject(filePath);
        m_Name = filePathObject.filename().string();
    }

    bool File::DoesFileExist(const std::string& filePath)
    {
        return std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath);
    }

}