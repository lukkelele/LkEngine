#pragma once

#include <glm/glm.hpp>

#include "LkEngine/Core/Base.h"


namespace LkEngine {

	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		static s_ptr<Shader> Create(const std::string& filepath); 
		static s_ptr<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath); 

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual int GetUniformLocation(const std::string& name) = 0;
		virtual void SetUniform1i(const std::string& name, int value) = 0;
		virtual void SetUniform1f(const std::string& name, float value) = 0;
		virtual void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) = 0;
		virtual void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) = 0;
		virtual unsigned int CompileShader(unsigned int type, const std::string& source) = 0;
		virtual unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) = 0;

		ShaderProgramSource ParseShader(const std::string& filepath);
		ShaderProgramSource ParseShaders(const std::string& vertexPath, const std::string& fragmentPath);

	protected:
		unsigned int m_RendererID;
		std::string m_FilePath;
		std::string m_VertexPath;
		std::string m_FragmentPath;
		std::unordered_map<std::string, int> m_UniformLocationCache;
	};

}
