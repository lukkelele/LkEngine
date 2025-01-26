#include "LKpch.h"
#include "OpenGLMaterial.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    LOpenGLMaterial::LOpenGLMaterial(const TObjectPtr<LShader>& InShader, const std::string& InName)
        : Shader(InShader.As<LOpenGLShader>())
        , Name(InName)
    {
		LOBJECT_REGISTER();
		LRenderer::RegisterShader(Shader, this);
		LK_CORE_TRACE_TAG("OpenGLMaterial", "Creating '{}' with shader {}", InName, Shader->GetName());

        Texture = LRenderer::GetWhiteTexture();
		LK_CORE_ASSERT(Texture);
    }

   	LOpenGLMaterial::LOpenGLMaterial(TObjectPtr<LMaterial> InMaterial, const std::string& InName)
		: Shader(InMaterial->GetShader().As<LOpenGLShader>())
        , Name(InName)
	{
		LOBJECT_REGISTER();
		LK_CORE_ASSERT(Shader, "Invalid shader for material: {}", InName);
		LK_CORE_TRACE_TAG("OpenGLMaterial", "Creating copy of '{}''", InMaterial->GetName());

        if (Name.empty())
        {
			Name = InMaterial->GetName();
			LK_CORE_VERIFY(!Name.empty());
        }

		LRenderer::RegisterShader(Shader, this);

        if (TObjectPtr<LTexture2D> MaterialTexture = InMaterial->GetTexture(); MaterialTexture != nullptr)
        {
            Texture = MaterialTexture;
        }
        else
        {
            Texture = LRenderer::GetWhiteTexture();
        }
		LK_CORE_ASSERT(Texture, "Material {} texture is corrupt", InName);

		UniformStorageBuffer = InMaterial->GetUniformStorageBuffer();
	}

    void LOpenGLMaterial::Invalidate()
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::OnShaderReloaded()
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::SetTexture(TObjectPtr<LTexture> InTexture)
    {
		LK_CORE_VERIFY(InTexture);
        Texture = InTexture.As<LTexture2D>();
        LK_CORE_TRACE_TAG("OpenGLMaterial", "Set texture '{}' to material: {}", Texture->GetName(), Name);
	}

    void LOpenGLMaterial::SetTexture(TObjectPtr<LTexture2D> InTexture)
    {
		LK_CORE_VERIFY(InTexture);
        Texture = InTexture;
        LK_CORE_TRACE_TAG("OpenGLMaterial", "Set texture '{}' to material: {}", Texture->GetName(), Name);
    }

    TObjectPtr<LTexture2D> LOpenGLMaterial::GetTexture(const std::string& Name)
    {
		LK_CORE_VERIFY(Texture);
        return Texture;
    }

    const TObjectPtr<LTexture2D>& LOpenGLMaterial::GetTexture(const std::string& Name) const
    {
		LK_CORE_VERIFY(Texture);
        return Texture;
    }

    TObjectPtr<LTexture2D> LOpenGLMaterial::TryGetTexture(const std::string& Name)
    {
        return Texture;
    }

    void LOpenGLMaterial::Set(const std::string& Name, int value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, bool value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, float value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, uint32_t value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::vec2& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::vec3& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::vec4& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::ivec2& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::ivec3& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::ivec4& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::mat3& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const glm::mat4& value)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const TObjectPtr<LTexture2D>& InTexture)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const TObjectPtr<LTexture2D>& InTexture, const uint32_t ArrayIdx)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LOpenGLMaterial::Set(const std::string& Name, const TObjectPtr<LImage2D>& Image)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    float& LOpenGLMaterial::GetFloat(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static float f = 0;
        return f;
    }

    int32_t& LOpenGLMaterial::GetInt(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static int32_t Int = 0;
        return Int;
    }

    uint32_t& LOpenGLMaterial::GetUInt(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static uint32_t Int = 0;
        return Int;
    }

    bool& LOpenGLMaterial::GetBool(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static bool Bool = true;
        return Bool;
    }

    glm::vec2& LOpenGLMaterial::GetVector2(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static glm::vec2 Vec2(0.0f);
        return Vec2;
    }

    glm::vec3& LOpenGLMaterial::GetVector3(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static glm::vec3 Vec3(0.0f);
        return Vec3;
    }

    glm::vec4& LOpenGLMaterial::GetVector4(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static glm::vec4 Vec4(0.0f);
        return Vec4;
    }

    glm::mat3& LOpenGLMaterial::GetMatrix3(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static glm::mat3 Mat(1.0f);
        return Mat;
    }

    glm::mat4& LOpenGLMaterial::GetMatrix4(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        static glm::mat4 Mat(1.0f);
        return Mat;
    }

    TObjectPtr<LTextureCube> LOpenGLMaterial::GetTextureCube(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        return nullptr;
    }

    TObjectPtr<LTextureCube> LOpenGLMaterial::TryGetTextureCube(const std::string& Name)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
        return nullptr;
    }

}
