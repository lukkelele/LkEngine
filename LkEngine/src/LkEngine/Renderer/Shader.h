#pragma once

#include "ShaderUniform.h"
#include "ShaderResource.h"

#include "Texture.h"


namespace LkEngine {

	class Shader : public RefCounted
	{
	public:
		virtual ~Shader() = default;

		static Ref<Shader> Create(const std::string& filepath); 
		static Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath); 

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

		static const char* ConvertUniformType(const TextureUniformType& type)
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


    class ShaderLibrary : public RefCounted
    {
    public:
        ShaderLibrary();
        ~ShaderLibrary();

		static Ref<ShaderLibrary> GetLibrary() { return m_Instance; }

        Ref<Shader>& Get(std::string_view shaderName);

        void Add(const Ref<Shader>& shader);
		void Load(std::string_view name, const std::string& path);

    private:
        std::unordered_map<std::string_view, Ref<Shader>> m_Shaders;

		inline static Ref<ShaderLibrary> m_Instance = nullptr;
    };


}
