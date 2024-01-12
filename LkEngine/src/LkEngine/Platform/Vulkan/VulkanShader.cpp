#include "LKpch.h"
#include "VulkanShader.h"


namespace LkEngine {

	VulkanShader::VulkanShader(const std::string& filePath)
	{
		m_FilePath = filePath;   
		ShaderProgramSource source = ParseShader(filePath);
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	VulkanShader::VulkanShader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		m_VertexPath = vertexPath;
		m_FragmentPath = fragmentPath;
		ShaderProgramSource source = ParseShaders(vertexPath, fragmentPath);
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::SetUniform1i(const std::string& name, int value)
	{
	}

	void VulkanShader::SetUniform1f(const std::string& name, float value)
	{
	}

	void VulkanShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
	}

	void VulkanShader::SetUniform4f(const std::string& name, const glm::vec4& vec4)
	{
	}

	void VulkanShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
	}

	int VulkanShader::GetUniformLocation(const std::string& name)
	{
		return 0;
	}

	unsigned int VulkanShader::CompileShader(unsigned int type, const std::string& source)
	{
		return 0;
	}

	unsigned int VulkanShader::CreateShader(const std::string& vertexVulkanShader, const std::string& fragmentVulkanShader)
	{
		return 0;
	}

}