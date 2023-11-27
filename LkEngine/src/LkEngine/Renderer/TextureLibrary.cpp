#include "LKpch.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/IO/FileExplorer.h"


namespace LkEngine {

    TextureLibrary::TextureLibrary(const std::string& texturesDir)
        : m_TexturesDir(texturesDir)
    {
        m_Instance = this;
        m_Collection = {};
        m_Collection2D = {};
    }

    TextureLibrary::~TextureLibrary()
    {
        m_Collection.clear();
        m_Collection2D.clear();
    }

    std::shared_ptr<TextureLibrary> TextureLibrary::Create(const std::string& texturesDir)
    {
        if (texturesDir.at(texturesDir.size() - 1) != '/')
            return std::make_shared<TextureLibrary>(texturesDir + "/");
        return std::make_shared<TextureLibrary>(texturesDir);
    }
    
    std::shared_ptr<Texture> TextureLibrary::FindTexture(const std::string textureName)
    {
        if (m_Instance == nullptr)
        {
            LOG_ERROR("Texture library not created!");
            LK_ASSERT(false); // throw error
        }
        auto& lib = *m_Instance;
        if (lib.m_Collection.empty())
            return nullptr;

        for (const auto& texture : lib.m_Collection)
        {
            if (texture.first == textureName)
                return texture.second;
        }
        return nullptr;
    }

    std::shared_ptr<Texture2D> TextureLibrary::FindTexture2D(const std::string textureName)
    {
        if (m_Instance == nullptr)
        {
            LOG_ERROR("Texture library not created!");
            LK_ASSERT(false); // throw error
        }
        auto& lib = *m_Instance;
        if (lib.m_Collection2D.empty())
            return nullptr;

        for (const auto& texture : lib.m_Collection2D)
        {
            if (texture.first == textureName)
                return texture.second;
        }
        return nullptr;
    }

    // Iterate the textures/images directory and create the texture objects
    void TextureLibrary::LoadTextures()
    {
        std::vector<File> textureFiles = FileExplorer::GetFilesInDirectory(m_TexturesDir);
        for (const auto& file : textureFiles)
        {
            LOG_DEBUG("Retrieved texture file: {0} ({1})", file.GetName(), file.GetPath());
            AddTexture(file.GetName(), file.GetPath());
        }
    }

    void TextureLibrary::AddTexture(const std::string& textureName, const std::string& filePath)
    {
        s_ptr<Texture> texture = Texture::Create(filePath);
        m_Collection.insert({ textureName, texture });
        LOG_DEBUG("Added texture {0} to texture library", textureName);
    }

    void TextureLibrary::AddTexture2D(const std::string& textureName, const std::string& filePath)
    {
        s_ptr<Texture2D> texture2D = Texture2D::Create(filePath);
        m_Collection2D.insert({ textureName, texture2D });
        LOG_DEBUG("Added 2D texture {0} to texture library", textureName);
    }

}