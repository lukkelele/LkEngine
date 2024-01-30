#pragma once

#include "LkEngine/Core/Buffer.h"
#include "LkEngine/Renderer/BlendingSpecification.h"

#include "LkEngine/Core/Event/AssetEvent.h"

#include "Color.h"


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
		HostRead
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

		virtual const ImageSpecification GetSpecification() const = 0;
		virtual ImageSpecification& GetSpecification() = 0;
	
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

		int64_t GetImageFormat(ImageFormat format);
		uint32_t GetMemorySize(ImageFormat format, uint32_t width, uint32_t height);
		std::string ImageFormatToString(const ImageFormat format);
		uint32_t GetFormatBPP(ImageFormat format);
		uint32_t BytesPerPixel(ImageFormat format);
		uint32_t CalculateMipCount(uint32_t width, uint32_t height);
		bool IsDepthFormat(ImageFormat format);

	}

}
