#include "LKpch.h"
#include "TextureLibrary.h"


namespace LkEngine {

    static std::vector<std::string> ImageExtensions = { 
        ".jpg", 
        ".jpeg", 
        ".png", 
        ".bmp" 
    };

    namespace fs = std::filesystem;

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

    TextureLibrary::TextureLibrary()
    {
        m_Instance = Ref<TextureLibrary>(this);

		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA32F;
		spec.Width = 200;
		spec.Height = 200;
        spec.SamplerFilter = TextureFilter::None;
        spec.SamplerWrap = TextureWrap::None;
        spec.Name = "white-texture";
        spec.DebugName = "white-texture";
        spec.Path = "assets/Textures/white-texture.png";
        m_WhiteTexture2D = Texture2D::Create(spec);
        m_Collection2D.insert({ "white-texture", m_WhiteTexture2D });

        spec.Name = "black-texture";
		constexpr uint32_t blackTextureData = 0xFF000000;
        m_BlackTexture2D = Texture2D::Create(spec, Buffer(&blackTextureData, sizeof(uint32_t)));
        LK_VERIFY(m_BlackTexture2D, "m_BlackTexture2D is nullptr");
        m_Collection2D.insert({ "black-texture", m_BlackTexture2D });

        // Texture: metal
        {
		    spec.Format = ImageFormat::RGBA32F;
		    spec.Width = 1024;
		    spec.Height = 1024;
            spec.Name = "metal-ground";
            spec.DebugName = "metal-ground";
            spec.Path = "assets/Textures/metal.png";
            spec.GenerateMips = true;
            spec.SamplerWrap = TextureWrap::Repeat;
            spec.SamplerFilter = TextureFilter::Nearest;
            m_Collection2D.insert({ "metal-ground", Texture2D::Create(spec) });
        }

        // Texture: wood
        {
		    spec.Format = ImageFormat::RGBA32F;
		    spec.Width = 1024;
		    spec.Height = 1024;
            spec.Name = "wood-floor";
            spec.DebugName = "wood-floor";
            spec.Path = "assets/Textures/wood.png";
            spec.GenerateMips = true;
            spec.SamplerWrap = TextureWrap::Repeat;
            spec.SamplerFilter = TextureFilter::Linear;
            m_Collection2D.insert({ "wood-floor", Texture2D::Create(spec) });
        }

        // Texture: wood container
        {
		    spec.Format = ImageFormat::RGB;
		    spec.Width = 512;
		    spec.Height = 512;
            spec.Name = "wood-container";
            spec.DebugName = "wood-container";
            spec.Path = "assets/Textures/container.jpg";
            spec.SamplerWrap = TextureWrap::Repeat;
            spec.SamplerFilter = TextureFilter::Linear;
            spec.GenerateMips = false;
            m_Collection2D.insert({ "wood-container", Texture2D::Create(spec) });
        }

        // Skybox
        {
            TextureSpecification skyboxSpec;
            skyboxSpec.Width = 200;
            skyboxSpec.Height = 200;
            skyboxSpec.Name = "skybox-ice-back";
            skyboxSpec.DebugName = "skybox-ice-back";
            skyboxSpec.Path = "assets/Textures/SkyBox/back.jpg";
            skyboxSpec.GenerateMips = false;
            skyboxSpec.Format = ImageFormat::RGB;
            skyboxSpec.SamplerFilter = TextureFilter::Nearest;
            skyboxSpec.SamplerWrap = TextureWrap::Clamp;
            m_Collection2D.insert({ "skybox-ice-back", Texture2D::Create(skyboxSpec) });
        }
        // Misc
        {
            TextureSpecification miscSpec;
            miscSpec.Width = 200;
            miscSpec.Height = 200;
            miscSpec.Name = "lukas-1";
            miscSpec.DebugName = "lukas-1";
            miscSpec.Path = "assets/Textures/Misc/lukas1.JPG";
            miscSpec.GenerateMips = false;
            miscSpec.Format = ImageFormat::RGB;
            miscSpec.SamplerFilter = TextureFilter::Nearest;
            miscSpec.SamplerWrap = TextureWrap::Clamp;
            m_Collection2D.insert({ "lukas-1", Texture2D::Create(miscSpec) });

            miscSpec.Width = 200;
            miscSpec.Height = 200;
            miscSpec.Name = "cowboy-hat";
            miscSpec.DebugName = "cowboy-hat";
            miscSpec.Path = "assets/Textures/Misc/cowboy-hat.png";
            miscSpec.GenerateMips = false;
            miscSpec.Format = ImageFormat::RGBA32F;
            miscSpec.SamplerFilter = TextureFilter::Nearest;
            miscSpec.SamplerWrap = TextureWrap::Clamp;
            m_Collection2D.insert({ "cowboy-hat", Texture2D::Create(miscSpec) });
        }

        // TODO: Read fileinfo and determine size and other info instead of manually setting it
        // Nanosuit
        {
            std::unordered_map<std::string, Ref<Texture2D>> nanosuitCollection;
            constexpr const char* nanosuitDirectory = "assets/Meshes/Template/Nanosuit";
            TextureSpecification nanosuitSpec;

            nanosuitSpec.Width = 1024;
            nanosuitSpec.Height = 1024;
            for (const auto& entry : fs::directory_iterator(nanosuitDirectory))
            {
                if (entry.is_regular_file())
                {
                    std::string filename = entry.path().filename().string();
                    std::string fileExt = filename.substr(filename.size() - 3);
                    if (fileExt != "jpg" && fileExt != "png" && fileExt != "JPG")
                    {
                        LK_CORE_ERROR_TAG("TextureLibrary", "Found unwanted file extension: {}", fileExt);
                        continue;
                    }
                    if (fileExt == "jpg" || fileExt == "JPG") nanosuitSpec.Format = ImageFormat::RGB;
                    if (fileExt == "png") nanosuitSpec.Format = ImageFormat::RGBA;

                    nanosuitSpec.Path = nanosuitDirectory + std::string("/") + filename;
                    nanosuitCollection.insert({ filename, Texture2D::Create(nanosuitSpec) });
                    //LK_CORE_INFO_TAG("TextureLibrary", "Added nanosuit file: {}", filename);
                }
            }
            m_Collections2D.insert({ "Nanosuit", nanosuitCollection });
        }
    }

