#include "LKpch.h"
#include "SwapChain.h"

#include "LkEngine/Core/Window.h"

#include "RendererAPI.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLSwapChain.h"


namespace LkEngine {

    TObjectPtr<LSwapChain> LSwapChain::Create(uint32_t* width, uint32_t* height, bool vsync)
    {
        switch (LRendererAPI::Get())
        {
            case ERendererAPI::OpenGL: 
            {
                TObjectPtr<OpenGLSwapChain> SwapChain = TObjectPtr<OpenGLSwapChain>::Create();
                SwapChain->Create(width, height, vsync);
                return SwapChain;
            }

            case ERendererAPI::None: break;
            default: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }

}