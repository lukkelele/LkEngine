#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Core/Hash.h"

#include "ShaderUniform.h"
#include "ShaderResource.h"

#include "Texture.h"


namespace LkEngine {

	class LShader : public LObject
	{
	public:
		virtual ~LShader() = default;

		static TObjectPtr<LShader> Create(const std::string& InFilepath); 
		static TObjectPtr<LShader> Create(const std::string& InVertexPath, 
										  const std::string& fragmentPath); 

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, uint32_t value) = 0;
		virtual void Set(const std::string& name, const glm::vec2& value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec2& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec3& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec4& value) = 0;
		virtual void Set(const std::string& name, const glm::mat4& value) = 0;

		virtual unsigned int CompileShader(unsigned int type, const std::string& source) = 0;
		virtual unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) = 0;

		ShaderProgramSource ParseShader(const std::string& filepath);
		ShaderProgramSource ParseShaders(const std::string& vertexPath, const std::string& fragmentPath);

		virtual size_t GetHash() = 0;

		FORCEINLINE static const char* ConvertUniformType(const TextureUniformType& type)
		{
			switch (type)
			{
				case TextureUniformType::Diffuse:          return "u_Diffuse";
				case TextureUniformType::Specular:         return "u_Specular";
				case TextureUniformType::Normal:           return "u_Normal"; // rename ambient?
				case TextureUniformType::Height:           return "u_Height";
				case TextureUniformType::DiffuseRoughness: return "u_DiffuseRoughness";
				case TextureUniformType::Emissive:         return "u_Emissive";
			}

			LK_CORE_ASSERT(false, "Could not convert the TextureUniformType {}", (int)type);
		}

	};

    /**
     * LShaderLibrary
	 * 
	 *  Library for shaders.
     */
    class LShaderLibrary : public LObject
    {
    public:
		LShaderLibrary() = default;
		~LShaderLibrary() = default;

        TObjectPtr<LShader>& Get(std::string_view shaderName);

        void Add(const TObjectPtr<LShader>& shader);
		void Load(std::string_view InName, const std::string& InFilepath);

    private:
        inline static std::unordered_map<std::string_view, TObjectPtr<LShader>> Shaders;
    };


}
