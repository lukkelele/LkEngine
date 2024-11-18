#pragma once

#include <ranges>

#include "Texture.h"

#include "LkEngine/Core/IO/File.h"


namespace LkEngine {

    /// DONT USE THESE.....................
    using TTexturePair = std::pair<std::string, TObjectPtr<LTexture>>;
    using TTextureMap = std::unordered_map<std::string, TObjectPtr<LTexture>>;

    using TTexture2DPair = std::pair<std::string, TObjectPtr<LTexture2D>>;
    using TTexture2DMap = std::unordered_map<std::string, TObjectPtr<LTexture2D>>;

    using TextureCollectionContainer = std::unordered_map<std::string, TTextureMap>;
    using Texture2DCollectionContainer = std::unordered_map<std::string, TTexture2DMap>;
    /// .....................

    class LTextureLibrary : public LObject
    {
    public:
        LTextureLibrary();
        ~LTextureLibrary();

        void Initialize(bool loadRecursively = false);

        TObjectPtr<LTexture2D> GetTexture(const int TextureID);
        TObjectPtr<LTexture2D> GetTexture(std::string_view TextureName);
        TObjectPtr<LTexture2D> AddTexture(const FTextureSpecification& TextureSpecification);

        std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>> GetTextures2D();
        int GetTextures2D(std::vector<TTexture2DPair>& TextureContainer);

        void RenameTexture(const std::string& name, const std::string& newName);

        TObjectPtr<LTexture2D> GetWhiteTexture();
        TObjectPtr<LTexture2D> GetBlackTexture();

        bool HasTextureWithFilename(const std::string& filename);
        bool VerifyTexturesAreLoaded();

        TObjectPtr<LTexture> TryToGetTextureWithFilename(const std::string& filename);

        TTextureMap& GetTextureCollection(std::string_view collectionName);
        TTexture2DMap& GetTexture2DCollection(std::string_view collectionName);

        void AddCollection(std::string_view directoryPath);

        static LTextureLibrary& Get();

    private:
        TObjectPtr<LTexture2D> m_WhiteTexture{};
        TObjectPtr<LTexture2D> m_BlackTexture{};

        TextureCollectionContainer m_Collections;

        std::unordered_map<std::string, TObjectPtr<LTexture2D>> m_Collection2D;
        std::unordered_map<std::string, TTexture2DMap> m_Collections2D;

        LCLASS(LTextureLibrary);
    };

}
