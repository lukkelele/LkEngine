#pragma once

#include "LkEngine/Renderer/Shader.h"

#include "LkOpenGL.h"


namespace LkEngine {

    class LOpenGLShader : public LShader
    {
	public:
		LOpenGLShader(const std::filesystem::path& InFilePath);
		LOpenGLShader(const FShaderProgramSource& ShaderProgramSource);
		LOpenGLShader() = delete;
		~LOpenGLShader();

		FORCEINLINE virtual void Bind() const override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
		}

		FORCEINLINE virtual void Unbind() const override
		{
			LK_OpenGL(glUseProgram(0));
		}

		FORCEINLINE virtual RendererID GetRendererID() const override { return m_RendererID; }
		FORCEINLINE virtual RendererID& GetRendererID() override { return m_RendererID; }

		FORCEINLINE virtual void Set(std::string_view Uniform, const int Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform1i(GetUniformLocation(Uniform.data()), Value));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const bool Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform1i(GetUniformLocation(Uniform.data()), static_cast<int>(Value)));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const float Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform1f(GetUniformLocation(Uniform.data()), Value));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const uint32_t Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform1i(GetUniformLocation(Uniform.data()), Value));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::vec2& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID)); 
			LK_OpenGL(glUniform2f(GetUniformLocation(Uniform.data()), Value.x, Value.y));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::vec3& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform3f(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::vec4& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform4f(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z, Value.w));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::ivec2& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform2i(GetUniformLocation(Uniform.data()), Value.x, Value.y));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::ivec3& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform3i(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::ivec4& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniform4i(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z, Value.w));
		}

		FORCEINLINE virtual void Set(std::string_view Uniform, const glm::mat4& Value) override
		{
			LK_OpenGL(glUseProgram(m_RendererID));
			LK_OpenGL(glUniformMatrix4fv(GetUniformLocation(Uniform.data()), 1, GL_FALSE, &Value[0][0]));
		}
		
		FORCEINLINE virtual std::size_t GetHash() override
		{
			return Hash::GenerateFNVHash(FilePath.string());
		}

	private:
		/**
		 * @brief Get location of a uniform.
         * 
		 * Do not use any string view types for the passed argument here.
		 * Need to extend the string lifetime to be sure call to OpenGL doesn't fail.
		 */
		FORCEINLINE int GetUniformLocation(const std::string& Uniform)
		{
			if (UniformLocationCache.find(Uniform) != UniformLocationCache.end())
			{
				return UniformLocationCache[Uniform];
			}

			int UniformLocation;
			LK_OpenGL(UniformLocation = glGetUniformLocation(m_RendererID, Uniform.c_str()));
			if (UniformLocation == -1)
			{
				LK_CORE_WARN_TAG("Shader", "Uniform \"{}\" is not in use", Uniform);
			}

			/* TODO: Should caching be placed in 'else' statement here? */
			UniformLocationCache[Uniform] = UniformLocation;

			return UniformLocation;
		}

		virtual uint32_t CompileShader(const uint32_t InShaderType, const std::string& Source) override;
		virtual uint32_t CreateShader(const FShaderProgramSource& ShaderProgramSource) override;

	private:
		RendererID m_RendererID = 0;

		std::unordered_map<std::string, int> UniformLocationCache;
		std::filesystem::path FilePath{};

		LCLASS(LOpenGLShader);
    };
}