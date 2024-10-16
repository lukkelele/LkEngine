#include "LKpch.h"
#include "OpenGLShader.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		m_FilePath = std::filesystem::path(filepath);   
		ShaderProgramSource source = ParseShader(filepath);
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
		LK_OpenGL(glDeleteProgram(m_RendererID));
	}

	void OpenGLShader::Bind() const
	{
		LK_OpenGL(glUseProgram(m_RendererID));
	}

	void OpenGLShader::Unbind() const
	{
		LK_OpenGL(glUseProgram(0));
	}

	void OpenGLShader::Set(const std::string& name, int value)
	{
		LK_OpenGL(glUseProgram(m_RendererID));
		LK_OpenGL(glUniform1i(GetUniformLocation(name), value));
	}

	void OpenGLShader::Set(const std::string& name, bool value)
	{
		LK_OpenGL(glUseProgram(m_RendererID));
		LK_OpenGL(glUniform1i(GetUniformLocation(name), (int)value));
	}

	void OpenGLShader::Set(const std::string& name, float value)
	{
		LK_OpenGL(glUseProgram(m_RendererID));
		LK_OpenGL(glUniform1f(GetUniformLocation(name), value));
	}

	void OpenGLShader::Set(const std::string& name, uint32_t value)
	{
		LK_OpenGL(glUseProgram(m_RendererID));
		LK_OpenGL(glUniform1i(GetUniformLocation(name), value));
	}

	void OpenGLShader::Set(const std::string& name, const glm::vec4& value)
	{
		LK_OpenGL(glUseProgram(m_RendererID));
		LK_OpenGL(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w));
	}

	void OpenGLShader::Set(const std::string& name, const glm::vec2& value)
	{
		LK_OpenGL(glUseProgram(m_RendererID));
		LK_OpenGL(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0]));
	}

	void OpenGLShader::Set(const std::string& name, const glm::vec3& value)
	{
		glUseProgram(m_RendererID);
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0]);
	}

	void OpenGLShader::Set(const std::string& name, const glm::ivec2& value)
	{
		glUseProgram(m_RendererID);
		glUniform2i(GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::Set(const std::string& name, const glm::ivec3& value)
	{
		glUseProgram(m_RendererID);
		glUniform3i(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::Set(const std::string& name, const glm::ivec4& value)
	{
		glUseProgram(m_RendererID);
		glUniform4i(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::Set(const std::string& name, const glm::mat4& value)
	{
		glUseProgram(m_RendererID);
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int location;
		LK_OpenGL(location = glGetUniformLocation(m_RendererID, name.c_str()));
		if (location == -1)
			LK_CORE_WARN("[SHADER] Warning: uniform {0} isn't in use", name);

		m_UniformLocationCache[name] = location;
		//LK_CORE_DEBUG("{} in uniform cache --> m_UniformLocationCache[{}] == {}", name, name, location);
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
			LK_CORE_FATAL_TAG("OpenGLShader", "Failed to compile {} shader at {}, \"{}\"", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), m_FilePath, message);

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
		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}

	size_t OpenGLShader::GetHash()
	{
		return Hash::GenerateFNVHash(m_FilePath.string());
	}

}