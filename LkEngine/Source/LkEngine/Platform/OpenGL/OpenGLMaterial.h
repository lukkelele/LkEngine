#pragma once

#include "LkEngine/Renderer/Material.h"
#include "LkEngine/Renderer/Shader.h"

#include "OpenGLShader.h"


namespace LkEngine {

    class LOpenGLMaterial : public LMaterial
    {
    public:
    	LOpenGLMaterial(const TObjectPtr<LShader>& Shader, const std::string& name = "");
		LOpenGLMaterial(TObjectPtr<LMaterial> Material, const std::string& name = "");
		~LOpenGLMaterial() = default;

		void Invalidate() override;
		void OnShaderReloaded() override;

		virtual void Set(const std::string& name, int value) override;
		virtual void Set(const std::string& name, bool value) override;
		virtual void Set(const std::string& name, float value) override;
		virtual void Set(const std::string& name, uint32_t value) override;
		virtual void Set(const std::string& name, const glm::vec2& value) override;
		virtual void Set(const std::string& name, const glm::vec3& value) override;
		virtual void Set(const std::string& name, const glm::vec4& value) override;
		virtual void Set(const std::string& name, const glm::ivec2& value) override;
		virtual void Set(const std::string& name, const glm::ivec3& value) override;
		virtual void Set(const std::string& name, const glm::ivec4& value) override;

		virtual void Set(const std::string& name, const glm::mat3& value) override;
		virtual void Set(const std::string& name, const glm::mat4& value) override;

		virtual void Set(const std::string& name, const TObjectPtr<LTexture2D>& texture) override;
		virtual void Set(const std::string& name, const TObjectPtr<LTexture2D>& texture, uint32_t arrayIndex) override;
		virtual void Set(const std::string& name, const TObjectPtr<LImage2D>& image) override;

		/// FIXME: Do template impl here !!
		virtual float& GetFloat(const std::string& name) override;
		virtual int32_t& GetInt(const std::string& name) override;
		virtual uint32_t& GetUInt(const std::string& name) override;
		virtual bool& GetBool(const std::string& name) override;
		virtual glm::vec2& GetVector2(const std::string& name) override;
		virtual glm::vec3& GetVector3(const std::string& name) override;
		virtual glm::vec4& GetVector4(const std::string& name) override;
		virtual glm::mat3& GetMatrix3(const std::string& name) override;
		virtual glm::mat4& GetMatrix4(const std::string& name) override;

		virtual void SetTexture(TObjectPtr<LTexture> texture) override;
		virtual TObjectPtr<LTexture2D> GetTexture(const std::string& name) override;
		virtual const TObjectPtr<LTexture2D>& GetTexture(const std::string& name) const override;

		virtual TObjectPtr<LTexture2D> TryGetTexture(const std::string& name) override;

		virtual TObjectPtr<LTextureCube> GetTextureCube(const std::string& name) override;
		virtual TObjectPtr<LTextureCube> TryGetTextureCube(const std::string& name) override;

		virtual TObjectPtr<LShader> GetShader() override;
		virtual const std::string& GetName() const override;

    private:
		std::string Name{};
		MaterialSpecification Specification{};

		//TObjectPtr<LOpenGLShader> Shader{};
		TObjectPtr<LShader> Shader{};
		TObjectPtr<LTexture2D> Texture{};

		uint32_t MaterialFlags = 0;
		FBuffer UniformStorageBuffer{};
    };


}