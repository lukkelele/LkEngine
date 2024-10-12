#include "LKpch.h"
#include "LkEngine/Core/IO/File.h"


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
        {
            return filename;
        }

        return ExtractFilename(filename.substr(0, filename.size() - ((size_t)fileExtension + 1)));
    }

    File File::GetFile(const std::string& filepath)
    {
        bool filepathIsDirectory = std::filesystem::is_directory(filepath);
        if (!filepathIsDirectory)
        {
            return File(filepath);
        }

        LK_CORE_ASSERT(false, "Couldnt get file in \"{}\"", filepath);
    }

    std::vector<File> File::GetFilesInDirectory(const std::string& directory)
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