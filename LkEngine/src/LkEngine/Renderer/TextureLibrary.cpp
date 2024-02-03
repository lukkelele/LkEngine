#include "LKpch.h"
#include "TextureLibrary.h"


namespace LkEngine {

    static std::vector<std::string> ImageExtensions = { ".jpg", ".jpeg", ".png", ".bmp" };

    namespace fs = std::filesystem;

    FileExtension DetermineExtension(const std::filesystem::directory_entry& entry)
    {
        if (entry.is_regular_file() == false)
            return FileExtension::Unknown;

        std::string filename = entry.path().filename().string();
        std::string filepath = entry.path().string();
        std::string fileExtension = entry.path().extension().string();

        if (fileExtension == "png")      return FileExtension::PNG;
        else if (fileExtension == "jpg") return FileExtension::JPG;
        return FileExtension::Unknown;
    }

    TextureLibrary::TextureLibrary()
    {
        m_Instance = Ref<TextureLibrary>(this);
        m_Collection2D.clear();

        // White Texture
		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA32F;
		spec.Width = 2048;
		spec.Height = 2048;
        spec.SamplerFilter = TextureFilter::None;
        spec.SamplerWrap = TextureWrap::None;
        spec.Name = "white-texture";
        spec.DebugName = "white-texture";
        spec.Path = "assets/Textures/white-texture.png";
        m_WhiteTexture2D = Texture2D::Create(spec);
        m_Collection2D.insert({ "white-texture", m_WhiteTexture2D });

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
            //LK_CORE_TRACE("Entry: {}   {}", entry.first, entry.second->GetName());
            if (entry.first == File::ExtractFilenameWithoutExtension(textureSpec.Name))
            {
                //if (textureSpec.Width == entry.second->GetWidth() && textureSpec.Height == entry.second->GetHeight() || entry.first == textureSpec.Name)
                //{
                //    LK_CORE_WARN_TAG("TextureLibrary", "Texture {} already exists and has same dimensions as the passed specification, returning it", textureSpec.Name);
                //    return entry.second;
                //}
                return entry.second;
            }
        }
        Ref<Texture2D> texture2D = Texture2D::Create(textureSpec);
        m_Collection2D.insert({ textureSpec.Name, texture2D });

        return texture2D;
    }

    Ref<Texture> TextureLibrary::GetWhiteTexture()
    {
        return m_WhiteTexture;
    }

    Ref<Texture2D> TextureLibrary::GetWhiteTexture2D()
    {
        return m_WhiteTexture2D;
    }

    Ref<Texture> TextureLibrary::GetBlackTexture()
    {
        return m_BlackTexture;
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