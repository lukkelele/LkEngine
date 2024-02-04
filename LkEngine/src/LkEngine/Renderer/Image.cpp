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


}
