#include "LKpch.h"
#include "LkEngine/IO/File.h"


namespace LkEngine {

    File::File(const std::string& filePath)
        : m_Path(filePath)
        , m_PathObject(filePath)
    {
        m_Name = m_PathObject.filename().string();
    }

    std::string File::GetParentPath() const
    {
        return m_PathObject.parent_path().string();
    }

    void File::PrintInformation() const
    {
        printf("Filename: %s    Path: %s\n", m_Name.c_str(), m_Path.c_str());
    }

    bool File::DoesFileExist(const std::string& filePath)
    {
        return std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath);
    }

}