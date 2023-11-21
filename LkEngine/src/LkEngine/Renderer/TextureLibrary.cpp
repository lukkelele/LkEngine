#include "LKpch.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/IO/FileExplorer.h"


namespace LkEngine {

    TextureLibrary::TextureLibrary()
    {
    }

    TextureLibrary::~TextureLibrary()
    {
        m_Collection.clear();
        m_Collection2D.clear();
    }

    // Iterate the textures/images directory and create the texture objects
    void TextureLibrary::LoadTextures()
    {
        std::vector<File> textureFiles = FileExplorer::GetFilesInDirectory(m_TexturesPath);
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