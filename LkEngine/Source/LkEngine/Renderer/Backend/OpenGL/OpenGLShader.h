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
			LK_OpenGL_Verify(glUseProgram(RendererID));
		}

		FORCEINLINE virtual void Unbind() const override
		{
			LK_OpenGL_Verify(glUseProgram(0));
		}

		FORCEINLINE virtual LRendererID GetRendererID() const override { return RendererID; }
		FORCEINLINE virtual LRendererID& GetRendererID() override { return RendererID; }

		virtual void Get(std::string_view Uniform, int& Value) override;
		virtual void Get(std::string_view Uniform, bool& Value) override;
		virtual void Get(std::string_view Uniform, float& Value) override;
		virtual void Get(std::string_view Uniform, uint32_t& Value) override;
		virtual void Get(std::string_view Uniform, glm::vec3& Value) override;
		virtual void Get(std::string_view Uniform, glm::vec4& Value) override;

		virtual void Set(std::string_view Uniform, const int Value) override;
		virtual void Set(std::string_view Uniform, const bool Value) override;
		virtual void Set(std::string_view Uniform, const float Value) override;
		virtual void Set(std::string_view Uniform, const uint32_t Value) override;
		virtual void Set(std::string_view Uniform, const glm::vec2& Value) override;
		virtual void Set(std::string_view Uniform, const glm::vec3& Value) override;
		virtual void Set(std::string_view Uniform, const glm::vec4& Value) override;
		virtual void Set(std::string_view Uniform, const glm::ivec2& Value) override;
		virtual void Set(std::string_view Uniform, const glm::ivec3& Value) override;
		virtual void Set(std::string_view Uniform, const glm::ivec4& Value) override;
		virtual void Set(std::string_view Uniform, const glm::mat4& Value) override;
		
		virtual std::size_t GetHash() const override
		{
			return LHash::Generate<EHash::FNV>(FilePath.string());
		}

		virtual std::string GetName() const override
		{
			if (FilePath.has_filename())
			{
				return FilePath.filename().string();
			}
			return "UNKNOWN";
		}

		FORCEINLINE virtual const FShaderProgramSource& GetSource() const override 
		{ 
			return Source; 
		}

	private:
		/**
		 * @brief Get location of a uniform.
         * 
		 * Do not use any string view types for the passed argument here.
		 * Need to extend the lifetime of the string to be sure call to OpenGL doesn't fail.
		 */
		FORCEINLINE int GetUniformLocation(const std::string& Uniform)
		{
			if (UniformLocationCache.find(Uniform) != UniformLocationCache.end())
			{
				return UniformLocationCache[Uniform];
			}

			int UniformLocation;
			LK_OpenGL_Verify(UniformLocation = glGetUniformLocation(RendererID, Uniform.c_str()));
			if (UniformLocation == -1)
			{
				LK_CORE_WARN_TAG("Shader", "Uniform '{}' is not in use ({})", Uniform, FilePath.filename().string());
			}

			/* TODO: Should caching be placed in 'else' statement here? */
			UniformLocationCache[Uniform] = UniformLocation;

			return UniformLocation;
		}

		virtual uint32_t CompileShader(const uint32_t InShaderType, const std::string& Source) override;
		virtual uint32_t CreateShader(const FShaderProgramSource& ShaderProgramSource) override;

	private:
		LRendererID RendererID = 0;

		std::unordered_map<std::string, int> UniformLocationCache;
		std::filesystem::path FilePath{};

		FShaderProgramSource Source{};

		LCLASS(LOpenGLShader);
    };
}
