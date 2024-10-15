#include "LKpch.h"
#include "TextureLibrary.h"


namespace LkEngine {

    static std::vector<std::string> ImageExtensions = { 
        ".jpg", 
        ".jpeg", 
        ".png", 
        ".bmp" 
    };

    static FileExtension DetermineExtension(const std::filesystem::directory_entry& Entry)
    {
        if (Entry.is_regular_file() == false)
        {
            return FileExtension::Unknown;
        }

        const std::string Filename = Entry.path().filename().string();
        const std::string filepath = Entry.path().string();
        const std::string fileExtension = Entry.path().extension().string();

        if (fileExtension == "png")
        {
            return FileExtension::PNG;
        }
        else if (fileExtension == "jpg")
        {
            return FileExtension::JPG;
        }

        return FileExtension::Unknown;
    }

    LTextureLibrary::LTextureLibrary()
    {
        m_Collection2D.clear();

        /* Create white texture. */
		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA32F;
		spec.Width = 2048;
		spec.Height = 2048;
        spec.SamplerFilter = TextureFilter::None;
        spec.SamplerWrap = TextureWrap::None;
        spec.Path = "Assets/Textures/white-texture.png";
        spec.Name = "white-texture";
        spec.DebugName = "white-texture";
        m_WhiteTexture = LTexture2D::Create(spec);
        m_Collection2D.insert({ "white-texture", m_WhiteTexture });

        //Initialize();

    #if LK_TEST_LoadNanoSuit
        // TODO: Read fileinfo and determine size and other info instead of manually setting it
        // Nanosuit
        {
            std::unordered_map<std::string, TObjectPtr<LTexture2D>> nanosuitCollection;
            constexpr const char* nanosuitDirectory = "Assets/Meshes/Template/Nanosuit";
            TextureSpecification nanosuitSpec;

            nanosuitSpec.Width = 1024;
            nanosuitSpec.Height = 1024;
            for (const auto& Entry : fs::directory_iterator(nanosuitDirectory))
            {
                if (Entry.is_regular_file())
                {
                    std::string Filename = Entry.path().Filename().string();
                    std::string fileExt = Filename.substr(Filename.size() - 3);
                    if (fileExt != "jpg" && fileExt != "png" && fileExt != "JPG")
                    {
                        LK_CORE_ERROR_TAG("TextureLibrary", "Found unwanted file extension: {}", fileExt);
                        continue;
                    }
                    if (fileExt == "jpg" || fileExt == "JPG") nanosuitSpec.Format = ImageFormat::RGB;
                    if (fileExt == "png") nanosuitSpec.Format = ImageFormat::RGBA;

                    nanosuitSpec.Path = nanosuitDirectory + std::string("/") + Filename;
                    nanosuitCollection.insert({ Filename, Texture2D::Create(nanosuitSpec) });
                }
            }
            m_Collections2D.insert({ "Nanosuit", nanosuitCollection });
        }
    #endif
    }

    LTextureLibrary::~LTextureLibrary()
    {
        if (m_Collection2D.size() > 0)
        {
            /* Clean up assets. */

            m_Collection2D.clear();
        }
    }

    void LTextureLibrary::Initialize(bool loadRecursively)
    {
        LObject::Initialize();

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
            TextureSpecification textureSpec{};
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

        bInitialized = true;
    }

    /// FIXME: 
    TObjectPtr<LTexture2D> LTextureLibrary::GetTexture(const int textureID)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        if (m_Collection2D.empty())
        {
            LK_CORE_ASSERT(false, "2D Texture Library is empty");
            return nullptr;
        }

