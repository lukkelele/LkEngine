#include "LKpch.h"
#include "RendererAPI.h"

#include "LkEngine/Platform/Vulkan/VulkanRenderer.h"
#include "LkEngine/Platform/OpenGl/OpenGLRenderer.h"


namespace LkEngine {

    Ref<RendererAPI> RendererAPI::Create()
    {
        switch (RendererAPI::Current())
        {
            case RendererAPIType::Vulkan: return Ref<VulkanRenderer>::Create();
            case RendererAPIType::OpenGL: return Ref<OpenGLRenderer>::Create();
        }
        LK_CORE_ASSERT(false, "No Render API detected");
    }

}