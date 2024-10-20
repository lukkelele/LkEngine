#pragma once

#include <string>
#include <filesystem>

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_resize2.h>

#include "LkEngine/Core/Memory/MemoryUtils.h"


namespace LkEngine {

    enum class EFileExtension
    {
        Unknown = -1,
        PNG     = 3,
        JPG     = 3,
        LUA     = 3,
        PY      = 2
    };

    enum class EFileExtensionLength
    {
        None = 0,
        PNG  = 3,
        JPG  = 3,
        LUA  = 3,
        PY   = 2
    };

    class LFile
    {
    public:
        LFile(const std::string& InFilePath);
        ~LFile() = default;

        template<typename T>
        FORCEINLINE static bool DoesFileExist(T FilePath)
        {
            LK_UNUSED(FilePath);
            return false;
        }

        template<>
        FORCEINLINE static bool DoesFileExist(std::string_view InFilePath)
        {
            return std::filesystem::exists(InFilePath) && !std::filesystem::is_directory(InFilePath);
        }

        template<>
        FORCEINLINE static bool DoesFileExist(std::filesystem::path InFilePath)
        {
            return std::filesystem::exists(InFilePath) && !std::filesystem::is_directory(InFilePath);
        }

    #if 0
        /**
         * @brief Check if a file exists.
         */
        FORCEINLINE static bool DoesFileExist(std::string_view InFilePath)
        {
            return std::filesystem::exists(InFilePath) && !std::filesystem::is_directory(InFilePath);
        }
    #endif

        /**
         * @brief Check if file has ANY file extension.
         */
        FORCEINLINE static bool HasFileExtension(const std::string& filename)
        {
			size_t pos = filename.rfind('.');
			if (pos == std::string::npos)
			{
				return false;
			}

			return true;
        }

        static std::string ExtractFilename(std::string_view InFilePath)
        {
            const size_t Pos = InFilePath.find_last_of("/\\");
            if (Pos != std::string::npos)
            {
                return std::string(InFilePath.substr(Pos + 1));
            }

            return {};
        }

        static int GetFilesInDirectory(std::string_view Directory, std::vector<LFile>& Files);

        FORCEINLINE std::string GetName() const { return Name; }
        FORCEINLINE std::string GetPath() const { return Path.string(); }

        FORCEINLINE std::filesystem::path GetParentPath() const
        {
            return Path.parent_path();
        }

        FORCEINLINE void SetPath(const std::string& InFilePath) 
        { 
            Path = InFilePath; 
        }

    private:
        std::filesystem::path Path{};
        std::string Name{};
    };

}