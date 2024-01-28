#include "LKpch.h"
#include "TextureLibrary.h"

#include "LkEngine/Core/IO/File.h"

#include "LkEngine/Utilities/MemoryUtils.h"


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
        m_Instance = Ref<TextureLibrary>(this);

		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA32F;
		spec.Width = 200;
		spec.Height = 200;
        spec.Name = "white-texture";
        spec.Path = "assets/textures/white-texture.png";

		//constexpr uint32_t whiteTextureData = 0xFFFFFFFF; // White Pixel
		//m_WhiteTexture2D = Texture2D::Create(spec, Buffer(&whiteTextureData, sizeof(uint32_t)));
        m_WhiteTexture2D = Texture2D::Create(spec);

        LK_CORE_ASSERT(m_WhiteTexture2D, "m_WhiteTexture2D is nullptr");
        m_Collection2D.insert({ "white-texture", m_WhiteTexture2D });

#if 0
        spec.Name = "black-texture";
		constexpr uint32_t blackTextureData = 0xFF000000;
		//m_BlackTexture2D = Texture2D::Create(spec, Buffer(&blackTextureData, sizeof(uint32_t)));
        m_BlackTexture2D = Texture2D::Create(spec, Buffer(&blackTextureData, sizeof(uint32_t)));
        LK_CORE_ASSERT(m_BlackTexture2D, "m_BlackTexture2D is nullptr");
        m_Collection2D.insert({ "black-texture", m_BlackTexture2D });
#endif

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
            TextureSpecification textureSpec;
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
                        textureSpec.Name = entry.path().filename().string();
                        textureSpec.Path = filepath;
                        LK_CORE_DEBUG("Found image/texture: {}", filepath);
                        AddTexture2D(textureSpec);
                    }
                }
            }
        }
        m_Initialized = true;
    }

    Ref<TextureLibrary> TextureLibrary::Create(const std::string& texturesDir)
    {
        if (texturesDir.at(texturesDir.size() - 1) != '/')
            return Ref<TextureLibrary>::Create(texturesDir + "/");
        return Ref<TextureLibrary>::Create(texturesDir);
    }

    Ref<Texture> TextureLibrary::GetTexture(int textureID)
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

    Ref<Texture> TextureLibrary::GetTexture(const std::string textureName)
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

    Ref<Texture2D> TextureLibrary::GetTexture2D(int textureID)
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

    Ref<Texture2D> TextureLibrary::GetTexture2D(const std::string textureName)
    {
        if (m_Collection2D.empty())
            throw std::runtime_error("2D Texture library is empty");

        for (auto iter = m_Collection2D.begin(); iter != m_Collection2D.end(); ++iter)
        {
            auto& texture = *iter;
            if (File::ExtractFilenameWithoutExtension(texture.first, FileExtension::PNG) == textureName)
                return texture.second;
        }

        return nullptr;
        // If no texture is found, return white texture
        //return m_Collection2D.at("white-texture"); // White texture is the first added texture in the 2D collection
    }

    Ref<Texture2D> TextureLibrary::AddTexture2D(const TextureSpecification& textureSpec)
    {
        for (const auto& entry : m_Collection2D)
        {
            if (entry.first == File::ExtractFilenameWithoutExtension(textureSpec.Name))
            {
                LK_CORE_DEBUG("Texture {} already exists in the 2D collection, returning it", textureSpec.Name);
                return entry.second;
            }
        }
        Ref<Texture2D> texture2D = Texture2D::Create(textureSpec);
        m_Collection2D.insert({ textureSpec.Name, texture2D });

        return texture2D;
    }

    Ref<Texture> TextureLibrary::GetWhiteTexture()
    {
        return m_WhiteTexture2D;
    }

    Ref<Texture2D> TextureLibrary::GetWhiteTexture2D()
    {
        //LK_ASSERT(m_Collection2D.at("white-texture") != nullptr);
        //return m_Collection2D.at("white-texture");
        return m_WhiteTexture2D;
    }

    Ref<Texture> TextureLibrary::GetBlackTexture()
    {
        return m_BlackTexture2D;
    }

    Ref<Texture2D> TextureLibrary::GetBlackTexture2D()
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

    std::vector<std::pair<std::string, Ref<Texture2D>>> TextureLibrary::GetTextures2D()
    {
        std::vector<std::pair<std::string, Ref<Texture2D>>> textures;
        textures.push_back({ "white-texture", m_WhiteTexture2D });
        for (auto& entry : m_Collection2D)
        {
            if (entry.second->GetName() == "black-texture" || entry.second->GetName() == "white-texture")
                continue;
            textures.push_back(entry);
        }
        return textures;
    }

    // FIXME
    bool TextureLibrary::VerifyTexturesAreLoaded() const
    {
        return true;
    }


}