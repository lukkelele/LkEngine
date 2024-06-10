#pragma once

#include <filesystem>
#include <stb_image/stb_image.h>

#include "LkEngine/Core/Memory/Buffer.h"

#include "BlendingSpecification.h"
#include "Color.h"

#include "LkEngine/Core/Event/AssetEvent.h"


namespace LkEngine {

	enum class ImageFormat
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

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	enum class ImageUsage
	{
		None = 0,
		Texture,
		Attachment,
		Storage,
	};

	enum class TextureWrap
	{
		None = 0,
		Clamp,
		Repeat
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class TextureType
	{
		None = 0,
		Texture2D,
		TextureCube
	};

	enum class TextureAnistropicFiltering
	{
		None = 0,
		Bilnear,
		Trilnear
	};

	enum class TextureUniformType : uint8_t
	{
		Diffuse = 0,
		Specular,
		Normal,
		Height,
		Emissive,
		DiffuseRoughness,
	};

	struct TextureSpecification
	{
		std::string Path = "";
		std::string Name = "";
		uint32_t Width = 1;
		uint32_t Height = 1;
		bool GenerateMips = true;

		ImageFormat Format = ImageFormat::RGBA;
		TextureWrap SamplerWrap = TextureWrap::Clamp;
		TextureFilter SamplerFilter = TextureFilter::Linear;

		TextureUniformType UniformType;

		bool Storage = false;
		bool StoreLocally = false;

		std::string DebugName;
	};

	struct ImageSubresourceRange
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

	namespace Utils {
		static uint32_t GetMemorySize(ImageFormat format, uint32_t width, uint32_t height);
		static uint32_t CalculateMipCount(uint32_t width, uint32_t height);
		static uint32_t BytesPerPixel(ImageFormat format);
	}

	struct ImageSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Mips = 1;
		uint32_t Layers = 1;
		std::string Path = "";
		uint64_t Size = 0;

		ImageFormat Format = ImageFormat::RGBA;
		ImageUsage Usage = ImageUsage::Texture;
		TextureWrap Wrap = TextureWrap::Clamp;
		TextureFilter Filter = TextureFilter::Linear;
		TextureAnistropicFiltering AnistropicFiltering = TextureAnistropicFiltering::Trilnear;

		bool Deinterleaved = false;
		bool Transfer = false; // Will it be used for transfer ops?

		std::string Name = "";
		std::string DebugName;

		ImageSpecification() = default;
		ImageSpecification(const TextureSpecification& textureSpec)
			: Width(textureSpec.Width), Height(textureSpec.Height)
			, Path(textureSpec.Path), Format(textureSpec.Format)
			, Filter(textureSpec.SamplerFilter), Wrap(textureSpec.SamplerWrap)
			, Name(textureSpec.Name), DebugName(textureSpec.DebugName)
		{
			textureSpec.GenerateMips ? Mips = Utils::CalculateMipCount(textureSpec.Width, textureSpec.Height) : Mips = 1;
			Size = Utils::GetMemorySize(textureSpec.Format, textureSpec.Width, textureSpec.Height);
		}
		~ImageSpecification() = default;
	};
	
	//-------------------------------------------------------------------------------
	// Image
	//-------------------------------------------------------------------------------
	class Image : public RefCounted
	{
	public:
		virtual ~Image() = default;
	
		virtual void SetData(const void* data) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Buffer GetBuffer() const = 0;
		virtual Buffer& GetBuffer() = 0;

		virtual void Invalidate() = 0;
		virtual void RT_Invalidate() = 0;

		virtual RendererID& GetRendererID() = 0;
		virtual RendererID GetRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		//virtual const ImageSpecification GetSpecification() const = 0;
		//virtual ImageSpecification& GetSpecification() = 0;
		virtual const ImageSpecification& GetSpecification() const = 0;
	
		virtual void Release() = 0;
		virtual void AllocateMemory(uint64_t size) = 0;

		static Ref<Image> Create(ImageSpecification spec, Buffer buffer);
		static Ref<Image> Create(ImageSpecification spec, void* data = nullptr);
	};


	//-------------------------------------------------------------------------------
	// Image2D
	//-------------------------------------------------------------------------------
	class Image2D : public Image
	{
	public:
		virtual ~Image2D() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		static Ref<Image2D> Create(ImageSpecification spec, Buffer buffer);
		static Ref<Image2D> Create(ImageSpecification spec, void* data = nullptr);
	};


	namespace Utils {

		static int64_t GetImageFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA:    return 4;
				case ImageFormat::RGBA32F: return 16;
				case ImageFormat::None:	   return 0;
			}
			return 0;
		}

		static uint32_t GetFormatBPP(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:
				case ImageFormat::RGBA8:   return 4;
				case ImageFormat::RGBA:    return 4;
				case ImageFormat::RGBA16F: return 2 * 4;
				case ImageFormat::RGBA32F: return 4 * 4;
			}
			LK_CORE_ASSERT(false, "GetFormatBPP failed, format not recognized!");
			return 0;
		}

		static uint32_t GetMemorySize(ImageFormat format, uint32_t width, uint32_t height)
		{
			return width * height * GetFormatBPP(format);
		}

		static std::string ImageFormatToString(const ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RG8:               return "RG8";
				case ImageFormat::RGB:               return "RGB";
				case ImageFormat::RGBA:              return "RGBA";
				case ImageFormat::RG16F:             return "RG16F";
				case ImageFormat::RGBA8:             return "RGBA8";
				case ImageFormat::RGBA16F:           return "RGBA16F";
				case ImageFormat::RGBA32F:           return "RGBA32F";
				case ImageFormat::SRGB:              return "SRGB";
				case ImageFormat::SRGBA:             return "SRGBA";
				case ImageFormat::RED32F:            return "RED32F";
				case ImageFormat::RED8UI:            return "RED8UI";
				case ImageFormat::RED8UN:            return "RED8UN";
				case ImageFormat::RED16UI:			 return "RED16UI";
				case ImageFormat::RED32UI:           return "RED32UI";
				case ImageFormat::B10R11G11UF:       return "B10R11G11UF";
				case ImageFormat::DEPTH24STENCIL8:   return "DEPTH24STENCIL8";
			}
			LK_CORE_ASSERT(false, "Unknown ImageFormat!");
		}

		static uint32_t BytesPerPixel(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA:    return 4;
				case ImageFormat::RGBA32F: return 16;
				case ImageFormat::None:	   return 0;
			}
			return 0;
		}

		static uint32_t CalculateMipCount(uint32_t width, uint32_t height)
		{
			return (uint32_t)std::floor(std::log2(glm::min(width, height))) + 1;
		}

		static bool IsDepthFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::DEPTH24STENCIL8:  return true;
			}
			return false;
		}

	}

}
