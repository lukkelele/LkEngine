#pragma once

#include "LkEngine/Renderer/Material.h"
#include "LkEngine/Renderer/Shader.h"

#include "OpenGLShader.h"


namespace LkEngine {

    class OpenGLMaterial : public Material
    {
    public:
    	OpenGLMaterial(const Ref<Shader>& shader, const std::string& name = "");
		OpenGLMaterial(Ref<Material> material, const std::string& name = "");
		~OpenGLMaterial();

		void Invalidate() override;
		void OnShaderReloaded() override;

		void Set(const std::string& name, int value) override;
		void Set(const std::string& name, bool value) override;
		void Set(const std::string& name, float value) override;
		void Set(const std::string& name, uint32_t value) override;
		void Set(const std::string& name, const glm::vec2& value) override;
		void Set(const std::string& name, const glm::vec3& value) override;
		void Set(const std::string& name, const glm::vec4& value) override;
		void Set(const std::string& name, const glm::ivec2& value) override;
		void Set(const std::string& name, const glm::ivec3& value) override;
		void Set(const std::string& name, const glm::ivec4& value) override;

		void Set(const std::string& name, const glm::mat3& value) override;
		void Set(const std::string& name, const glm::mat4& value) override;

		void Set(const std::string& name, const Ref<Texture2D>& texture) override;
		void Set(const std::string& name, const Ref<Texture2D>& texture, uint32_t arrayIndex) override;
		void Set(const std::string& name, const Ref<Image2D>& image) override;
		// void Set(const std::string& name, const Ref<TextureCube>& texture);
		// void Set(const std::string& name, const Ref<ImageView>& image);

		float& GetFloat(const std::string& name) override;
		int32_t& GetInt(const std::string& name) override;
		uint32_t& GetUInt(const std::string& name) override;
		bool& GetBool(const std::string& name) override;
		glm::vec2& GetVector2(const std::string& name) override;
		glm::vec3& GetVector3(const std::string& name) override;
		glm::vec4& GetVector4(const std::string& name) override;
		glm::mat3& GetMatrix3(const std::string& name) override;
		glm::mat4& GetMatrix4(const std::string& name) override;

		void SetTexture(Ref<Texture> texture) override;
		Ref<Texture2D> GetTexture(const std::string& name) override;
		Ref<Texture2D> TryGetTexture(const std::string& name) override;
		Ref<TextureCube> GetTextureCube(const std::string& name) override;
		Ref<TextureCube> TryGetTextureCube(const std::string& name) override;

		Ref<Shader> GetShader() override;
		const std::string& GetName() const override;

    private:
		Ref<OpenGLShader> m_Shader;

		std::string m_Name;
        Ref<Texture2D> m_Texture;

        MaterialSpecification m_Properties;

		uint32_t m_MaterialFlags = 0;
		Buffer m_UniformStorageBuffer;
    };


}