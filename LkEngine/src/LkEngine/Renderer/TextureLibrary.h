#pragma once

#include "Texture.h"

#include "LkEngine/Core/IO/File.h"


namespace LkEngine {

    using TextureCollection = std::unordered_map<std::string, Ref<Texture>>;
    using Texture2DCollection = std::unordered_map<std::string, Ref<Texture2D>>;
    using TextureCollectionContainer = std::unordered_map<std::string, TextureCollection>;
    using Texture2DCollectionContainer = std::unordered_map<std::string, Texture2DCollection>;

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

        void RenameTexture(const std::string& name, const std::string& newName);
        std::vector<std::pair<std::string, Ref<Texture2D>>> GetTextures2D();

        Ref<Texture> GetWhiteTexture();
        Ref<Texture2D> GetWhiteTexture2D();
        Ref<Texture> GetBlackTexture();
        Ref<Texture2D> GetBlackTexture2D();

        bool HasTextureWithFilename(const std::string& filename);
        bool VerifyTexturesAreLoaded() const;

        Ref<Texture> TryToGetTextureWithFilename(const std::string& filename);

        TextureCollection& GetTextureCollection(std::string_view collectionName);
        Texture2DCollection& GetTexture2DCollection(std::string_view collectionName);

        void AddCollection(std::string_view directoryPath);

        static Ref<TextureLibrary> Get() { return m_Instance; }

    private:
        bool m_Initialized = false;

        Ref<Texture> m_WhiteTexture;
        Ref<Texture> m_BlackTexture;
        Ref<Texture2D> m_WhiteTexture2D;
        Ref<Texture2D> m_BlackTexture2D;

        TextureCollection m_Collection;
        Texture2DCollection m_Collection2D;
        TextureCollectionContainer m_Collections;
        Texture2DCollectionContainer m_Collections2D;

        inline static Ref<TextureLibrary> m_Instance = nullptr;
    };

}