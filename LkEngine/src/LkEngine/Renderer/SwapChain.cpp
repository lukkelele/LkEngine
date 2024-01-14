#include "LKpch.h"
#include "SwapChain.h"

#include "LkEngine/Platform/Vulkan/VulkanContext.h"

#include "LkEngine/Platform/Vulkan/VulkanSwapChain.h"
#include "LkEngine/Platform/OpenGL/OpenGLSwapChain.h"

#include "LkEngine/Core/Window.h"

#include "RendererAPI.h"


namespace LkEngine {

    Ref<SwapChain> SwapChain::Create(uint32_t* width, uint32_t* height, bool vsync)
    {
        switch (RendererAPI::Current())
        {
            //case RendererAPIType::Vulkan: return Ref<VulkanSwapChain>::Create(width, height, vsync);
            //case RendererAPIType::OpenGL: return Ref<OpenGLSwapChain>::Create(width, height, vsync);
            //case RendererAPIType::Vulkan: return Ref<VulkanSwapChain>::Create();
            //case RendererAPIType::OpenGL: return Ref<OpenGLSwapChain>::Create();
            case RendererAPIType::Vulkan: 
            {
                Ref<VulkanSwapChain> vulkanSwapChain = Ref<VulkanSwapChain>::Create();
                vulkanSwapChain->Create(width, height, vsync);
                return Ref<VulkanSwapChain>();
                //Ref<VulkanSwapChain>()::Create(width, height, vsync);
		        //Ref<VulkanContext> context = Ref<GraphicsContext>().As<VulkanContext>();
		        //Ref<VulkanContext> context = GraphicsContext::Get().As<VulkanContext>();
                //LK_CORE_ASSERT(context, "VulkanContext is nullptr");
		        //vulkanSwapChain->Init(context->GetInstance(), context->GetDevice());
		        //vulkanSwapChain->InitSurface(Ref<Window>()->GetGlfwWindow());
                //vulkanSwapChain->Create(width, height, vsync);
                //return vulkanSwapChain;
            }

            case RendererAPIType::OpenGL: 
            {
                Ref<OpenGLSwapChain>()->Create(width, height, vsync);
                return Ref<OpenGLSwapChain>();
            }
        }
        LK_CORE_ASSERT(false, "RendererAPI could not get determined");
    }

}