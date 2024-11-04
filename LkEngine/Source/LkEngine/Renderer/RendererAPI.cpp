#include "LKpch.h"
#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGl/OpenGLRenderer.h"


namespace LkEngine {

    TObjectPtr<LRendererAPI> LRendererAPI::Create()
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                return TObjectPtr<OpenGLRenderer>::Create();
            }

            case ERendererAPI::Vulkan: 
            {
                LK_CORE_ASSERT(false, "Vulkan not supported");
                return nullptr;
            }
        }

        LK_CORE_ASSERT(false, "No Render API detected");
		return nullptr;
    }

}