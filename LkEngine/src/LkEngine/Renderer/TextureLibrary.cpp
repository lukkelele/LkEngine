#include "LKpch.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/IO/FileExplorer.h"


namespace LkEngine {

    TextureLibrary::TextureLibrary(const std::string& texturesDir)
        : m_TexturesDir(texturesDir)
    {
        m_Instance = this;
        m_Collection;
        m_Collection2D;
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
        if (m_Initialized == false)
        {
            LoadTextures();
            m_Initialized = true;
        }

        if (m_Collection.empty())
        {
            LOG_WARN("Texture library is empty");
            return nullptr;
        }

        //for (auto iter = lib.m_Collection.begin(); iter != lib.m_Collection.end(); ++iter)
        for (auto iter = m_Collection.begin(); iter != m_Collection.end(); ++iter)
        {
            auto& texture = *iter;
            if (texture.first == textureName)
                return texture.second;
        }
        return nullptr;
    }

    std::shared_ptr<Texture2D> TextureLibrary::FindTexture2D(const std::string textureName)
    {
        if (m_Initialized == false)
        {
            LoadTextures();
            m_Initialized = true;
            LK_ASSERT(false); // testing
        }

        if (m_Collection2D.empty())
        {
            LOG_WARN("Texture library is empty");
            return nullptr;
        }
        //for (auto iter = lib.m_Collection2D.begin(); iter != lib.m_Collection2D.end(); ++iter)
        for (auto iter = m_Collection2D.begin(); iter != m_Collection2D.end(); ++iter)
        {
            auto& texture = *iter;
            auto name = FileExplorer::ExtractFileName(texture.first);
            LOG_DEBUG("FILENAME FOR ITERATED 2D_COLL: {} ({})", name, texture.first);

            if (FileExplorer::ExtractFileName(texture.first) == textureName)
                return texture.second;
        }
        return nullptr;
    }

    // Iterate the textures/images directory and create the texture objects
    void TextureLibrary::LoadTextures()
    {
        LOG_CRITICAL("Loading textures into texture library");
        std::vector<File> textureFiles = FileExplorer::GetFilesInDirectory(m_TexturesDir);
        for (const auto& file : textureFiles)
        {
            LOG_WARN("Retrieved texture file: {0} ({1})", file.GetName(), file.GetPath());

            // TODO: Need to make it so 2D and 3D texture are located in different dirs
            //AddTexture(file.GetName(), file.GetPath());
            AddTexture2D(file.GetName(), file.GetPath());
        }
        m_Initialized = true;
    }

    void TextureLibrary::AddTexture(const std::string& textureName, const std::string& filePath)
    {
        s_ptr<Texture> texture = Texture::Create(filePath);
        m_Collection.insert({ textureName, texture });
        LOG_DEBUG("Added texture {0} to texture library", textureName);
    }

    void TextureLibrary::AddTexture2D(const std::string& textureName, const std::string& filePath)
    {
        // Check if the texture already exists
        for (const auto& entry : m_Collection2D)
        {
            if (entry.first == textureName)
            {
                LOG_WARN("Texture (2D) {0} already exists", textureName);
                return;
            }
        }
        s_ptr<Texture2D> texture2D = Texture2D::Create(filePath);
        m_Collection2D.insert({ textureName, texture2D });
        LOG_DEBUG("Added 2D texture {0} to texture library", textureName);
        LK_ASSERT(m_Collection2D.at(textureName) != nullptr);
    }

}