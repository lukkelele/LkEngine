#include "LKpch.h"
#include "ArrayTexture.h"

#include "RendererAPI.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLArrayTexture.h"


namespace LkEngine {

    TObjectPtr<LArrayTexture> LArrayTexture::Create(const FArrayTextureSpecification& Specification)
    {
        switch (LRendererAPI::Get())
        {
            case ERendererAPI::OpenGL: 
            {
                return TObjectPtr<LOpenGLArrayTexture>::Create(Specification);
            }

            case ERendererAPI::Vulkan:
            default: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }

}
