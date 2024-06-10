#include "LKpch.h"
#include "Framebuffer.h"

#include "LkEngine/Platform/OpenGL/OpenGLFramebuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanFramebuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:	
			{
				return Ref<OpenGLFramebuffer>::Create(spec);
			}
			case ERendererAPI::Vulkan:	
			{
				LK_CORE_ASSERT(false, "Vulkan not supported");
				return nullptr;
				//return Ref<VulkanFramebuffer>::Create(spec);
			}

			case ERendererAPI::None:		return nullptr;
			{
				break;
			}
		}

		LK_CORE_ASSERT(false, "No framebuffer could be created, no renderer API could be determined");
	}

	/// @FIXME: REMOVE THIS 
	void Framebuffer::TargetSwapChain()
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:	
			{
				OpenGLFramebuffer::TargetSwapChain();
				break;
			}
		}
	}


}