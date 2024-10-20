#include "LKpch.h"
#include "TextureLibrary.h"


namespace LkEngine {

    static std::vector<std::string> ImageExtensions = { 
        ".jpg", 
        ".jpeg", 
        ".png", 
        ".bmp" 
    };

    static constexpr std::string_view ExtractFileNameWithoutExtension(std::string_view FilePath)
    {
        /* Find the last path separator('/' or '\\') in the string. */
		const std::size_t LastSlash = FilePath.find_last_of("/\\");
		
		/* Extract the filename(the part after the last separator). */
		std::string_view FileName = (LastSlash == std::string_view::npos) ? FilePath : FilePath.substr(LastSlash + 1);

		/* Find the last dot(.) to remove the extension. */
		const std::size_t LastDot = FileName.find_last_of('.');

		/* Return the substring without the extension. */
		return (LastDot == std::string_view::npos) ? FileName : FileName.substr(0, LastDot);
	}

    LTextureLibrary::LTextureLibrary()
    {
        m_Collection2D.clear();

        /* Create white texture. */
		FTextureSpecification Specification;
		Specification.Format = EImageFormat::RGBA32F;
		Specification.Width = 2048;
		Specification.Height = 2048;
        Specification.SamplerFilter = ETextureFilter::None;
        Specification.SamplerWrap = ETextureWrap::None;
        Specification.Path = "Assets/Textures/white-texture.png";
        Specification.Name = "white-texture";
        Specification.DebugName = "white-texture";
        m_WhiteTexture = LTexture2D::Create(Specification);

        m_Collection2D.insert({ "white-texture", m_WhiteTexture });
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
			FTextureSpecification TextureSpecification;

			// Grass
			TextureSpecification.Width = 512;
		    TextureSpecification.Height = 512;
            TextureSpecification.Path = "Assets/Textures/grass.png";
            TextureSpecification.Name = "grass-512x512";
            TextureSpecification.DebugName = "grass-512x512";
            TextureSpecification.GenerateMips = true;
			TextureSpecification.Format = EImageFormat::RGBA32F;
            TextureSpecification.SamplerWrap = ETextureWrap::Repeat;
            TextureSpecification.SamplerFilter = ETextureFilter::Linear;
			AddTexture(TextureSpecification);

			// Ice Skybox 
            TextureSpecification.Path = "Assets/Textures/Skybox/back.jpg";
            TextureSpecification.Name = "skybox-ice-back-512x512";
            TextureSpecification.DebugName = "skybox-ice-back-512x512";
            TextureSpecification.GenerateMips = false;
			TextureSpecification.Format = EImageFormat::RGBA32F;
            TextureSpecification.SamplerWrap = ETextureWrap::Clamp;
            TextureSpecification.SamplerFilter = ETextureFilter::Nearest;
			AddTexture(TextureSpecification);
		}

