#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    // Forward declaration
    class Texture;

    class TextureLibrary
    {
    public:
        TextureLibrary();
        ~TextureLibrary();

        void LoadTextures();
        void AddTexture(const std::string& textureName, const std::string& filePath);
        void AddTexture2D(const std::string& textureName, const std::string& filePath);
        std::string GetTexturesDirectoryPath() { return m_TexturesPath; }

    private:
        std::string m_TexturesPath = "assets/textures";
        std::unordered_map<std::string, s_ptr<Texture>> m_Collection;
        std::unordered_map<std::string, s_ptr<Texture>> m_Collection2D;
    };

}