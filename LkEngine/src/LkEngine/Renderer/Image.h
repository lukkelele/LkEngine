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
	    RGBA,
		RGBA16F,
	    RGBA32F,
		DEPTH32F,
		DEPTH24STENCIL8,
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
	
		virtual void SetData(const void* data) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual Buffer GetBuffer() const = 0;
		virtual Buffer& GetBuffer() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual int64_t GetImageFormat(ImageFormat fmt) = 0;
		virtual RendererID GetRendererID() const = 0;
		virtual const ImageSpecification GetImageSpecification() const = 0;
	
	protected:
		virtual void AllocateMemory(uint64_t size) = 0;
		virtual void ReleaseMemory() = 0;

	};
}
