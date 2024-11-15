#include "LKpch.h"
#include "TextureArray.h"

#include "RendererAPI.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLTextureArray.h"


namespace LkEngine {

    TObjectPtr<LTextureArray> LTextureArray::Create(const FTextureArraySpecification& Specification)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                return TObjectPtr<LOpenGLTextureArray>::Create(Specification);
            }

            case ERendererAPI::Vulkan:
            default: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }
}