        for (auto iter = m_Collection2D.begin(); iter != m_Collection2D.end(); ++iter)
        {
            auto& texture = *iter;
            if (texture.second->GetRendererID() == textureID)
            {
                return texture.second;
            }
        }
    } 

    TObjectPtr<LTexture2D> LTextureLibrary::GetTexture(const std::string textureName)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        if (m_Collection2D.empty())
        {
            LK_CORE_ASSERT(false, "2D Texture Library is empty");
            return nullptr;
        }

        //auto FindTexture = [&]()

        for (auto iter = m_Collection2D.begin(); iter != m_Collection2D.end(); ++iter)
        {
            auto& texture = *iter;
            if (File::ExtractFilenameWithoutExtension(texture.first, FileExtension::PNG) == textureName)
            {
                return texture.second;
            }
        }

        return nullptr;
    }

    TObjectPtr<LTexture2D> LTextureLibrary::AddTexture(const TextureSpecification& textureSpec)
    {
        for (const TTexture2DPair& Entry : m_Collection2D)
        {
            if (Entry.first == File::ExtractFilenameWithoutExtension(textureSpec.Name))
            {
                if (((textureSpec.Width == Entry.second->GetWidth()) 
                    && (textureSpec.Height == Entry.second->GetHeight()))
                    || (Entry.first == textureSpec.Name))
                {
                    LK_CORE_WARN_TAG("TextureLibrary", "Texture {} already exists and has same dimensions "
                                     "as the passed specification, returning it", textureSpec.Name);
                    return Entry.second;
                }
            }
        }

        TObjectPtr<LTexture2D> texture2D = LTexture2D::Create(textureSpec);
        m_Collection2D.insert({ textureSpec.Name, texture2D });

        return texture2D;
    }

    TObjectPtr<LTexture2D> LTextureLibrary::GetWhiteTexture()
    {
        LK_CORE_VERIFY(m_WhiteTexture);
        return m_WhiteTexture;
    }

    TObjectPtr<LTexture2D> LTextureLibrary::GetBlackTexture()
    {
        LK_CORE_VERIFY(m_BlackTexture);
        return m_BlackTexture;
    }

    void LTextureLibrary::RenameTexture(const std::string& name, const std::string& newName)
    {
        auto Iter = m_Collection2D.find(name);
        if (Iter == m_Collection2D.end())
        {
            return;
        }

        m_Collection2D[name] = Iter->second;
        m_Collection2D.erase(Iter);
    }


    std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>> LTextureLibrary::GetTextures2D()
    {
        std::vector<TTexture2DPair> textures;
        textures.push_back({ "white-texture", m_WhiteTexture });

#if 0
        for (const TTexture2DPair& Entry : m_Collection2D)
        {
            if ((Entry.second->GetName() == "black-texture")
                || (Entry.second->GetName() == "white-texture"))
            {
                continue;
            }

            textures.push_back(Entry);
        }

			| std::views::filter([](const TTexture2DPair& Entry)
			{
				return (Entry.second->GetName() != "white-texture")
					&& (Entry.second->GetName() != "black-texture");
			}), 
#endif

        auto RetrieveTexture = [&](const TTexture2DPair& Entry)
        {
			return (Entry.second->GetName() != "white-texture")
				&& (Entry.second->GetName() != "black-texture");
        };

        /* Copy all entries. */
        std::ranges::copy(
            m_Collection2D
			| std::views::filter(RetrieveTexture)
            , std::back_inserter(textures)
        );

        return textures;
    }

    int LTextureLibrary::GetTextures2D(std::vector<TTexture2DPair>& TextureContainer)
    {
        if (TextureContainer.size() > 0)
        {
            LK_CORE_WARN_TAG("TextureLibrary", "Texture container is not empty");
        }

        TextureContainer.clear();
        TextureContainer.push_back({ "white-texture", m_WhiteTexture });

        auto RetrieveTexture = [&](const TTexture2DPair& Entry)
        {
			return (Entry.second->GetName() != "white-texture")
				&& (Entry.second->GetName() != "black-texture");
        };

        /* Copy all entries to the container. */
        std::ranges::copy(
            m_Collection2D
			| std::views::filter(RetrieveTexture)
            , std::back_inserter(TextureContainer)
        );

        return TextureContainer.size();
    }

    bool LTextureLibrary::VerifyTexturesAreLoaded()
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        return true;
    }

    bool LTextureLibrary::HasTextureWithFilename(const std::string& FileName)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
    #if 0 /// OLD IMPL
        for (const TTexture2DPair& Entry : m_Collection2D)
        {
            const TObjectPtr<LTexture2D>& texture = Entry.second;
            if (texture->GetPath().Filename() == Filename)
            {
                return true;
            }
        }

        return false;
    #endif

        return std::ranges::any_of(m_Collection2D, [&](const TTexture2DPair& Entry)
		{
		    return (Entry.second->GetFilename() == FileName);
		});
    }

    TObjectPtr<LTexture> LTextureLibrary::TryToGetTextureWithFilename(const std::string& Filename)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");

