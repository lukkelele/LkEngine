#pragma once

#include <filesystem>
#include <stb_image/stb_image.h>

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Memory/Buffer.h"

#include "BlendingSpecification.h"
#include "Color.h"

#include "LkEngine/Core/Event/AssetEvent.h"


namespace LkEngine {

	/**
	 * EImageFormat
	 */
	enum class EImageFormat
	{
		None = 0,
		RED8UN,
		RED8UI,
		RED16UI,
		RED32UI,
		RED32F,
		RG8,
		RG16F,
		RG32F,
		RGB,
		RGBA,

		RGB8,
		RGBA8,

		RGBA16F,
		RGBA32F,

		B10R11G11UF,

		SRGB,
		SRGBA,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		Depth = DEPTH24STENCIL8,
	};

	/**
	 * EImageUsage
	 */
	enum class EImageUsage
	{
		None = 0,
		Texture,
		Attachment,
		Storage,
	};

	/**
	 * ETextureWrap
	 */
	enum class ETextureWrap
	{
		None = 0,
		Clamp,
		Repeat
	};

	enum class ETextureFilter
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class ETextureType
	{
		None = 0,
		Texture2D,
		TextureCube
	};

	enum class ETextureAnistropicFiltering
	{
		None = 0,
		Bilnear,
		Trilnear
	};

	enum class ETextureUniformType : uint8_t
	{
		Diffuse = 0,
		Specular,
		Normal,
		Height,
		Emissive,
		DiffuseRoughness,
	};

	struct FTextureSpecification
	{
		std::string Path = "";
		std::string Name = "";
		uint32_t Width = 1;
		uint32_t Height = 1;
		bool GenerateMips = true;

		EImageFormat Format = EImageFormat::RGBA;
		ETextureWrap SamplerWrap = ETextureWrap::Clamp;
		ETextureFilter SamplerFilter = ETextureFilter::Linear;

		ETextureUniformType UniformType = ETextureUniformType::Diffuse;

		bool Storage = false;
		bool StoreLocally = false;

		std::string DebugName;
	};

	struct FImageSubresourceRange
	{
		uint32_t BaseMip = 0;
		uint32_t MipCount = UINT_MAX;
		uint32_t BaseLayer = 0;
		uint32_t LayerCount = UINT_MAX;
	};

	union ImageClearValue
	{
		glm::vec4 FloatValues;
		glm::ivec4 IntValues;
		glm::uvec4 UIntValues;
	};

	namespace ImageUtils 
	{
		static uint32_t GetMemorySize(const EImageFormat ImageFormat, const uint32_t Width, const uint32_t Height);
		static uint32_t CalculateMipCount(const uint32_t Width, const uint32_t Height);
		static uint32_t BytesPerPixel(const EImageFormat ImageFormat);
	}

	struct FImageSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Mips = 1;
		uint32_t Layers = 1;
		std::string Path = "";
		uint64_t Size = 0;

		EImageFormat Format = EImageFormat::RGBA;
		EImageUsage Usage = EImageUsage::Texture;
		ETextureWrap Wrap = ETextureWrap::Clamp;
		ETextureFilter Filter = ETextureFilter::Linear;
		ETextureAnistropicFiltering AnistropicFiltering = ETextureAnistropicFiltering::Trilnear;

		bool Deinterleaved = false;
		bool Transfer = false; // Will it be used for transfer ops?

		std::string Name{};
		std::string DebugName{};

