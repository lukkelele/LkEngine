#include "LKpch.h"
#include "OpenGLShader.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		m_FilePath = filePath;   
		ShaderProgramSource source = ParseShader(filePath);
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	OpenGLShader::OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		m_VertexPath = vertexPath;
		m_FragmentPath = fragmentPath;
		ShaderProgramSource source = ParseShaders(vertexPath, fragmentPath);
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	OpenGLShader::~OpenGLShader()
	{
		GL_CALL(glDeleteProgram(m_RendererID));
	}

	void OpenGLShader::Bind() const
	{
		GL_CALL(glUseProgram(m_RendererID));
	}

	void OpenGLShader::Unbind() const
	{
		GL_CALL(glUseProgram(0));
	}

	void OpenGLShader::SetUniform1i(const std::string& name, int value)
	{
		GL_CALL(glUniform1i(GetUniformLocation(name), value));
	}

	void OpenGLShader::SetUniform1f(const std::string& name, float value)
	{
		GL_CALL(glUniform1f(GetUniformLocation(name), value));
	}

	void OpenGLShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		int location = GetUniformLocation(name);
		GL_CALL(glUniform4f(location, v0, v1, v2, v3));
	}

	void OpenGLShader::SetUniform4f(const std::string& name, const glm::vec4& vec4)
	{
		int location = GetUniformLocation(name);
		GL_CALL(glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w));
	}

	void OpenGLShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
		GL_CALL(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
	}

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int location;
		GL_CALL(location = glGetUniformLocation(m_RendererID, name.c_str()));
		if (location == -1)
			LOG_WARN("[SHADER] Warning: uniform {0} isn't in use", name);

		m_UniformLocationCache[name] = location;
		return location;
	}

	unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();   // &source[0] 
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		// Error handling
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)_malloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			printf("Failed to compile %s shader!\n%s", type == GL_VERTEX_SHADER ? "vertex" : "fragment", message);
			glDeleteShader(id);
			return 0;
		}
		return id;
	}

	unsigned int OpenGLShader::CreateShader(const std::string& vertexOpenGLShader, const std::string& fragmentOpenGLShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexOpenGLShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentOpenGLShader);
		
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		// NOTE: Detach shaders instead of deleting IF needed for debugging 
		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}


}