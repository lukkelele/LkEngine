#pragma once

#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    class OpenGLShader : public LShader
    {
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath);
		~OpenGLShader() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		/// FIXME: TEMPLATES
		virtual void Set(const std::string& name, int value) override;
		virtual void Set(const std::string& name, bool value) override;
		virtual void Set(const std::string& name, float value) override;;
		virtual void Set(const std::string& name, uint32_t value) override;
		virtual void Set(const std::string& name, const glm::vec2& value) override;
		virtual void Set(const std::string& name, const glm::vec3& value) override;
		virtual void Set(const std::string& name, const glm::vec4& value) override; 
		virtual void Set(const std::string& name, const glm::ivec2& value) override;
		virtual void Set(const std::string& name, const glm::ivec3& value) override;
		virtual void Set(const std::string& name, const glm::ivec4& value) override;
		virtual void Set(const std::string& name, const glm::mat4& value) override;
		
		int GetUniformLocation(const std::string& name);

		virtual RendererID GetRendererID() const override { return m_RendererID; }
		virtual RendererID& GetRendererID() override { return m_RendererID; }

		virtual unsigned int CompileShader(unsigned int type, const std::string& source) override;
		virtual unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) override;

		virtual size_t GetHash() override;

	private:
		RendererID m_RendererID;
		std::unordered_map<std::string, int> m_UniformLocationCache;

		std::filesystem::path m_FilePath{};

		std::string m_VertexPath{};
		std::string m_FragmentPath{};
    };
}