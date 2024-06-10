#include "LKpch.h"
#include "LkEngine/Renderer/Material.h"

#include "LkEngine/Platform/OpenGL/OpenGLMaterial.h"

#include "RendererAPI.h"


namespace LkEngine {

    Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: return Ref<OpenGLMaterial>::Create(shader, name);
            /// @FIXME
            //case RendererAPIType::Vulkan:: break;
        }

        LK_CORE_ASSERT(false, "Unknown renderer API");
    }

    Ref<Material> Material::Copy(const Ref<Material>& other, const std::string& name)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: return Ref<OpenGLMaterial>::Create(other, name);
            /// @FIXME
            //case RendererAPIType::Vulkan:: break;
        }
        LK_CORE_ASSERT(false, "Unknown renderer API");
    }

}