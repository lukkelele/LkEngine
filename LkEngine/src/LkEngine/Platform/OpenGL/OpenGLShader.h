#pragma once

#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    class OpenGLShader : public Shader
    {
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath);
		~OpenGLShader();

		RendererID GetRendererID() const { return m_RendererID; }
		RendererID& GetRendererID() { return m_RendererID; }
		void Bind() const;
		void Unbind() const;
		int GetUniformLocation(const std::string& name);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform4f(const std::string& name, const glm::vec4& vec4);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	private:
		RendererID m_RendererID;
		std::string m_FilePath;
		std::string m_VertexPath;
		std::string m_FragmentPath;
		std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}