#include "LKpch.h"
#include "LkEngine/Renderer/Material.h"

#include "LkEngine/Platform/OpenGL/OpenGLMaterial.h"

#include "RendererAPI.h"


namespace LkEngine {

    TObjectPtr<LMaterial> LMaterial::Create(const TObjectPtr<LShader>& Shader, const std::string& name)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLMaterial>::Create(Shader, name);
            default: break;
        }

        LK_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

    TObjectPtr<LMaterial> LMaterial::Copy(const TObjectPtr<LMaterial>& other, const std::string& name)
    {
        switch (LRendererAPI::Current())
        {
            case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLMaterial>::Create(other, name);

            default: break;
        }

        LK_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

}