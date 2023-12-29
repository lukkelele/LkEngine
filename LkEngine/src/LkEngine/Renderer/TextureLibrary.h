#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    constexpr const char* TEXTURE_DIR = "assets/textures";

    // Forward declaration
    class Texture;
    class Texture2D;

    class TextureLibrary
    {
    public:
        TextureLibrary(const std::string& texturesDir);
        ~TextureLibrary();

        static TextureLibrary* Get() { return m_Instance; }
        static s_ptr<TextureLibrary> Create(const std::string& texturesDir = TEXTURE_DIR);

        void Init();
        void LoadTextures();
        s_ptr<Texture> GetTexture(int textureID);
        s_ptr<Texture> GetTexture(const std::string textureName);
        s_ptr<Texture2D> GetTexture2D(int textureID);
        s_ptr<Texture2D> GetTexture2D(const std::string textureName);
        s_ptr<Texture> GetWhiteTexture();
        s_ptr<Texture> AddTexture(const std::string& textureName, const std::string& filePath);
        s_ptr<Texture> AddTexture2D(const std::string& textureName, const std::string& filePath);
        std::string GetTexturesDirectoryPath() { return m_TexturesDir; }
        void RenameTexture(const std::string& name, const std::string& newName);
        std::vector<std::pair<std::string, s_ptr<Texture2D>>> GetTextures2D();
        bool VerifyTexturesAreLoaded() const;

    private:
        bool m_Initialized = false;
        std::string m_TexturesDir;
        std::unordered_map<std::string, s_ptr<Texture>> m_Collection;
        std::unordered_map<std::string, s_ptr<Texture2D>> m_Collection2D;
        inline static TextureLibrary* m_Instance = nullptr;
    };

}