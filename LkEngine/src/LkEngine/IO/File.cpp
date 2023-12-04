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

    bool File::HasFileExtension(const std::string& filename)
    {
        size_t pos = filename.rfind('.');
        if (pos == std::string::npos) 
            return false;
        return true;
    }

    std::string File::ExtractFilename(const std::string& filePath)
    {
        size_t pos = filePath.find_last_of("/\\");
        if (pos != std::string::npos)
        {
            return filePath.substr(pos + 1);
        }
        return filePath;
    }

    std::string File::ExtractFilenameWithoutExtension(const std::string& filename, const FileExtension& fileExtension)
    {
        if (HasFileExtension(filename) == false)
            return filename;
        //return ExtractFilename(filename.substr(0, filename.size() - 4));
        return ExtractFilename(filename.substr(0, filename.size() - ((size_t)fileExtension + 1)));
    }


}