#if OLD_IMPL
        for (const TTexture2DPair& Entry : m_Collection2D)
        {
            if (TObjectPtr<LTexture2D> texture = Entry.second)
            {
				if (texture->GetFilename() == Filename)
				{
					return texture;
				}
            }
        }

        for (const std::pair<std::string, TTexture2DMap>& CurrentTexture2DMap : m_Collections2D)
        {
            for (const TTexture2DPair& Entry : CurrentTexture2DMap.second)
            {
                if (TObjectPtr<LTexture2D> texture = Entry.second)
                {
					if (texture->GetFilename() == Filename)
					{
						return texture;
					}
                }
            }
        }
#endif

        /* Search in 2D collection to begin. */
		if (auto Texture = std::ranges::find_if(m_Collection2D, [&](const TTexture2DPair& Entry)
		{
			return Entry.second && Entry.second->GetFilename() == Filename;
		}); Texture != m_Collection2D.end())
		{
			return Texture->second;  // Return the found texture
		}

		/* Search in nested 2D collection (nested maps). */
		for (const std::pair<std::string, TTexture2DMap>& CurrentTexture2DMap : m_Collections2D)
		{
            auto CheckEntry = [&](const TTexture2DPair& Entry)
            {
                return (Entry.second && (Entry.second->GetFilename() == Filename));
            };
    #if 0
			if (auto Texture = std::ranges::find_if(CurrentTexture2DMap.second, [&](const TTexture2DPair& Entry)
			{
				return Entry.second && Entry.second->GetFilename() == Filename;
			}); Texture != CurrentTexture2DMap.second.end())
			{
				return Texture->second; /* Return the found texture. */
			}
    #endif
			if (auto Texture = std::ranges::find_if(CurrentTexture2DMap.second, CheckEntry); 
                Texture != CurrentTexture2DMap.second.end())
			{
				return Texture->second; /* Return the found texture. */
			}
		}

        LK_CORE_ERROR_TAG("TextureLibrary", "Did not find texture: {}", Filename);
        return nullptr;
    }

    TTextureMap& LTextureLibrary::GetTextureCollection(std::string_view collectionName)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        /// UPDATE: Dont iterate like this here
        for (auto& collection : m_Collections)
        {
            if (collection.first == collectionName)
            {
                return collection.second;
            }
        }

        LK_CORE_ASSERT(false, "Failed to find collection: {}", collectionName);
        return m_Collections.at(0); /* Will crash. */
    }

    TTexture2DMap& LTextureLibrary::GetTexture2DCollection(std::string_view collectionName)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        for (auto& collection : m_Collections2D)
        {
            if (collection.first == collectionName)
            {
                return collection.second;
            }
        }

        LK_CORE_ASSERT(false, "Failed to find collection: {}", collectionName);
        return m_Collections2D.at(0); /* Will crash. */
    }

    void LTextureLibrary::AddCollection(std::string_view InDirectoryPath)
    {
        std::filesystem::path Directory = std::filesystem::path(InDirectoryPath);
        LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    LTextureLibrary& LTextureLibrary::Get()
    {
        static LTextureLibrary TextureLibrary;
        return TextureLibrary;
    }

}