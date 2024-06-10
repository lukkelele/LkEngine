#include "LKpch.h"
#include "TextureArray.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLTextureArray.h"
//#include "LkEngine/Platform/Vulkan/VulkanTextureArray.h"

namespace LkEngine {

    Ref<TextureArray> TextureArray::Create(const TextureArraySpecification& specification)
    {
        switch (RendererAPI::Current())
        {
            case RendererAPIType::OpenGL: return Ref<OpenGLTextureArray>::Create(specification);
            //case RendererAPIType::Vulkan: return Ref<VulkanTextureArray>::Create(specification);
        }
        LK_CORE_ASSERT(false, "Could not determine RendererAPI");
    }
}