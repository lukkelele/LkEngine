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

	void VulkanShader::Set(const std::string& name, int value)
	{
	}

	void VulkanShader::Set(const std::string& name, bool value)
	{
	}

	void VulkanShader::Set(const std::string& name, float value)
	{
	}

	void VulkanShader::Set(const std::string& name, uint32_t value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::vec2& value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::vec3& value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::vec4& value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::mat4& value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::ivec2& value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::ivec3& value)
	{
	}

	void VulkanShader::Set(const std::string& name, const glm::ivec4& value)
	{
	}

#if 0
	int VulkanShader::GetUniformLocation(const std::string& name)
	{
		return 0;
	}
#endif

	unsigned int VulkanShader::CompileShader(unsigned int type, const std::string& source)
	{
		return 0;
	}

	unsigned int VulkanShader::CreateShader(const std::string& vertexVulkanShader, const std::string& fragmentVulkanShader)
	{
		return 0;
	}

	std::vector<VkDescriptorSetLayout> VulkanShader::GetAllDescriptorSetLayouts()
	{
		std::vector<VkDescriptorSetLayout> result;
		result.reserve(m_DescriptorSetLayouts.size());
		for (auto& layout : m_DescriptorSetLayouts)
			result.emplace_back(layout);

		return result;
	}

}