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
        spec.Path = "Assets/Textures/white-texture.png";
        spec.Name = "white-texture";
        spec.DebugName = "white-texture";
        m_WhiteTexture = Texture2D::Create(spec);
        m_Collection2D.insert({ "white-texture", m_WhiteTexture });

        Init();
#if 0
        // TODO: Read fileinfo and determine size and other info instead of manually setting it
        // Nanosuit
        {
            std::unordered_map<std::string, Ref<Texture2D>> nanosuitCollection;
            constexpr const char* nanosuitDirectory = "Assets/Meshes/Template/Nanosuit";
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
#endif
    }

    TextureLibrary::~TextureLibrary()
    {
        //m_Collection.clear();
        m_Collection2D.clear();
    }

    void TextureLibrary::Init(bool loadRecursively)
    {
		// Textures: 512x512
		{
			TextureSpecification textureSpec;
			// Grass
			textureSpec.Width = 512;
		    textureSpec.Height = 512;
            textureSpec.Path = "Assets/Textures/grass.png";
            textureSpec.Name = "grass-512x512";
            textureSpec.DebugName = "grass-512x512";
            textureSpec.GenerateMips = true;
			textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Linear;
			AddTexture(textureSpec);

			// Ice Skybox 
            textureSpec.Path = "Assets/Textures/Skybox/back.jpg";
            textureSpec.Name = "skybox-ice-back-512x512";
            textureSpec.DebugName = "skybox-ice-back-512x512";
            textureSpec.GenerateMips = false;
			textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			AddTexture(textureSpec);
		}
		// Textures: 1024x1024
		{
			TextureSpecification textureSpec;
			// Brickwall
			textureSpec.Width = 1024;
		    textureSpec.Height = 1024;
            textureSpec.Path = "Assets/Textures/brickwall.jpg";
            textureSpec.Name = "brickwall";
            textureSpec.DebugName = "brickwall";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			AddTexture(textureSpec);

			// Wood container
			textureSpec.Format = ImageFormat::RGBA32F;
			textureSpec.Width = 512;
			textureSpec.Height = 512;
			textureSpec.Path = "Assets/Textures/container.jpg";
			textureSpec.Name = "wood-container_512x512";
			textureSpec.DebugName = "wood-container_512x512";
			textureSpec.SamplerWrap = TextureWrap::Clamp;
			textureSpec.SamplerFilter = TextureFilter::Nearest;
            textureSpec.GenerateMips = true;
			AddTexture(textureSpec);

		}
		// Textures: 2048x2048 
		{
			TextureSpecification textureSpec;
			// Wood container
			textureSpec.Format = ImageFormat::RGBA32F;
			textureSpec.Width = 2048;
			textureSpec.Height = 2048;
			textureSpec.Path = "Assets/Textures/container.jpg";
			textureSpec.Name = "wood-container";
			textureSpec.DebugName = "wood-container";
			textureSpec.SamplerWrap = TextureWrap::Clamp;
			textureSpec.SamplerFilter = TextureFilter::Linear;
            textureSpec.GenerateMips = true;
			AddTexture(textureSpec);

			// Wood container 2
			textureSpec.Path = "Assets/Textures/container2.png";
			textureSpec.Name = "wood-container2";
			textureSpec.DebugName = "wood-container2";
			AddTexture(textureSpec);

			// Bricks
			textureSpec.Path = "Assets/Textures/bricks_orange.jpg";
			textureSpec.Name = "bricks";
			textureSpec.DebugName = "bricks";
			AddTexture(textureSpec);

			// Åle texture
			textureSpec.Format = ImageFormat::RGBA32F;
			textureSpec.Path = "Assets/Textures/Misc/ale_1024x1024.png";
			textureSpec.Name = "ale1024";
			textureSpec.DebugName = "ale1024";
			AddTexture(textureSpec);

			// Lukas texture
			textureSpec.Path = "Assets/Textures/Misc/lukas_1024.jpg";
			textureSpec.Name = "lukas_1024";
			textureSpec.DebugName = "lukas-1024x1024";
			AddTexture(textureSpec);

			// Metal
            textureSpec.Path = "Assets/Textures/metal.png";
            textureSpec.Name = "metal-ground";
            textureSpec.DebugName = "metal-ground";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			AddTexture(textureSpec);

			// Wood
            textureSpec.Name = "wood";
            textureSpec.DebugName = "wood";
            textureSpec.Path = "Assets/Textures/wood.png";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			AddTexture(textureSpec);

			// Skybox
            textureSpec.Name = "skybox-ice-back";
            textureSpec.DebugName = "skybox-ice-back";
            textureSpec.Path = "Assets/Textures/Skybox/back.jpg";
            textureSpec.GenerateMips = false;
            textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			AddTexture(textureSpec);
        }

        m_Initialized = true;
    }

#if 0
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
#endif

    Ref<Texture2D> TextureLibrary::GetTexture(int textureID)
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

    Ref<Texture2D> TextureLibrary::GetTexture(const std::string textureName)
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

    Ref<Texture2D> TextureLibrary::AddTexture(const TextureSpecification& textureSpec)
    {
        for (const auto& entry : m_Collection2D)
        {
            //LK_CORE_TRACE("Entry: {}   {}", entry.first, entry.second->GetName());
            if (entry.first == File::ExtractFilenameWithoutExtension(textureSpec.Name))
            {
                if (textureSpec.Width == entry.second->GetWidth() && textureSpec.Height == entry.second->GetHeight() || entry.first == textureSpec.Name)
                {
                    LK_CORE_WARN_TAG("TextureLibrary", "Texture {} already exists and has same dimensions as the passed specification, returning it", textureSpec.Name);
                    return entry.second;
                }
                //return entry.second;
            }
        }
        Ref<Texture2D> texture2D = Texture2D::Create(textureSpec);
        m_Collection2D.insert({ textureSpec.Name, texture2D });

        return texture2D;
    }

    Ref<Texture2D> TextureLibrary::GetWhiteTexture()
    {
        return m_WhiteTexture;
    }

    Ref<Texture2D> TextureLibrary::GetBlackTexture()
    {
        return m_BlackTexture;
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
        textures.push_back({ "white-texture", m_WhiteTexture });
        for (auto& entry : m_Collection2D)
        {
            if (entry.second->GetName() == "black-texture" || entry.second->GetName() == "white-texture") 
                continue;
            textures.push_back(entry);
        }
// Causes crashes
#if 0 
        for (auto& collection : m_Collections2D)
        {
            for (auto& entry : collection.second)
            {
                textures.push_back(entry);
            }
        }
#endif
        return textures;
    }

    // FIXME
    bool TextureLibrary::VerifyTexturesAreLoaded() const
    {
        return true;
    }

    bool TextureLibrary::HasTextureWithFilename(const std::string& filename)
    {
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

    void TextureLibrary::AddCollection(std::string_view directoryPath)
    {
        auto directory = std::filesystem::path(directoryPath);
    }


}