#include "LKpch.h"
#include "OpenGLMaterial.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& name)
        : m_Shader(shader.As<OpenGLShader>())
        , m_Name(name)
    {
		Renderer::RegisterShaderDependency(m_Shader, this);
        m_Texture = Renderer::GetWhiteTexture();
    }

   	OpenGLMaterial::OpenGLMaterial(Ref<Material> material, const std::string& name)
		: m_Shader(material->GetShader().As<OpenGLShader>())
        , m_Name(name)
	{
		Renderer::RegisterShaderDependency(m_Shader, this);
		if (name.empty())
			m_Name = material->GetName();
        
        if (Ref<Texture2D> texture = material->GetTexture(""))
        {
            m_Texture = texture;
        }
        else
        {
            m_Texture = Renderer::GetWhiteTexture();
        }

		Ref<OpenGLMaterial> glMaterial = material.As<OpenGLMaterial>();
		m_UniformStorageBuffer = Buffer::Copy(glMaterial->m_UniformStorageBuffer.Data, glMaterial->m_UniformStorageBuffer.Size);
        //m_Texture = glMaterial->GetTexture("");

        LK_CORE_WARN_TAG("OpenGLMaterial", "Created new material based on another material called \"{}\"", material->GetName());
	}

    OpenGLMaterial::~OpenGLMaterial()
    {
    }

    void OpenGLMaterial::Invalidate()
    {
    }

    void OpenGLMaterial::OnShaderReloaded()
    {
    }

    void OpenGLMaterial::SetTexture(Ref<Texture> texture)
    {
        LK_CORE_ASSERT(texture);
        m_Texture = texture.As<Texture2D>();
        LK_CORE_ASSERT(m_Texture, "OpenGLMaterial::m_Texture ({}) is nullptr even after setting it with {}", m_Name, texture->GetName());
        LK_CORE_WARN_TAG("OpenGLMaterial", "Set texture {} to material \"{}\"", texture->GetName(), m_Name);
    }

    Ref<Texture2D> OpenGLMaterial::GetTexture(const std::string& name)
    {
        //LK_CORE_ASSERT(m_Texture, "OpenGLMaterial::m_Texture ({}) is nullptr!", m_Name);
        return m_Texture;
    }

    Ref<Texture2D> OpenGLMaterial::TryGetTexture(const std::string& name)
    {
        return m_Texture;
    }

    void OpenGLMaterial::Set(const std::string& name, int value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, bool value)
    {
        LK_CORE_INFO_TAG("OpenGLMaterial", "Setting (bool) value {}", value);
    }

    void OpenGLMaterial::Set(const std::string& name, float value)
    {
        LK_CORE_INFO_TAG("OpenGLMaterial", "Setting (float) value {}", value);
    }

    void OpenGLMaterial::Set(const std::string& name, uint32_t value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::vec2& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::vec3& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::vec4& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::ivec2& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::ivec3& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::ivec4& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::mat3& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::mat4& value)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const Ref<Texture2D>& texture)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const Ref<Texture2D>& texture, uint32_t arrayIndex)
    {
    }

    void OpenGLMaterial::Set(const std::string& name, const Ref<Image2D>& image)
    {
    }

    float& OpenGLMaterial::GetFloat(const std::string& name)
    {
        float f = 0;
        return f;
    }

    int32_t& OpenGLMaterial::GetInt(const std::string& name)
    {
        LK_CORE_ASSERT(false);
        int32_t i = 0;
        return i;
    }

    uint32_t& OpenGLMaterial::GetUInt(const std::string& name)
    {
        // TODO: insert return statement here
        LK_CORE_ASSERT(false);
        uint32_t i = 0;
        return i;
    }

    bool& OpenGLMaterial::GetBool(const std::string& name)
    {
        // TODO: insert return statement here
        LK_CORE_ASSERT(false);
        bool boolean = true;
        return boolean;
    }

    glm::vec2& OpenGLMaterial::GetVector2(const std::string& name)
    {
        LK_CORE_ASSERT(false);
        glm::vec2 vec2(0.0f);
        return vec2;
    }

    glm::vec3& OpenGLMaterial::GetVector3(const std::string& name)
    {
        // TODO: insert return statement here
        glm::vec3 vec3(0.0f);
        return vec3;
    }

    glm::vec4& OpenGLMaterial::GetVector4(const std::string& name)
    {
        //m_Shader->GetUniforms
        glm::vec4 vec4(0.0f);
        return vec4;
    }

    glm::mat3& OpenGLMaterial::GetMatrix3(const std::string& name)
    {
        glm::mat3 mat(1.0f);
        return mat;
    }

    glm::mat4& OpenGLMaterial::GetMatrix4(const std::string& name)
    {
        glm::mat4 mat(1.0f);
        return mat;
    }

    Ref<Shader> OpenGLMaterial::GetShader()
    {
        return m_Shader;
    }

    const std::string& OpenGLMaterial::GetName() const
    {
        return m_Name;
    }

    Ref<TextureCube> OpenGLMaterial::GetTextureCube(const std::string& name)
    {
        return nullptr;
    }

    Ref<TextureCube> OpenGLMaterial::TryGetTextureCube(const std::string& name)
    {
        return nullptr;
    }

}