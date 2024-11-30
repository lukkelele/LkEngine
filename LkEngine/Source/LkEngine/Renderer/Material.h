#pragma once

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    struct FMaterialSpecification
    {
        float Roughness = 0.50f;
        float Density = 1.0f;
        float Friction = 0.30f;

        FMaterialSpecification() = default;
        FMaterialSpecification(float InRoughness, float InDensity, float InFriction) 
            : Roughness(InRoughness) 
            , Density(InDensity)
            , Friction(InFriction) 
		{
		}
    };

	/**
	 * @brief Index of fallback texture incase all other fails.
	 */
	static constexpr int MATERIAL_FALLBACK_TEXTURE = -1;

	/// UPDATE THIS
	/// * Make the const string ref's into string_view
    class LMaterial : public LObject
    {
    public:
        virtual ~LMaterial() = default;

		virtual void Invalidate() = 0;
		virtual void OnShaderReloaded() = 0;

		virtual FBuffer GetUniformStorageBuffer() const = 0;
		virtual FBuffer& GetUniformStorageBuffer() = 0;

		virtual void Set(const std::string& Name, const float Value) = 0;
		virtual void Set(const std::string& Name, const int Value) = 0;
		virtual void Set(const std::string& Name, const uint32_t Value) = 0;
		virtual void Set(const std::string& Name, const bool Value) = 0;
		virtual void Set(const std::string& Name, const glm::vec2& Value) = 0;
		virtual void Set(const std::string& Name, const glm::vec3& Value) = 0;
		virtual void Set(const std::string& Name, const glm::vec4& Value) = 0;
		virtual void Set(const std::string& Name, const glm::ivec2& Value) = 0;
		virtual void Set(const std::string& Name, const glm::ivec3& Value) = 0;
		virtual void Set(const std::string& Name, const glm::ivec4& Value) = 0;

		virtual void Set(const std::string& Name, const glm::mat3& Value) = 0;
		virtual void Set(const std::string& Name, const glm::mat4& Value) = 0;

		virtual void Set(const std::string& Name, const TObjectPtr<LTexture2D>& Texture) = 0;
		virtual void Set(const std::string& Name, const TObjectPtr<LTexture2D>& Texture, const uint32_t ArrayIndex) = 0;
		virtual void Set(const std::string& Name, const TObjectPtr<LImage2D>& Image) = 0;
	#if 0
		virtual void Set(const std::string& Name, const TObjectPtr<LTextureCube>& Texture) = 0;
		virtual void Set(const std::string& Name, const Ref<ImageView>& Image) = 0;
	#endif

		virtual float& GetFloat(const std::string& Name) = 0;
		virtual int32_t& GetInt(const std::string& Name) = 0;
		virtual uint32_t& GetUInt(const std::string& Name) = 0;
		virtual bool& GetBool(const std::string& Name) = 0;
		virtual glm::vec2& GetVector2(const std::string& Name) = 0;
		virtual glm::vec3& GetVector3(const std::string& Name) = 0;
		virtual glm::vec4& GetVector4(const std::string& Name) = 0;
		virtual glm::mat3& GetMatrix3(const std::string& Name) = 0;
		virtual glm::mat4& GetMatrix4(const std::string& Name) = 0;

		virtual void SetTexture(TObjectPtr<LTexture> Texture) = 0;
		virtual void SetTexture(TObjectPtr<LTexture2D> Texture) = 0;

		virtual TObjectPtr<LTexture2D> GetTexture(const std::string& Name = "") = 0;
		virtual const TObjectPtr<LTexture2D>& GetTexture(const std::string& Name = "") const = 0;
		virtual TObjectPtr<LTexture2D> TryGetTexture(const std::string& Name = "") = 0;

		virtual TObjectPtr<LTextureCube> GetTextureCube(const std::string& Name) = 0;
		virtual TObjectPtr<LTextureCube> TryGetTextureCube(const std::string& Name) = 0;

		virtual TObjectPtr<LShader> GetShader() = 0;
		virtual const std::string& GetName() const = 0;

    	static TObjectPtr<LMaterial> Create(const TObjectPtr<LShader>& Shader, const std::string& Name); 
		static TObjectPtr<LMaterial> Copy(const TObjectPtr<LMaterial>& Other, const std::string& Name = "");

	private:
		LCLASS(LMaterial)
    };

}
