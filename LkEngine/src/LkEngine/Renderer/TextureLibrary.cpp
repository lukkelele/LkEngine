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

    s_ptr<TextureLibrary> TextureLibrary::Create(const std::string& texturesDir)
    {
        if (texturesDir.at(texturesDir.size() - 1) != '/')
            return std::make_shared<TextureLibrary>(texturesDir + "/");
        return std::make_shared<TextureLibrary>(texturesDir);
    }

    s_ptr<Texture> TextureLibrary::GetTexture(int textureID)
    {
        if (m_Collection.empty())
            throw std::runtime_error("Texture library is empty");

        for (auto iter = m_Collection.begin(); iter != m_Collection.end(); ++iter)
        {
            auto& texture = *iter;
            if (texture.second->GetRendererID() == textureID)
            {
                return texture.second;
            }
        }
    } 

    s_ptr<Texture> TextureLibrary::GetTexture(const std::string textureName)
    {
        if (m_Collection.empty())
            throw std::runtime_error("Texture library is empty");

        for (auto iter = m_Collection.begin(); iter != m_Collection.end(); ++iter)
        {
            auto& texture = *iter;
            if (File::ExtractFilenameWithoutExtension(texture.first, FileExtension::PNG) == textureName)
                return texture.second;
        }
        return nullptr;
    }

    s_ptr<Texture2D> TextureLibrary::GetTexture2D(int textureID)
    {
        if (m_Collection2D.empty())
            throw std::runtime_error("2D Texture library is empty");

        for (auto iter = m_Collection2D.begin(); iter != m_Collection2D.end(); ++iter)
        {
            auto& texture = *iter;
            if (texture.second->GetRendererID() == textureID)
            {
                return texture.second;
            }
        }
    } 

    s_ptr<Texture2D> TextureLibrary::GetTexture2D(const std::string textureName)
    {
        if (m_Collection2D.empty())
            throw std::runtime_error("2D Texture library is empty");

        for (auto iter = m_Collection2D.begin(); iter != m_Collection2D.end(); ++iter)
        {
            auto& texture = *iter;
            if (File::ExtractFilenameWithoutExtension(texture.first, FileExtension::PNG) == textureName)
            {
                return texture.second;
            }
        }

        LOG_WARN("GetTexture: Couldn't find texture, creating it instead -> ( {}, {} )", textureName, m_TexturesDir + textureName);
        AddTexture2D(textureName, m_TexturesDir + textureName);

        return nullptr;
    }

    // Iterate the textures/images directory and create the texture objects
    void TextureLibrary::LoadTextures()
    {
        std::vector<File> textureFiles = FileExplorer::GetFilesInDirectory(m_TexturesDir);
        for (const auto& file : textureFiles)
        {
            // TODO: Need to make it so 2D and 3D texture are located in different dirs
            AddTexture2D(file.GetName(), file.GetPath());
        }
        m_Initialized = true;
    }

    // TODO: Expand this as the 2D function is as well
    s_ptr<Texture> TextureLibrary::AddTexture(const std::string& textureName, const std::string& filePath)
    {
        s_ptr<Texture> texture = Texture::Create(filePath);
        m_Collection.insert({ textureName, texture });

        LOG_DEBUG("Added texture {0} to texture library", textureName);
        return texture;
    }

    s_ptr<Texture> TextureLibrary::AddTexture2D(const std::string& textureName, const std::string& filePath)
    {
        // Check if the texture already exists
        for (const auto& entry : m_Collection2D)
        {
            if (entry.first == File::ExtractFilenameWithoutExtension(textureName))
            {
                return entry.second;
            }
        }

        s_ptr<Texture2D> texture2D = Texture2D::Create(filePath);
        m_Collection2D.insert({ File::ExtractFilenameWithoutExtension(textureName), texture2D });
        LOG_DEBUG("Added 2D texture {0} to texture library", File::ExtractFilenameWithoutExtension(textureName));

        return texture2D;
    }

}