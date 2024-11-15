#include "LKpch.h"
#include "OpenGLMaterial.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    LOpenGLMaterial::LOpenGLMaterial(const TObjectPtr<LShader>& InShader, const std::string& InName)
        : Shader(InShader.As<LOpenGLShader>())
        , Name(InName)
    {
		LCLASS_REGISTER();
		LRenderer::RegisterShaderDependency(Shader, this);

        Texture = LRenderer::GetWhiteTexture();
    }

   	LOpenGLMaterial::LOpenGLMaterial(TObjectPtr<LMaterial> InMaterial, const std::string& InName)
		: Shader(InMaterial->GetShader().As<LOpenGLShader>())
        , Name(InName)
	{
		LCLASS_REGISTER();
		LRenderer::RegisterShaderDependency(Shader, this);

        if (Name.empty())
        {
			Name = InMaterial->GetName();
		    LK_ASSERT(!Name.empty(), "Material Name is empty for class '{}'", InMaterial->GetClass()->GetName());
        }
        
        /// FIX THIS
        if (TObjectPtr<LTexture2D> MaterialTexture = InMaterial->GetTexture(""))
        {
            Texture = MaterialTexture;
        }
        else
        {
            Texture = LRenderer::GetWhiteTexture();
        }

		UniformStorageBuffer = InMaterial->GetUniformStorageBuffer();

        LK_CORE_WARN_TAG("OpenGLMaterial", "Created new material based on another material called \"{}\"", InMaterial->GetName());
	}

    void LOpenGLMaterial::Invalidate()
    {
    }

    void LOpenGLMaterial::OnShaderReloaded()
    {
    }

    void LOpenGLMaterial::SetTexture(TObjectPtr<LTexture> InTexture)
    {
        LK_CORE_ASSERT(InTexture, "Passed texture is nullptr");
        Texture = InTexture.As<LTexture2D>();
        LK_CORE_WARN_TAG("OpenGLMaterial", "Set texture {} to material \"{}\"", Texture->GetName(), Name);
    }

    TObjectPtr<LTexture2D> LOpenGLMaterial::GetTexture(const std::string& Name)
    {
        //LK_CORE_ASSERT(Texture, "LOpenGLMaterial::m_Texture ({}) is nullptr!", Name);
        return Texture;
    }

    const TObjectPtr<LTexture2D>& LOpenGLMaterial::GetTexture(const std::string& Name) const
    {
        return Texture;
    }

    TObjectPtr<LTexture2D> LOpenGLMaterial::TryGetTexture(const std::string& Name)
    {
        return Texture;
    }

    void LOpenGLMaterial::Set(const std::string& Name, int value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, bool value)
    {
        LK_CORE_INFO_TAG("OpenGLMaterial", "Setting (bool) value {}", value);
    }

    void LOpenGLMaterial::Set(const std::string& Name, float value)
    {
        LK_CORE_INFO_TAG("OpenGLMaterial", "Setting (float) value {}", value);
    }

    void LOpenGLMaterial::Set(const std::string& Name, uint32_t value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::vec2& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::vec3& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::vec4& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::ivec2& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::ivec3& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::ivec4& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::mat3& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::mat4& value)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const TObjectPtr<LTexture2D>& texture)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const TObjectPtr<LTexture2D>& texture, uint32_t arrayIndex)
    {
    }

    void LOpenGLMaterial::Set(const std::string& Name, const TObjectPtr<LImage2D>& image)
    {
    }

    float& LOpenGLMaterial::GetFloat(const std::string& Name)
    {
        float f = 0;
        return f;
    }

    int32_t& LOpenGLMaterial::GetInt(const std::string& Name)
    {
        LK_CORE_ASSERT(false);
        int32_t i = 0;
        return i;
    }

    uint32_t& LOpenGLMaterial::GetUInt(const std::string& Name)
    {
        // TODO: insert return statement here
        LK_CORE_ASSERT(false);
        uint32_t i = 0;
        return i;
    }

    bool& LOpenGLMaterial::GetBool(const std::string& Name)
    {
        // TODO: insert return statement here
        LK_CORE_ASSERT(false);
        bool boolean = true;
        return boolean;
    }

    glm::vec2& LOpenGLMaterial::GetVector2(const std::string& Name)
    {
        LK_CORE_ASSERT(false);
        glm::vec2 vec2(0.0f);
        return vec2;
    }

    glm::vec3& LOpenGLMaterial::GetVector3(const std::string& Name)
    {
        // TODO: insert return statement here
        glm::vec3 vec3(0.0f);
        return vec3;
    }

    glm::vec4& LOpenGLMaterial::GetVector4(const std::string& Name)
    {
        //Shader->GetUniforms
        glm::vec4 vec4(0.0f);
        return vec4;
    }

    glm::mat3& LOpenGLMaterial::GetMatrix3(const std::string& Name)
    {
        glm::mat3 mat(1.0f);
        return mat;
    }

    glm::mat4& LOpenGLMaterial::GetMatrix4(const std::string& Name)
    {
        glm::mat4 mat(1.0f);
        return mat;
    }

    TObjectPtr<LTextureCube> LOpenGLMaterial::GetTextureCube(const std::string& Name)
    {
        return nullptr;
    }

    TObjectPtr<LTextureCube> LOpenGLMaterial::TryGetTextureCube(const std::string& Name)
    {
        return nullptr;
    }

}