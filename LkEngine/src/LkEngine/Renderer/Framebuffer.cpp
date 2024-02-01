#include "LKpch.h"
#include "Framebuffer.h"

#include "LkEngine/Platform/OpenGL/OpenGLFramebuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanFramebuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:	return Ref<OpenGLFramebuffer>::Create(spec);
			//case RendererAPIType::Vulkan:	return Ref<VulkanFramebuffer>::Create(spec);
			case RendererAPIType::None:		return nullptr;
		}
		LK_CORE_ASSERT(false, "No framebuffer could be created, no renderer API could be determined");
	}

	void Framebuffer::TargetSwapChain()
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:	
				OpenGLFramebuffer::TargetSwapChain();
				break;
		}
	}


}