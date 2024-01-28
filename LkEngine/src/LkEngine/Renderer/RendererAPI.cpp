#include "LKpch.h"
#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGl/OpenGLRenderer.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderer.h"


namespace LkEngine {

    Ref<RendererAPI> RendererAPI::Create()
    {
        switch (RendererAPI::Current())
        {
            case RendererAPIType::OpenGL: return Ref<OpenGLRenderer>::Create();
            //case RendererAPIType::Vulkan: return Ref<VulkanRenderer>::Create();
        }
        LK_CORE_ASSERT(false, "No Render API detected");
    }

}