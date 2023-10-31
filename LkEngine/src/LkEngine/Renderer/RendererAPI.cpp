#include "LKpch.h"
#include "LkEngine/Renderer/RendererAPI.h"


#ifdef LK_RENDERER_API_VULKAN 
    #include "LkEngine/Renderer/Vulkan/VulkanRenderer.h"
#elif defined(LK_RENDERER_API_OPENGL)
    #include "LkEngine/Renderer/OpenGl/OpenGLRenderer.h"
#endif


namespace LkEngine {

    s_ptr<RendererAPI> RendererAPI::Create()
    {
    #ifdef LK_RENDERER_API_VULKAN 
        return std::make_shared<VulkanRenderer>();
    #elif defined(LK_RENDERER_API_OPENGL)
        return std::make_shared<OpenGLRenderer>();
    #else
        return nullptr;
    #endif

    }

}