		// Textures: 1024x1024
		{
			FTextureSpecification TextureSpecification;
			// Brickwall
			TextureSpecification.Width = 1024;
		    TextureSpecification.Height = 1024;
            TextureSpecification.Path = "Assets/Textures/brickwall.jpg";
            TextureSpecification.Name = "brickwall";
            TextureSpecification.DebugName = "brickwall";
            TextureSpecification.GenerateMips = true;
            TextureSpecification.SamplerWrap = ETextureWrap::Clamp;
            TextureSpecification.SamplerFilter = ETextureFilter::Nearest;
			AddTexture(TextureSpecification);

			// Wood container
			TextureSpecification.Format = EImageFormat::RGBA32F;
			TextureSpecification.Width = 512;
			TextureSpecification.Height = 512;
			TextureSpecification.Path = "Assets/Textures/container.jpg";
			TextureSpecification.Name = "wood-container_512x512";
			TextureSpecification.DebugName = "wood-container_512x512";
			TextureSpecification.SamplerWrap = ETextureWrap::Clamp;
			TextureSpecification.SamplerFilter = ETextureFilter::Nearest;
            TextureSpecification.GenerateMips = true;
			AddTexture(TextureSpecification);

		}
		// Textures: 2048x2048 
		{
            FTextureSpecification TextureSpecification{};
			// Wood container
			TextureSpecification.Format = EImageFormat::RGBA32F;
			TextureSpecification.Width = 2048;
			TextureSpecification.Height = 2048;
			TextureSpecification.Path = "Assets/Textures/container.jpg";
			TextureSpecification.Name = "wood-container";
			TextureSpecification.DebugName = "wood-container";
			TextureSpecification.SamplerWrap = ETextureWrap::Clamp;
			TextureSpecification.SamplerFilter = ETextureFilter::Linear;
            TextureSpecification.GenerateMips = true;
			AddTexture(TextureSpecification);

			// Wood container 2
			TextureSpecification.Path = "Assets/Textures/container2.png";
			TextureSpecification.Name = "wood-container2";
			TextureSpecification.DebugName = "wood-container2";
			AddTexture(TextureSpecification);

			// Bricks
			TextureSpecification.Path = "Assets/Textures/bricks_orange.jpg";
			TextureSpecification.Name = "bricks";
			TextureSpecification.DebugName = "bricks";
			AddTexture(TextureSpecification);

			// Åle texture
			TextureSpecification.Format = EImageFormat::RGBA32F;
			TextureSpecification.Path = "Assets/Textures/Misc/ale_1024x1024.png";
			TextureSpecification.Name = "ale1024";
			TextureSpecification.DebugName = "ale1024";
			AddTexture(TextureSpecification);

			// Lukas texture
			TextureSpecification.Path = "Assets/Textures/Misc/lukas_1024.jpg";
			TextureSpecification.Name = "lukas_1024";
			TextureSpecification.DebugName = "lukas-1024x1024";
			AddTexture(TextureSpecification);

			// Metal
            TextureSpecification.Path = "Assets/Textures/metal.png";
            TextureSpecification.Name = "metal-ground";
            TextureSpecification.DebugName = "metal-ground";
            TextureSpecification.GenerateMips = true;
            TextureSpecification.SamplerWrap = ETextureWrap::Repeat;
            TextureSpecification.SamplerFilter = ETextureFilter::Nearest;
			AddTexture(TextureSpecification);

			// Wood
            TextureSpecification.Name = "wood";
            TextureSpecification.DebugName = "wood";
            TextureSpecification.Path = "Assets/Textures/wood.png";
            TextureSpecification.GenerateMips = true;
            TextureSpecification.SamplerWrap = ETextureWrap::Clamp;
            TextureSpecification.SamplerFilter = ETextureFilter::Nearest;
			AddTexture(TextureSpecification);

			// Skybox
            TextureSpecification.Name = "skybox-ice-back";
            TextureSpecification.DebugName = "skybox-ice-back";
            TextureSpecification.Path = "Assets/Textures/Skybox/back.jpg";
            TextureSpecification.GenerateMips = false;
            TextureSpecification.Format = EImageFormat::RGBA32F;
            TextureSpecification.SamplerWrap = ETextureWrap::Clamp;
            TextureSpecification.SamplerFilter = ETextureFilter::Nearest;
			AddTexture(TextureSpecification);
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

        for (auto Iter = m_Collection2D.begin(); Iter != m_Collection2D.end(); ++Iter)
        {
            auto& texture = *Iter;
            if (texture.second->GetRendererID() == textureID)
            {
                return texture.second;
            }
        }
    } 

    TObjectPtr<LTexture2D> LTextureLibrary::GetTexture(std::string_view TextureName)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        if (m_Collection2D.empty())
        {
            LK_CORE_ASSERT(false, "2D Texture Library is empty");
            return nullptr;
        }

        for (auto Iter = m_Collection2D.begin(); Iter != m_Collection2D.end(); ++Iter)
        {
            auto& Texture = *Iter;
            if (ExtractFileNameWithoutExtension(Texture.first) == TextureName)
            {
                return Texture.second;
            }
        }

        return nullptr;
    }

    TObjectPtr<LTexture2D> LTextureLibrary::AddTexture(const FTextureSpecification& TextureSpecification)
    {
        for (const TTexture2DPair& Entry : m_Collection2D)
        {
            if (Entry.first == ExtractFileNameWithoutExtension(TextureSpecification.Name))
            {
                if (((TextureSpecification.Width == Entry.second->GetWidth()) 
                    && (TextureSpecification.Height == Entry.second->GetHeight()))
                    || (Entry.first == TextureSpecification.Name))
                {
                    LK_CORE_WARN_TAG("TextureLibrary", "Texture {} already exists and has same dimensions "
                                     "as the passed Specificationification, returning it", TextureSpecification.Name);
                    return Entry.second;
                }
            }
        }

        TObjectPtr<LTexture2D> texture2D = LTexture2D::Create(TextureSpecification);
        m_Collection2D.insert({ TextureSpecification.Name, texture2D });

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
        return std::ranges::any_of(m_Collection2D, [&](const TTexture2DPair& Entry)
		{
		    return (Entry.second->GetFilename() == FileName);
		});
    }

    TObjectPtr<LTexture> LTextureLibrary::TryToGetTextureWithFilename(const std::string& Filename)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");

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

			if (auto Texture = std::ranges::find_if(CurrentTexture2DMap.second, CheckEntry); 
                Texture != CurrentTexture2DMap.second.end())
			{
                /* Return the found texture. */
				return (Texture->second);
			}
		}

        LK_CORE_ERROR_TAG("TextureLibrary", "Did not find texture: {}", Filename);
        return nullptr;
    }

    TTextureMap& LTextureLibrary::GetTextureCollection(std::string_view collectionName)
    {
        LK_CORE_ASSERT(bInitialized, "TextureLibrary is not initialized");
        /// UPDATE: Dont Iterate like this here
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