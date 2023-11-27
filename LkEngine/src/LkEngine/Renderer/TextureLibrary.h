#pragma once

#include "LkEngine/Core/Base.h"



namespace LkEngine {

    constexpr const char* LK_TEXTURES_DIR = "assets/img/";

    // Forward declaration
    class Texture;

    class TextureLibrary
    {
    public:
        TextureLibrary(const std::string& texturesDir = LK_TEXTURES_DIR);
        ~TextureLibrary();

        static TextureLibrary* Get() { return m_Instance; }
        static std::shared_ptr<TextureLibrary> Create(const std::string& texturesDir = LK_TEXTURES_DIR);
        static std::shared_ptr<Texture> FindTexture(const std::string textureName);
        static std::shared_ptr<Texture2D> FindTexture2D(const std::string textureName);

        void LoadTextures();
        void AddTexture(const std::string& textureName, const std::string& filePath);
        void AddTexture2D(const std::string& textureName, const std::string& filePath);
        std::string GetTexturesDirectoryPath() { return m_TexturesDir; }

    private:
        std::string m_TexturesDir;
        std::unordered_map<std::string, s_ptr<Texture>> m_Collection;
        std::unordered_map<std::string, s_ptr<Texture2D>> m_Collection2D;
        inline static TextureLibrary* m_Instance = nullptr;
    };

}