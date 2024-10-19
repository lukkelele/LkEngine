#include "LKpch.h"
#include "TextureArray.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLTextureArray.h"


namespace LkEngine {

    TObjectPtr<TextureArray> TextureArray::Create(const FTextureArraySpecification& Specification)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                return TObjectPtr<OpenGLTextureArray>::Create(Specification);
            }

            case ERendererAPI::Vulkan:
            default: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }
}