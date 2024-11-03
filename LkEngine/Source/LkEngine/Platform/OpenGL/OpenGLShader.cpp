#include "LKpch.h"
#include "OpenGLShader.h"


namespace LkEngine {

	LOpenGLShader::LOpenGLShader(const std::filesystem::path& InFilePath)
		: FilePath(InFilePath)
	{
		LCLASS_REGISTER();

		FShaderProgramSource ShaderProgramSource;
		if (ParseShader(ShaderProgramSource, InFilePath))
		{
			m_RendererID = CreateShader(ShaderProgramSource);
		}
		else
		{
			LK_CORE_ERROR_TAG("Shader", "Parse of shader file failed at \"{}\"", InFilePath.string());
		}
	}

	LOpenGLShader::LOpenGLShader(const FShaderProgramSource& ShaderProgramSource)
	{
		LCLASS_REGISTER();

		if (ShaderProgramSource.IsValid())
		{
			m_RendererID = CreateShader(ShaderProgramSource);
		}
	}

	LOpenGLShader::~LOpenGLShader()
	{
		if (m_RendererID > 0)
		{
			LK_OpenGL(glDeleteProgram(m_RendererID));
		}
	}

	uint32_t LOpenGLShader::CompileShader(const uint32_t ShaderType, const std::string& ShaderSource)
	{
		static_assert(sizeof(uint32_t) == sizeof(GLuint), "Type size mismatch");
		uint32_t ShaderID;
		LK_OpenGL(ShaderID = glCreateShader(ShaderType));

		const char* Source = ShaderSource.c_str();
		LK_OpenGL(glShaderSource(ShaderID, 1, &Source, nullptr)); /* TODO: Check if works with &ShaderSource.c_str() */
		LK_OpenGL(glCompileShader(ShaderID));

		/* Error handling. */
		int Result;
		LK_OpenGL(glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result));
		if (Result == GL_FALSE)
		{
			int Length;
			LK_OpenGL(glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &Length));

			char* ErrorMessage = (char*)_malloca(Length * sizeof(char));
			LK_OpenGL(glGetShaderInfoLog(ShaderID, Length, &Length, ErrorMessage));
			LK_CORE_ERROR_TAG("OpenGLShader", "Failed to compile {} shader at {}, \"{}\"", 
							  ((ShaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment"), FilePath.string(), ErrorMessage);
			LK_OpenGL(glDeleteShader(ShaderID));

			return 0;
		}

		return ShaderID;
	}

	uint32_t LOpenGLShader::CreateShader(const FShaderProgramSource& ShaderProgramSource)
	{
		uint32_t ShaderProgram;
		LK_OpenGL(ShaderProgram = glCreateProgram());

		const uint32_t VertexShader = CompileShader(GL_VERTEX_SHADER, ShaderProgramSource.Vertex);
		const uint32_t FragmentShader = CompileShader(GL_FRAGMENT_SHADER, ShaderProgramSource.Fragment);
		
		/* Attach shaders. */
		LK_OpenGL(glAttachShader(ShaderProgram, VertexShader));
		LK_OpenGL(glAttachShader(ShaderProgram, FragmentShader));

		/* Link and validate. */
		LK_OpenGL(glLinkProgram(ShaderProgram));
		LK_OpenGL(glValidateProgram(ShaderProgram));

		/* Delete shader resources after shader programs are created and validated. */
		LK_OpenGL(glDeleteShader(VertexShader));
		LK_OpenGL(glDeleteShader(FragmentShader));

		return ShaderProgram;
	}

}