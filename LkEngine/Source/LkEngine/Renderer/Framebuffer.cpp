#include "LKpch.h"
#include "Framebuffer.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLFramebuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LFramebuffer> LFramebuffer::Create(const FFramebufferSpecification& InSpecification)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL:	
			{
				return TObjectPtr<LOpenGLFramebuffer>::Create(InSpecification);
			}

			case ERendererAPI::Vulkan:	
			default: break;
		}

		LK_CORE_ASSERT(false, "No framebuffer could be created, no renderer API could be determined");
		return nullptr;
	}

	void LFramebuffer::TargetSwapChain()
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL:	
			{
				LOpenGLFramebuffer::TargetSwapChain();
				break;
			}

			case ERendererAPI::Vulkan:	
			default: break;
		}
	}


}