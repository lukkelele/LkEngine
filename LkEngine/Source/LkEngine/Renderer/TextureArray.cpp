#include "LKpch.h"
#include "TextureArray.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLTextureArray.h"


namespace LkEngine {

    Ref<TextureArray> TextureArray::Create(const TextureArraySpecification& specification)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                return Ref<OpenGLTextureArray>::Create(specification);
            }

            case ERendererAPI::None: break;
            default: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }
}