    TextureLibrary::~TextureLibrary()
    {
        m_Collection.clear();
        m_Collection2D.clear();
    }

    void TextureLibrary::Init(bool loadRecursively)
    {
        m_Initialized = true;
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
        //for (auto& collection : m_Collections2D)
        //{
        //    for (auto& entry : collection.second)
        //    {
        //        textures.push_back(entry);
        //    }
        //}
        return textures;
    }

    // FIXME
    bool TextureLibrary::VerifyTexturesAreLoaded() const
    {
        return true;
    }

    bool TextureLibrary::HasTextureWithFilename(const std::string& filename)
    {
        for (auto& tex3D : m_Collection)
        {
            Ref<Texture>& texture = tex3D.second;
            if (texture->GetPath().filename() == filename)
                return true;
        }
        for (auto& tex2D : m_Collection2D)
        {
            Ref<Texture2D>& texture = tex2D.second;
            if (texture->GetPath().filename() == filename)
                return true;
        }

        return false;
    }

    Ref<Texture> TextureLibrary::TryToGetTextureWithFilename(const std::string& filename)
    {
        //LK_CORE_INFO_TAG("TextureLibrary", "Looking for texture with filename: {}", filename);
        for (auto& tex3D : m_Collection)
        {
            Ref<Texture> texture = tex3D.second;
            if (texture->GetPath().filename() == filename || File::ExtractFilenameWithoutExtension(texture->GetPath().filename().string()) == filename)
                return texture;
        }
        for (auto& tex2D : m_Collection2D)
        {
            Ref<Texture2D> texture = tex2D.second;
            if (texture->GetPath().filename() == filename || File::ExtractFilenameWithoutExtension(texture->GetPath().filename().string()) == filename)
                return texture;
        }
        for (auto& texCollection2D : m_Collections2D)
        {
            for (auto& t : texCollection2D.second)
            {
                Ref<Texture2D> texture = t.second;
                if (texture->GetPath().filename() == filename || File::ExtractFilenameWithoutExtension(texture->GetPath().filename().string()) == filename)
                    return texture;
            }
        }
        LK_CORE_ERROR_TAG("TextureLibrary", "Did not find texture: {}", filename);
        return nullptr;
    }

    TextureCollection& TextureLibrary::GetTextureCollection(std::string_view collectionName)
    {
        for (auto& collection : m_Collections)
        {
            if (collection.first == collectionName)
                return collection.second;
        }
        LK_CORE_ASSERT(false, "Couldnt find TextureCollection {}", collectionName);
    }

    Texture2DCollection& TextureLibrary::GetTexture2DCollection(std::string_view collectionName)
    {
        for (auto& collection : m_Collections2D)
        {
            if (collection.first == collectionName)
                return collection.second;
        }
        LK_CORE_ASSERT(false, "Couldnt find Texture2DCollection {}", collectionName);
    }


}