		FImageSpecification() = default;
		FImageSpecification(const FTextureSpecification& InSpecification)
			: Width(InSpecification.Width)
			, Height(InSpecification.Height)
			, Path(InSpecification.Path)
			, Format(InSpecification.Format)
			, Filter(InSpecification.SamplerFilter)
			, Wrap(InSpecification.SamplerWrap)
			, Name(InSpecification.Name)
			, DebugName(InSpecification.DebugName)
		{
			InSpecification.GenerateMips ? Mips = ImageUtils::CalculateMipCount(InSpecification.Width, InSpecification.Height) : Mips = 1;
			Size = ImageUtils::GetMemorySize(InSpecification.Format, InSpecification.Width, InSpecification.Height);
		}
		~FImageSpecification() = default;
	};
	
	//-------------------------------------------------------------------------------
	// Image
	//-------------------------------------------------------------------------------
	class LImage : public LObject
	{
	public:
		virtual ~LImage() = default;
	
		virtual void SetData(const void* InData) = 0;
		virtual void Resize(const uint32_t InWidth, const uint32_t InHeight) = 0;

		virtual FBuffer GetBuffer() const = 0;
		virtual FBuffer& GetBuffer() = 0;

		virtual void Invalidate() = 0;
		virtual void RT_Invalidate() = 0;

		virtual LRendererID& GetRendererID() = 0;
		virtual LRendererID GetRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual const FImageSpecification& GetSpecification() const = 0;
	
		virtual void Release() = 0;
		virtual void AllocateMemory(const uint64_t InSize) = 0;

		/** Factory functions. */
		static TObjectPtr<LImage> Create(const FImageSpecification& InSpecification, FBuffer InBuffer);
		static TObjectPtr<LImage> Create(const FImageSpecification& InSpecification, void* InData = nullptr);
	};


	/**
	 * LImage2D
	 */
	class LImage2D : public LImage
	{
	public:
		virtual ~LImage2D() = default;

		virtual void Resize(const uint32_t InWidth, const uint32_t InHeight) = 0;

		/** Factory functions. */
		static TObjectPtr<LImage2D> Create(const FImageSpecification& InSpecification, FBuffer InBuffer);
		static TObjectPtr<LImage2D> Create(const FImageSpecification& InSpecification, void* InData = nullptr);
	};


	namespace ImageUtils 
	{
		FORCEINLINE static int64_t GetImageFormat(const EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGBA:	return 4;
				case EImageFormat::RGBA32F:	return 16;
				case EImageFormat::None:	return 0;
			}

			return 0;
		}

		FORCEINLINE static uint32_t GetFormatBPP(const EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGB:
				case EImageFormat::RGBA8:     return 4;
				case EImageFormat::RGBA:      return 4;
				case EImageFormat::RGBA16F:   return 2 * 4;
				case EImageFormat::RGBA32F:   return 4 * 4;
			}

			LK_CORE_ASSERT(false, "GetFormatBPP failed, format not recognized");
			return 0;
		}

		FORCEINLINE static uint32_t GetMemorySize(const EImageFormat ImageFormat, const uint32_t Width, const uint32_t Height)
		{
			return (Width * Height * GetFormatBPP(ImageFormat));
		}

		static uint32_t BytesPerPixel(const EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGBA:    return 4;
				case EImageFormat::RGBA32F: return 16;
				case EImageFormat::None:	return 0;
			}

			return 0;
		}

		FORCEINLINE static uint32_t CalculateMipCount(const uint32_t Width, const uint32_t Height)
		{
			return static_cast<uint32_t>(std::floor(std::log2(glm::min(Width, Height))) + 1);
		}

		FORCEINLINE static bool IsDepthFormat(const EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

	}

	namespace Enum
	{
		FORCEINLINE static constexpr const char* ToString(const EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RG8:               return "RG8";
				case EImageFormat::RGB:               return "RGB";
				case EImageFormat::RGBA:              return "RGBA";
				case EImageFormat::RG16F:             return "RG16F";
				case EImageFormat::RGBA8:             return "RGBA8";
				case EImageFormat::RGBA16F:           return "RGBA16F";
				case EImageFormat::RGBA32F:           return "RGBA32F";
				case EImageFormat::SRGB:              return "SRGB";
				case EImageFormat::SRGBA:             return "SRGBA";
				case EImageFormat::RED32F:            return "RED32F";
				case EImageFormat::RED8UI:            return "RED8UI";
				case EImageFormat::RED8UN:            return "RED8UN";
				case EImageFormat::RED16UI:			  return "RED16UI";
				case EImageFormat::RED32UI:           return "RED32UI";
				case EImageFormat::B10R11G11UF:       return "B10R11G11UF";
				case EImageFormat::DEPTH24STENCIL8:   return "DEPTH24STENCIL8";
			}

			LK_CORE_ASSERT(false, "Invalid image format");
			return "";
		}
	}

}
