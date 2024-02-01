#pragma once

#include "Texture.h"

#include "LkEngine/Core/IO/File.h"


namespace LkEngine {

    constexpr const char* DEFAULT_TEXTURE_DIR = "assets/Textures";

    class TextureLibrary : public RefCounted
    {
    public:
        TextureLibrary();
        ~TextureLibrary();

        void Init(bool loadRecursively = false);

        Ref<Texture> GetTexture(int textureID);
        Ref<Texture> GetTexture(const std::string textureName);
        Ref<Texture2D> GetTexture2D(int textureID);
        Ref<Texture2D> GetTexture2D(const std::string textureName);

        Ref<Texture2D> AddTexture2D(const TextureSpecification& texture);
        std::string GetTexturesDirectoryPath() { return m_TexturesDir; }

        void RenameTexture(const std::string& name, const std::string& newName);
        std::vector<std::pair<std::string, Ref<Texture2D>>> GetTextures2D();

        Ref<Texture> GetWhiteTexture();
        Ref<Texture2D> GetWhiteTexture2D();

        Ref<Texture> GetBlackTexture();
        Ref<Texture2D> GetBlackTexture2D();

        bool HasTextureWithFilename(const std::string& filename);
        bool VerifyTexturesAreLoaded() const;

        Ref<Texture> TryToGetTextureWithFilename(const std::string& filename);

        static Ref<TextureLibrary> Get() { return m_Instance; }

    private:
        bool m_Initialized = false;
        std::string m_TexturesDir = "assets/Textures";

        std::unordered_map<std::string, Ref<Texture>> m_Collection;
        std::unordered_map<std::string, Ref<Texture2D>> m_Collection2D;

        // Collections of collections
        std::unordered_map<std::string, std::unordered_map<std::string, Ref<Texture>>> m_Collections;
        std::unordered_map<std::string, std::unordered_map<std::string, Ref<Texture2D>>> m_Collections2D;

        Ref<Texture2D> m_WhiteTexture2D = nullptr;
        Ref<Texture2D> m_BlackTexture2D = nullptr;

        inline static Ref<TextureLibrary> m_Instance = nullptr;
    };

}