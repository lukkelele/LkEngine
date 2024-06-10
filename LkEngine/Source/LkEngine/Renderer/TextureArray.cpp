#include "LKpch.h"
#include "TextureArray.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLTextureArray.h"
//#include "LkEngine/Platform/Vulkan/VulkanTextureArray.h"

namespace LkEngine {

    Ref<TextureArray> TextureArray::Create(const TextureArraySpecification& specification)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: 
            {
                return Ref<OpenGLTextureArray>::Create(specification);
            }

            /// @FIXME
            //case RendererAPIType::Vulkan: return Ref<VulkanTextureArray>::Create(specification);

            case ERendererAPI::None: break;
        }

        LK_CORE_ASSERT(false, "Invalid Render API");
        return nullptr;
    }
}