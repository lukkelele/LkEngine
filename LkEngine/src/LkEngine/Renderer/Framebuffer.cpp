#include "LKpch.h"
#include "Framebuffer.h"

#ifdef LK_RENDERER_API_OPENGL
#include "LkEngine/Platform/OpenGL/OpenGLFramebuffer.h"
#else
#include "LkEngine/Platform/Vulkan/VulkanFramebuffer.h"
#endif

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	s_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& framebufferSpecification)
    {
        switch (RendererAPI::Current())
        {
            case RendererAPIType::OpenGL: return std::make_shared<OpenGLFramebuffer>(framebufferSpecification);
            case RendererAPIType::Vulkan: return nullptr;
            case RendererAPIType::None:   return nullptr;
        }
    }

}