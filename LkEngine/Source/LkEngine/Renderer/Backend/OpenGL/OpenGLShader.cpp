#include "LKpch.h"
#include "OpenGLShader.h"

#include "LkEngine/Core/IO/FileSystem.h"


namespace LkEngine {

	LOpenGLShader::LOpenGLShader(const std::filesystem::path& InFilePath)
		: FilePath(InFilePath)
	{
		LOBJECT_REGISTER();

		FShaderProgramSource ShaderProgramSource;
		if (ParseShader(ShaderProgramSource, InFilePath))
		{
			RendererID = CreateShader(ShaderProgramSource);
			Source = ShaderProgramSource;
		}
		else
		{
			LK_CORE_VERIFY(false, "Parsing of shader failed: '{}'", std::filesystem::relative(InFilePath, LFileSystem::GetEngineDir()).string());
		}
	}

	LOpenGLShader::LOpenGLShader(const FShaderProgramSource& ShaderProgramSource)
	{
		LOBJECT_REGISTER();

		if (ShaderProgramSource.IsValid())
		{
			RendererID = CreateShader(ShaderProgramSource);
		}
	}

	LOpenGLShader::~LOpenGLShader()
	{
		if (RendererID > 0)
		{
			LK_OpenGL_Verify(glDeleteProgram(RendererID));
		}
	}

	void LOpenGLShader::Get(std::string_view Uniform, int& Value) 
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glGetUniformiv(RendererID, GetUniformLocation(Uniform.data()), &Value));
	}

	void LOpenGLShader::Get(std::string_view Uniform, bool& Value) 
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		int ValueRef = 0;
		LK_OpenGL_Verify(glGetUniformiv(RendererID, GetUniformLocation(Uniform.data()), &ValueRef));
		Value = static_cast<bool>(ValueRef);
	}

	void LOpenGLShader::Get(std::string_view Uniform, float& Value) 
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glGetUniformfv(RendererID, GetUniformLocation(Uniform.data()), &Value));
	}

	void LOpenGLShader::Get(std::string_view Uniform, uint32_t& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glGetUniformuiv(RendererID, GetUniformLocation(Uniform.data()), &Value));
	}

	void LOpenGLShader::Get(std::string_view Uniform, glm::vec3& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		//float ValueRef[3]{};
		if (const auto Location = GetUniformLocation(Uniform.data()); Location != -1)
		{
			float ValueRef[3]{};
			LK_OpenGL_Verify(glGetUniformfv(RendererID, Location, ValueRef));
			//LK_OpenGL_Verify(glGetnUniformfv(RendererID, GetUniformLocation(Uniform.data()), 3, ValueRef));

			Value.x = ValueRef[0];
			Value.y = ValueRef[1];
			Value.z = ValueRef[2];
		}
		else
		{
			LK_CORE_ERROR("Failed to get location for: {} (glm::vec3)", Uniform);
			//LK_OpenGL_Verify(glGetUniformfv(RendererID, GetUniformLocation(Uniform.data()), ValueRef));
		}
	}

	void LOpenGLShader::Get(std::string_view Uniform, glm::vec4& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		float ValueRef[4]{};
		LK_OpenGL_Verify(glGetUniformfv(RendererID, GetUniformLocation(Uniform.data()), ValueRef));

		Value.x = ValueRef[0];
		Value.y = ValueRef[1];
		Value.z = ValueRef[2];
		Value.w = ValueRef[3];
	}

	void LOpenGLShader::Set(std::string_view Uniform, const int Value) 
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform1i(GetUniformLocation(Uniform.data()), Value));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const bool Value) 
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform1i(GetUniformLocation(Uniform.data()), static_cast<int>(Value)));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const float Value) 
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform1f(GetUniformLocation(Uniform.data()), Value));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const uint32_t Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform1i(GetUniformLocation(Uniform.data()), Value));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::vec2& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform2f(GetUniformLocation(Uniform.data()), Value.x, Value.y));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::vec3& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform3f(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::vec4& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform4f(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z, Value.w));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::ivec2& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform2i(GetUniformLocation(Uniform.data()), Value.x, Value.y));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::ivec3& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform3i(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::ivec4& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniform4i(GetUniformLocation(Uniform.data()), Value.x, Value.y, Value.z, Value.w));
	}

	void LOpenGLShader::Set(std::string_view Uniform, const glm::mat4& Value)
	{
		LK_OpenGL_Verify(glUseProgram(RendererID));
		LK_OpenGL_Verify(glUniformMatrix4fv(GetUniformLocation(Uniform.data()), 1, GL_FALSE, &Value[0][0]));
	}

	uint32_t LOpenGLShader::CompileShader(const uint32_t ShaderType, const std::string& ShaderSource)
	{
		static_assert(sizeof(uint32_t) == sizeof(GLuint), "Type size mismatch");
		uint32_t ShaderID;
		LK_OpenGL_Verify(ShaderID = glCreateShader(ShaderType));

		const char* Source = ShaderSource.c_str();
		LK_OpenGL_Verify(glShaderSource(ShaderID, 1, &Source, nullptr)); /* TODO: Check if works with &ShaderSource.c_str() */
		LK_OpenGL_Verify(glCompileShader(ShaderID));

		/* Error handling. */
		int Result;
		LK_OpenGL_Verify(glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result));
		if (Result == GL_FALSE)
		{
			int Length;
			LK_OpenGL_Verify(glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &Length));

			char* ErrorMessage = (char*)_malloca(Length * sizeof(char));
			LK_OpenGL_Verify(glGetShaderInfoLog(ShaderID, Length, &Length, ErrorMessage));
			LK_CORE_ERROR_TAG("OpenGLShader", "Failed to compile {} shader at {}, \"{}\"", 
							  ((ShaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment"), FilePath.string(), ErrorMessage);
			LK_OpenGL_Verify(glDeleteShader(ShaderID));

			return 0;
		}

		return ShaderID;
	}

	uint32_t LOpenGLShader::CreateShader(const FShaderProgramSource& ShaderProgramSource)
	{
		uint32_t ShaderProgram;
		LK_OpenGL_Verify(ShaderProgram = glCreateProgram());

		const uint32_t VertexShader = CompileShader(GL_VERTEX_SHADER, ShaderProgramSource.Vertex);
		const uint32_t FragmentShader = CompileShader(GL_FRAGMENT_SHADER, ShaderProgramSource.Fragment);
		
		/* Attach shaders. */
		LK_OpenGL_Verify(glAttachShader(ShaderProgram, VertexShader));
		LK_OpenGL_Verify(glAttachShader(ShaderProgram, FragmentShader));

		/* Link and validate. */
		LK_OpenGL_Verify(glLinkProgram(ShaderProgram));
		LK_OpenGL_Verify(glValidateProgram(ShaderProgram));

		/* Delete shader resources after shader programs are created and validated. */
		LK_OpenGL_Verify(glDeleteShader(VertexShader));
		LK_OpenGL_Verify(glDeleteShader(FragmentShader));

		return ShaderProgram;
	}

}
