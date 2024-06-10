#pragma once

#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    class OpenGLShader : public Shader
    {
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath);
		~OpenGLShader() override;

		void Set(const std::string& name, int value) override;
		void Set(const std::string& name, bool value) override;
		void Set(const std::string& name, float value) override;;
		void Set(const std::string& name, uint32_t value) override;
		void Set(const std::string& name, const glm::vec2& value) override;
		void Set(const std::string& name, const glm::vec3& value) override;
		void Set(const std::string& name, const glm::vec4& value) override; 
		void Set(const std::string& name, const glm::ivec2& value) override;
		void Set(const std::string& name, const glm::ivec3& value) override;
		void Set(const std::string& name, const glm::ivec4& value) override;
		void Set(const std::string& name, const glm::mat4& value) override;
		
		void Bind() const override;
		void Unbind() const override;

		int GetUniformLocation(const std::string& name);

		RendererID GetRendererID() const override { return m_RendererID; }
		RendererID& GetRendererID() override { return m_RendererID; }

		unsigned int CompileShader(unsigned int type, const std::string& source) override;
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) override;

		size_t GetHash() override;

	private:
		RendererID m_RendererID;
		std::unordered_map<std::string, int> m_UniformLocationCache;

		std::filesystem::path m_FilePath;
		std::string m_VertexPath, m_FragmentPath;
    };
}