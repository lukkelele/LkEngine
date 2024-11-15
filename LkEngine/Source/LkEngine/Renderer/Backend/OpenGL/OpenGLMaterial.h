#pragma once

#include "LkEngine/Renderer/Material.h"
#include "LkEngine/Renderer/Shader.h"

#include "OpenGLShader.h"


namespace LkEngine {

    class LOpenGLMaterial : public LMaterial
    {
    public:
    	LOpenGLMaterial(const TObjectPtr<LShader>& Shader, const std::string& Name);
		LOpenGLMaterial(TObjectPtr<LMaterial> Material, const std::string& Name);
		~LOpenGLMaterial() = default;

		virtual void Invalidate() override;
		virtual void OnShaderReloaded() override;

		FORCEINLINE virtual FBuffer& GetUniformStorageBuffer() override { return UniformStorageBuffer; }
		FORCEINLINE virtual FBuffer GetUniformStorageBuffer() const override { return UniformStorageBuffer; }

		virtual void Set(const std::string& Name, const int Value) override;
		virtual void Set(const std::string& Name, const bool Value) override;
		virtual void Set(const std::string& Name, const float Value) override;
		virtual void Set(const std::string& Name, const uint32_t Value) override;
		virtual void Set(const std::string& Name, const glm::vec2& Value) override;
		virtual void Set(const std::string& Name, const glm::vec3& Value) override;
		virtual void Set(const std::string& Name, const glm::vec4& Value) override;
		virtual void Set(const std::string& Name, const glm::ivec2& Value) override;
		virtual void Set(const std::string& Name, const glm::ivec3& Value) override;
		virtual void Set(const std::string& Name, const glm::ivec4& Value) override;

		virtual void Set(const std::string& Name, const glm::mat3& Value) override;
		virtual void Set(const std::string& Name, const glm::mat4& Value) override;

		virtual void Set(const std::string& Name, const TObjectPtr<LTexture2D>& Texture) override;
		virtual void Set(const std::string& Name, const TObjectPtr<LTexture2D>& Texture, 
						 const uint32_t ArrayIndex) override;
		virtual void Set(const std::string& Name, const TObjectPtr<LImage2D>& Image) override;

		/// FIXME: Do template impl here !!
		virtual float& GetFloat(const std::string& Name) override;
		virtual int32_t& GetInt(const std::string& Name) override;
		virtual uint32_t& GetUInt(const std::string& Name) override;
		virtual bool& GetBool(const std::string& Name) override;
		virtual glm::vec2& GetVector2(const std::string& Name) override;
		virtual glm::vec3& GetVector3(const std::string& Name) override;
		virtual glm::vec4& GetVector4(const std::string& Name) override;
		virtual glm::mat3& GetMatrix3(const std::string& Name) override;
		virtual glm::mat4& GetMatrix4(const std::string& Name) override;

		/// @TODO: Allow for multiple textures per material.
		virtual void SetTexture(TObjectPtr<LTexture> Texture) override;
		virtual TObjectPtr<LTexture2D> GetTexture(const std::string& Name = "") override;
		virtual const TObjectPtr<LTexture2D>& GetTexture(const std::string& Name) const override;

		virtual TObjectPtr<LTexture2D> TryGetTexture(const std::string& Name) override;

		virtual TObjectPtr<LTextureCube> GetTextureCube(const std::string& Name) override;
		virtual TObjectPtr<LTextureCube> TryGetTextureCube(const std::string& Name) override;

		FORCEINLINE virtual TObjectPtr<LShader> GetShader() override
		{
			return Shader;
		}

		FORCEINLINE virtual const std::string& GetName() const override
		{
			return Name;
		}

    private:
		std::string Name{};
		FMaterialSpecification Specification{};

		TObjectPtr<LShader> Shader{};
		TObjectPtr<LTexture2D> Texture{};

		uint32_t MaterialFlags = 0;
		FBuffer UniformStorageBuffer{};
    };


}