#pragma once

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    struct MaterialSpecification
    {
        float Roughness = 0.50f;
        float Density = 1.0f;
        float Friction = 0.30f;

        MaterialSpecification() = default;
        MaterialSpecification(float roughness, float density, float friction) 
            : Roughness(roughness) 
            , Density(density)
            , Friction(friction) {}
    };

	/// UPDATE THIS...
    class LMaterial : public LObject
    {
    public:
        virtual ~LMaterial() = default;

		virtual void Invalidate() = 0;
		virtual void OnShaderReloaded() = 0;

		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, uint32_t value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, const glm::vec2& value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec2& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec3& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec4& value) = 0;

		virtual void Set(const std::string& name, const glm::mat3& value) = 0;
		virtual void Set(const std::string& name, const glm::mat4& value) = 0;

		virtual void Set(const std::string& name, const TObjectPtr<LTexture2D>& texture) = 0;
		virtual void Set(const std::string& name, 
						 const TObjectPtr<LTexture2D>& texture, 
						 uint32_t arrayIndex) = 0;
		virtual void Set(const std::string& name, const TObjectPtr<LImage2D>& image) = 0;
		//virtual void Set(const std::string& name, const TObjectPtr<LTextureCube>& texture) = 0;
		//virtual void Set(const std::string& name, const Ref<ImageView>& image) = 0;

		virtual float& GetFloat(const std::string& name) = 0;
		virtual int32_t& GetInt(const std::string& name) = 0;
		virtual uint32_t& GetUInt(const std::string& name) = 0;
		virtual bool& GetBool(const std::string& name) = 0;
		virtual glm::vec2& GetVector2(const std::string& name) = 0;
		virtual glm::vec3& GetVector3(const std::string& name) = 0;
		virtual glm::vec4& GetVector4(const std::string& name) = 0;
		virtual glm::mat3& GetMatrix3(const std::string& name) = 0;
		virtual glm::mat4& GetMatrix4(const std::string& name) = 0;

		virtual void SetTexture(TObjectPtr<LTexture> texture) = 0;

		virtual TObjectPtr<LTexture2D> GetTexture(const std::string& name = "") = 0;
		virtual const TObjectPtr<LTexture2D>& GetTexture(const std::string& name = "") const = 0;
		virtual TObjectPtr<LTexture2D> TryGetTexture(const std::string& name = "") = 0;

		virtual TObjectPtr<LTextureCube> GetTextureCube(const std::string& name) = 0;
		virtual TObjectPtr<LTextureCube> TryGetTextureCube(const std::string& name) = 0;

		virtual TObjectPtr<LShader> GetShader() = 0;
		virtual const std::string& GetName() const = 0;

    	static TObjectPtr<LMaterial> Create(const TObjectPtr<LShader>& Shader, const std::string& Name = "");
		static TObjectPtr<LMaterial> Copy(const TObjectPtr<LMaterial>& Other, const std::string& name = "");

	private:
		LCLASS(LMaterial)
    };

}