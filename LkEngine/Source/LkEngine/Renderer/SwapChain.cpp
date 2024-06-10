#include "LKpch.h"
#include "SwapChain.h"

#include "LkEngine/Core/Window.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLSwapChain.h"
//#include "LkEngine/Platform/Vulkan/VulkanContext.h"
//#include "LkEngine/Platform/Vulkan/VulkanSwapChain.h"


namespace LkEngine {

    Ref<SwapChain> SwapChain::Create(uint32_t* width, uint32_t* height, bool vsync)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                /// @FIXME: Rename local var SwapchainRef whenever LSwapChain exists
                Ref<OpenGLSwapChain> SwapchainRef = Ref<OpenGLSwapChain>::Create();
                SwapchainRef->Create(width, height, vsync);
                return SwapchainRef;
            }

            //case RendererAPIType::Vulkan: 
            //{
            //    Ref<VulkanSwapChain> vulkanSwapChain = Ref<VulkanSwapChain>::Create();
            //    vulkanSwapChain->Create(width, height, vsync);
            //    return vulkanSwapChain;
            //}

            case ERendererAPI::None: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }

}