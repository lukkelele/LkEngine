#include "LKpch.h"
#include "LkEngine/Renderer/Material.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLMaterial.h"

#include "RendererAPI.h"


namespace LkEngine {

    TObjectPtr<LMaterial> LMaterial::Create(const TObjectPtr<LShader>& Shader, const std::string& Name)
    {
        switch (LRendererAPI::Get())
        {
            case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLMaterial>::Create(Shader, Name);

            default: break;
        }

        LK_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

    TObjectPtr<LMaterial> LMaterial::Copy(const TObjectPtr<LMaterial>& Other, const std::string& Name)
    {
        switch (LRendererAPI::Get())
        {
            case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLMaterial>::Create(Other, Name);

            default: break;
        }

        LK_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

}