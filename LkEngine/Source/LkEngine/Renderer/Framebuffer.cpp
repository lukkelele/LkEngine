#include "LKpch.h"
#include "Framebuffer.h"

#include "LkEngine/Platform/OpenGL/OpenGLFramebuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanFramebuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LFramebuffer> LFramebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:	
			{
				return TObjectPtr<LOpenGLFramebuffer>::Create(spec);
			}
			case ERendererAPI::Vulkan:	
			{
				LK_CORE_ASSERT(false, "Vulkan not supported");
				return nullptr;
			}

			default: break;
		}

		LK_CORE_ASSERT(false, "No framebuffer could be created, no renderer API could be determined");
		return nullptr;
	}

	/// @FIXME: REMOVE THIS 
	void LFramebuffer::TargetSwapChain()
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:	
			{
				LOpenGLFramebuffer::TargetSwapChain();
				break;
			}
		}
	}


}