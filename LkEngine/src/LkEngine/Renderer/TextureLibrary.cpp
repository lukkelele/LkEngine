#include "LKpch.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/IO/File.h"


namespace LkEngine {

    static std::vector<std::string> ImageExtensions = { 
        ".jpg", 
        ".jpeg", 
        ".png", 
        ".bmp" 
    };

    FileExtension DetermineExtension(const std::filesystem::directory_entry& entry)
    {
        if (entry.is_regular_file() == false)
            return FileExtension::Unknown;

        std::string filename = entry.path().filename().string();
        std::string filepath = entry.path().string();
        std::string fileExtension = entry.path().extension().string();

        if (fileExtension == "png")
            return FileExtension::PNG;
        else if (fileExtension == "jpg")
            return FileExtension::JPG;
        return FileExtension::Unknown;
    }


    TextureLibrary::TextureLibrary(const std::string& texturesDir)
        : m_TexturesDir(texturesDir)
    {
        m_Instance = this;
    }

    TextureLibrary::~TextureLibrary()
    {
        m_Collection.clear();
        m_Collection2D.clear();
    }

    void TextureLibrary::Init()
    {
        // Create white texture as first texture
        TextureSpecification whiteTextureSpec;
        whiteTextureSpec.Width = 400;
        whiteTextureSpec.Height = 400;
        whiteTextureSpec.Name = "White Texture";
        whiteTextureSpec.Path = "assets/img/white-texture.png";
        auto whiteTexture = Texture2D::Create(whiteTextureSpec);
        m_Collection2D.insert({ "WhiteTexture", whiteTexture });

        namespace fs = std::filesystem;

        // Find all textures and add to respective texture collections
        for (const auto& entry : fs::recursive_directory_iterator(m_TexturesDir)) 
        {
            if (entry.is_regular_file()) 
            {
                std::string filepath = entry.path().string();
                std::string extension = entry.path().extension().string();

                // Check if the file has an image extension
                if (std::find(ImageExtensions.begin(), ImageExtensions.end(), extension) != ImageExtensions.end()) 
                {
                    LOG_DEBUG("Found image/texture: {}", filepath);
                    AddTexture2D(File::ExtractFilenameWithoutExtension(entry.path().filename().string(), DetermineExtension(entry)), filepath);
                }
            }
        }

        m_Initialized = true;
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
                return texture.second;
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
                return texture.second;
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
                return texture.second;
        }

        //return nullptr;
        // If no texture is found, return white texture
        return m_Collection2D.at("WhiteTexture"); // White texture is the first added texture in the 2D collection
    }

    // Iterate the textures/images directory and create the texture objects
    void TextureLibrary::LoadTextures()
    {
        //std::vector<File> textureFiles = FileExplorer::GetFilesInDirectory(m_TexturesDir);
        std::vector<File> textureFiles = File::GetFilesInDirectory(m_TexturesDir);
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
                return entry.second;
        }
        s_ptr<Texture2D> texture2D = Texture2D::Create(filePath);
        m_Collection2D.insert({ File::ExtractFilenameWithoutExtension(textureName), texture2D });
        //LOG_DEBUG("Added 2D texture {0} to texture library", File::ExtractFilenameWithoutExtension(textureName));

        return texture2D;
    }

    s_ptr<Texture> TextureLibrary::GetWhiteTexture()
    {
        return m_Collection2D.at("WhiteTexture");
    }

    void TextureLibrary::RenameTexture(const std::string& name, const std::string& newName)
    {
        auto it = m_Collection2D.find(name);
        if (it == m_Collection2D.end())
            return;

        m_Collection2D[name] = it->second;
        m_Collection2D.erase(it);
    }

    std::vector<std::pair<std::string, s_ptr<Texture2D>>> TextureLibrary::GetTextures2D()
    {
        std::vector<std::pair<std::string, s_ptr<Texture2D>>> textures;
        for (auto& entry : m_Collection2D)
            textures.push_back(entry);
        return textures;
    }

}