#include "LKpch.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Platform/Vulkan/VulkanImage.h"
#include "LkEngine/Platform/OpenGL/OpenGLImage.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	Ref<Image> Image::Create(ImageSpecification spec, Buffer buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanImage>::Create(spec, buffer);
			case RendererAPIType::OpenGL: return Ref<OpenGLImage>::Create(spec, buffer);
			case RendererAPIType::None:   return nullptr;
		}
	}

	Ref<Image> Image::Create(ImageSpecification spec, void* buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanImage>::Create(spec, buffer);
			case RendererAPIType::OpenGL: return Ref<OpenGLImage>::Create(spec, buffer);
			case RendererAPIType::None:   return nullptr;
		}
		LK_CORE_ASSERT(false, "No RenderAPI detected");
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
		LK_CORE_ASSERT(false, "GetFormatBPP failed, format not recognized!");
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

	namespace Utils {

		std::string ImageFormatToString(const ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:               return "RGB";
				case ImageFormat::RGBA:              return "RGBA";
				case ImageFormat::RGBA8:             return "RGBA8";
				case ImageFormat::RGBA16F:           return "RGBA16F";
				case ImageFormat::RGBA32F:           return "RGBA32F";
				case ImageFormat::SRGB:              return "SRGB";
				case ImageFormat::SRGBA:             return "SRGBA";
				case ImageFormat::DEPTH24STENCIL8:   return "DEPTH24STENCIL8";
				case ImageFormat::RED32F:            return "RED32F";
			}
			return "";
		}
	}

}
