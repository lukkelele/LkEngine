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
        switch (RendererAPI::Current())
        {
            case RendererAPIType::OpenGL: 
            {
                Ref<OpenGLSwapChain> swapchain = Ref<OpenGLSwapChain>::Create();
                swapchain->Create(width, height, vsync);
                return swapchain;
            }

            //case RendererAPIType::Vulkan: 
            //{
            //    Ref<VulkanSwapChain> vulkanSwapChain = Ref<VulkanSwapChain>::Create();
            //    vulkanSwapChain->Create(width, height, vsync);
            //    return vulkanSwapChain;
            //}
        }
        LK_CORE_ASSERT(false, "RendererAPI could not get determined");
    }

}