#include "LKpch.h"
#include "OpenGLShader.h"


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
			LK_CORE_ERROR_TAG("Shader", "Parse of shader file failed at \"{}\"", InFilePath.string());
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