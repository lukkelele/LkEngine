#include "LKpch.h"
#include "LkEngine/Renderer/Image.h"

#ifdef LK_RENDERER_API_VULKAN
	#include "LkEngine/Renderer/Vulkan/VulkanImage.h"
#elif defined (LK_RENDERER_API_OPENGL)
	#include "LkEngine/Renderer/OpenGL/OpenGLImage.h"
#endif

namespace LkEngine {

	s_ptr<Image> Image::Create(ImageSpecification spec, Buffer buffer)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanImage>(spec, buffer);
	#elif defined (LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLImage>(spec, buffer);
	#else
		LK_ASSERT(false);
		return nullptr;
	#endif
	}

	s_ptr<Image> Image::Create(ImageSpecification spec, const void* data)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanImage>(spec, data);
	#elif defined (LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLImage>(spec, data);
	#else
		LK_ASSERT(false);
		return nullptr;
	#endif
	}
	
	uint32_t Image::BytesPerPixel(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA:    return 4;
			case ImageFormat::RGBA32F: return 16;
			case ImageFormat::None:	   return 0;
		}
		return 0;
	}
	
	int64_t Image::GetImageFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA:    return 4;
			case ImageFormat::RGBA32F: return 16;
			case ImageFormat::None:	   return 0;
		}
		return 0;
	}

	uint32_t Image::GetFormatBPP(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB:
			case ImageFormat::RGBA8:   return 4;
			case ImageFormat::RGBA:    return 4;
			case ImageFormat::RGBA16F: return 2 * 4;
			case ImageFormat::RGBA32F: return 4 * 4;
		}
		LK_ASSERT(false);
		return 0;
	}

	uint32_t Image::CalculateMipCount(uint32_t width, uint32_t height)
	{
		return (uint32_t)std::floor(std::log2(glm::min(width, height))) + 1;
	}

	uint32_t Image::GetMemorySize(ImageFormat format, uint32_t width, uint32_t height)
	{
		return width * height * GetFormatBPP(format);
	}

	bool Image::IsDepthFormat(ImageFormat format)
	{
		if (format == ImageFormat::DEPTH24STENCIL8 || format == ImageFormat::DEPTH32F)
			return true;
		return false;
	}


}
