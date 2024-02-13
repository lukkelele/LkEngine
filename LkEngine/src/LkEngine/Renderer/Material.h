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

    class Material : public RefCounted
    {
    public:
        virtual ~Material() = default;

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

		virtual void Set(const std::string& name, const Ref<Texture2D>& texture) = 0;
		virtual void Set(const std::string& name, const Ref<Texture2D>& texture, uint32_t arrayIndex) = 0;
		virtual void Set(const std::string& name, const Ref<Image2D>& image) = 0;
		//virtual void Set(const std::string& name, const Ref<TextureCube>& texture) = 0;
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

		virtual void SetTexture(Ref<Texture> texture) = 0;
		virtual Ref<Texture2D> GetTexture(const std::string& name) = 0;
		virtual Ref<Texture2D> TryGetTexture(const std::string& name) = 0;
		virtual Ref<TextureCube> GetTextureCube(const std::string& name) = 0;
		virtual Ref<TextureCube> TryGetTextureCube(const std::string& name) = 0;

		virtual Ref<Shader> GetShader() = 0;
		virtual const std::string& GetName() const = 0;

    	static Ref<Material> Create(const Ref<Shader>& shader, const std::string& name = "");
		static Ref<Material> Copy(const Ref<Material>& other, const std::string& name = "");
    };

}