#include "LKpch.h"
#include "TextureLibrary.h"
#include "Renderer.h"

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

		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA;
		spec.Width = 1;
		spec.Height = 1;
        spec.Name = "white-texture";

		constexpr uint32_t whiteTextureData = 0xFFFFFFFF;
		m_WhiteTexture2D = Texture2D::Create(spec, Buffer(&whiteTextureData, sizeof(uint32_t)));

        spec.Name = "black-texture";
		constexpr uint32_t blackTextureData = 0xFF000000;
		m_BlackTexture2D = Texture2D::Create(spec, Buffer(&blackTextureData, sizeof(uint32_t)));

        m_Collection2D.insert({ "white-texture", m_WhiteTexture2D });
        m_Collection2D.insert({ "black-texture", m_BlackTexture2D });
    }


    TextureLibrary::~TextureLibrary()
    {
        m_Collection.clear();
        m_Collection2D.clear();
    }

    void TextureLibrary::Init(bool loadRecursively)
    {
        namespace fs = std::filesystem;

        if (loadRecursively == true)
        {
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
        throw std::runtime_error(fmt::format("Could not find texture -> {}", textureName).c_str());
        return m_Collection2D.at("white-texture"); // White texture is the first added texture in the 2D collection
    }

    // Iterate the textures/images directory and create the texture objects
    void TextureLibrary::LoadTextures()
    {
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
            {
                LOG_DEBUG("Texture {} already exists in the 2D collection, returning it", textureName);
                return entry.second;
            }
        }
        s_ptr<Texture2D> texture2D = Texture2D::Create(filePath);
        m_Collection2D.insert({ File::ExtractFilenameWithoutExtension(textureName), texture2D });

        return texture2D;
    }

    s_ptr<Texture> TextureLibrary::AddTexture2D(const TextureSpecification& textureSpec)
    {
        // Check if the texture already exists
        for (const auto& entry : m_Collection2D)
        {
            if (entry.first == File::ExtractFilenameWithoutExtension(textureSpec.Name))
            {
                LOG_DEBUG("Texture {} already exists in the 2D collection, returning it", textureSpec.Name);
                return entry.second;
            }
        }
        s_ptr<Texture2D> texture2D = Texture2D::Create(textureSpec);
        m_Collection2D.insert({ File::ExtractFilenameWithoutExtension(textureSpec.Name), texture2D });

        Renderer::Get()->GetRendererAPI()->GetRenderer2D()->AddTextureToSlot(texture2D);

        return texture2D;
    }

    s_ptr<Texture> TextureLibrary::AddTexture2D(s_ptr<Texture2D> texture)
    {
        // Check if the texture already exists
        for (const auto& entry : m_Collection2D)
        {
            if (entry.first == File::ExtractFilenameWithoutExtension(texture->GetName()))
            {
                LOG_DEBUG("Texture {} already exists in the 2D collection, returning it", texture->GetName());
                return entry.second;
            }
        }
        m_Collection2D.insert({ File::ExtractFilenameWithoutExtension(texture->GetName()), texture });

        return texture;
    }

    s_ptr<Texture> TextureLibrary::GetWhiteTexture()
    {
        return m_WhiteTexture2D;
    }

    s_ptr<Texture2D> TextureLibrary::GetWhiteTexture2D()
    {
        //LK_ASSERT(m_Collection2D.at("white-texture") != nullptr);
        //return m_Collection2D.at("white-texture");
        return m_WhiteTexture2D;
    }

    s_ptr<Texture> TextureLibrary::GetBlackTexture()
    {
        return m_BlackTexture2D;
    }

    s_ptr<Texture2D> TextureLibrary::GetBlackTexture2D()
    {
        return m_BlackTexture2D;
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
        textures.push_back({ "white-texture", m_WhiteTexture2D });
        for (auto& entry : m_Collection2D)
        {
            if (entry.second->GetName() == "black-texture" || entry.second->GetName() == "white-texture")
                continue;
            textures.push_back(entry);
        }
        return textures;
    }

}