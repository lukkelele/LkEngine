#pragma once

#include "glm/glm.hpp"
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
		Shader() = default;
		Shader(const std::string& filepath);
		Shader(const std::string& vertexPath, const std::string& fragmentPath);
		~Shader();

		static s_ptr<Shader> Create(const std::string& filepath) { return std::make_shared<Shader>(filepath); }
		static s_ptr<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath) { return std::make_shared<Shader>(vertexPath, fragmentPath); }

		void Bind() const;
		void Unbind() const;

		int GetUniformLocation(const std::string& name);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

		ShaderProgramSource ParseShader(const std::string& filepath);
		ShaderProgramSource ParseShaders(const std::string& vertexPath, const std::string& fragmentPath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	private:
		std::string m_FilePath;
		std::string m_VertexPath;
		std::string m_FragmentPath;
		std::unordered_map<std::string, int> m_UniformLocationCache;
		unsigned int m_RendererID;
	};

}
