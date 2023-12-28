#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    // Forward declaration
    class Texture;
    class Texture2D;

    class TextureLibrary
    {
    public:
        TextureLibrary(const std::string& texturesDir);
        ~TextureLibrary();

        static TextureLibrary* Get() { return m_Instance; }
        static s_ptr<TextureLibrary> Create(const std::string& texturesDir = "assets/textures");

        void Init();
        s_ptr<Texture> GetTexture(int textureID);
        s_ptr<Texture> GetTexture(const std::string textureName);
        s_ptr<Texture2D> GetTexture2D(int textureID);
        s_ptr<Texture2D> GetTexture2D(const std::string textureName);
        s_ptr<Texture> GetWhiteTexture();

        void LoadTextures();
        s_ptr<Texture> AddTexture(const std::string& textureName, const std::string& filePath);
        s_ptr<Texture> AddTexture2D(const std::string& textureName, const std::string& filePath);
        std::string GetTexturesDirectoryPath() { return m_TexturesDir; }

        bool VerifyTexturesAreLoaded() const;

    private:
        bool m_Initialized = false;
        std::string m_TexturesDir;
        std::unordered_map<std::string, s_ptr<Texture>> m_Collection;
        std::unordered_map<std::string, s_ptr<Texture2D>> m_Collection2D;
        inline static TextureLibrary* m_Instance = nullptr;
    };

}