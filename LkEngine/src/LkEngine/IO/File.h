#pragma once

#include <string>
#include <filesystem>


namespace LkEngine {

    class File
    {
    public:
        File(const std::string& filePath);
        ~File() = default;

        static bool DoesFileExist(const std::string& filePath);

        std::string GetName() const { return m_Name; }
        std::string GetPath() const { return m_Path; }

    private:
        std::string m_Name;
        std::string m_Path;
    };

}