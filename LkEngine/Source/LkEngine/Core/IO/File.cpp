#include "LKpch.h"
#if 0
#include "LkEngine/Core/IO/File.h"

namespace LkEngine {

    LFile::LFile(const std::string& InFilePath)
        : Path(InFilePath)
    {
        if (Path.has_filename())
        {
            Name = Path.filename().string();
        }
    }

    int LFile::GetFilesInDirectory(std::string_view Directory, std::vector<LFile>& Files)
    {
        namespace fs = std::filesystem;
        for (const fs::directory_entry& Entry : fs::directory_iterator(Directory))
        {
            if (Entry.exists() && !Entry.is_symlink() && !Entry.is_directory())
            {
                Files.emplace_back(Entry.path().parent_path().string() + "/" + Entry.path().filename().string());
            }
        }

        return Files.size();
    }

}
#endif