#include "LKpch.h"
#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGl/OpenGLRenderer.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderer.h"


namespace LkEngine {

    Ref<LRendererAPI> LRendererAPI::Create()
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                return Ref<OpenGLRenderer>::Create();
            }

            case ERendererAPI::Vulkan: 
            {
                LK_CORE_ASSERT(false, "Vulkan not supported");
                return nullptr;
                //return Ref<VulkanRenderer>::Create();
            }
        }

        LK_CORE_ASSERT(false, "No Render API detected");
    }

}