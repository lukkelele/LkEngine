#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <stb_image/stb_image.h>

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Buffer.h"


namespace LkEngine {

	enum class ImageFormat
	{
	    None = 0,
		RGB,
		RGB8,
	    RGBA,
	    RGBA8,
		RGBA16F,
	    RGBA32F,
		DEPTH32F,
		DEPTH24STENCIL8,
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

	struct ImageSpecification
	{
		ImageFormat Format = ImageFormat::RGBA;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Mips = 1;
		uint32_t Layers = 1;
		bool Deinterleaved = false;

		std::string DebugName;
	};
	
	class Image
	{
	public:
		virtual ~Image() = default;
	
		//static s_ptr<Image> Create(std::string_view filepath);
		static s_ptr<Image> Create(ImageSpecification spec, Buffer buffer);
		static s_ptr<Image> Create(ImageSpecification spec, const void* data = nullptr);
		static uint32_t BytesPerPixel(ImageFormat format);
		static uint32_t GetFormatBPP(ImageFormat format);
		static uint32_t CalculateMipCount(uint32_t width, uint32_t height);
		static uint32_t GetMemorySize(ImageFormat format, uint32_t width, uint32_t height);
		static bool IsDepthFormat(ImageFormat format);
	
		virtual void Invalidate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void SetData(const void* data) = 0;

		virtual Buffer GetBuffer() const = 0;
		virtual Buffer& GetBuffer() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual int64_t GetImageFormat(ImageFormat fmt) = 0;
		virtual RendererID GetRendererID() const = 0;
		virtual const ImageSpecification GetImageSpecification() const = 0;
	
	protected:
		virtual void Release() = 0;
		virtual void AllocateMemory(uint64_t size) = 0;

	};
}
