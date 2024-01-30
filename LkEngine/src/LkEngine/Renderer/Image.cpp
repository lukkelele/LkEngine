#include "LKpch.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Platform/OpenGL/OpenGLImage.h"
//#include "LkEngine/Platform/Vulkan/VulkanImage.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	//-------------------------------------------------------------------------------
	// Image
	//-------------------------------------------------------------------------------
	Ref<Image> Image::Create(ImageSpecification spec, Buffer buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLImage>::Create(spec, buffer);
			//case RendererAPIType::Vulkan: return Ref<VulkanImage>::Create(spec, buffer);
		}
		LK_CORE_ASSERT(false, "No RenderAPI detected");
	}

	Ref<Image> Image::Create(ImageSpecification spec, void* buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLImage>::Create(spec, buffer);
			//case RendererAPIType::Vulkan: return Ref<VulkanImage>::Create(spec, buffer);
		}
		LK_CORE_ASSERT(false, "No RenderAPI detected");
	}


	//-------------------------------------------------------------------------------
	// Image2D
	//-------------------------------------------------------------------------------
	Ref<Image2D> Image2D::Create(ImageSpecification spec, Buffer buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLImage2D>::Create(spec, buffer);
			//case RendererAPIType::Vulkan: return Ref<VulkanImage>::Create(spec, buffer);
		}
		LK_CORE_ASSERT(false, "No RenderAPI detected");
	}

	Ref<Image2D> Image2D::Create(ImageSpecification spec, void* buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLImage2D>::Create(spec, buffer);
			//case RendererAPIType::Vulkan: return Ref<VulkanImage>::Create(spec, buffer);
		}
		LK_CORE_ASSERT(false, "No RenderAPI detected");
	}


	namespace Utils {

		int64_t GetImageFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA:    return 4;
				case ImageFormat::RGBA32F: return 16;
				case ImageFormat::None:	   return 0;
			}
			return 0;
		}

		uint32_t GetMemorySize(ImageFormat format, uint32_t width, uint32_t height)
		{
			return width * height * GetFormatBPP(format);
		}

		std::string ImageFormatToString(const ImageFormat format)
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

		uint32_t BytesPerPixel(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA:    return 4;
				case ImageFormat::RGBA32F: return 16;
				case ImageFormat::None:	   return 0;
			}
			return 0;
		}

		uint32_t GetFormatBPP(ImageFormat format)
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

		uint32_t CalculateMipCount(uint32_t width, uint32_t height)
		{
			return (uint32_t)std::floor(std::log2(glm::min(width, height))) + 1;
		}

		bool IsDepthFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::DEPTH24STENCIL8:  return true;
			}
			return false;
		}

	}

}
