#pragma once

#include "LkEngine/Renderer/Shader.h"

#include "VulkanShaderResource.h"


namespace LkEngine {

    class VulkanShader : public Shader
    {
		struct ReflectionData
		{
			std::vector<ShaderResource::ShaderDescriptorSet> ShaderDescriptorSets;
			std::unordered_map<std::string, ShaderResourceDeclaration> Resources;
			std::unordered_map<std::string, ShaderBuffer> ConstantBuffers;
			std::vector<ShaderResource::PushConstantRange> PushConstantRanges;
		};
	public:
		VulkanShader() = default;
		VulkanShader(const std::string& filepath);
		VulkanShader(const std::string& vertexPath, const std::string& fragmentPath);
		~VulkanShader();

		void Set(const std::string& name, int value) override;
		void Set(const std::string& name, bool value) override;
		void Set(const std::string& name, float value) override;
		void Set(const std::string& name, uint32_t value) override;
		void Set(const std::string& name, const glm::vec2& value) override;
		void Set(const std::string& name, const glm::vec3& value) override;
		void Set(const std::string& name, const glm::vec4& value) override;
		void Set(const std::string& name, const glm::ivec2& value) override;   
		void Set(const std::string& name, const glm::ivec3& value) override;  
		void Set(const std::string& name, const glm::ivec4& value) override; 
		void Set(const std::string& name, const glm::mat4& value) override;

		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

		RendererID GetRendererID() const { return m_RendererID; }
		RendererID& GetRendererID() { return m_RendererID; }

		void Bind() const;
		void Unbind() const;

		const std::vector<VkPipelineShaderStageCreateInfo>& GetPipelineShaderStageCreateInfos() const { return m_PipelineShaderStageCreateInfos; }
		const std::vector<ShaderResource::ShaderDescriptorSet>& GetShaderDescriptorSets() const { return m_ReflectionData.ShaderDescriptorSets; }

		VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
		VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t set) { return m_DescriptorSetLayouts.at(set); }
		std::vector<VkDescriptorSetLayout> GetAllDescriptorSetLayouts();

		ShaderResource::UniformBuffer& GetUniformBuffer(const uint32_t binding = 0, const uint32_t set = 0) { LK_CORE_ASSERT(m_ReflectionData.ShaderDescriptorSets.at(set).UniformBuffers.size() > binding); return m_ReflectionData.ShaderDescriptorSets.at(set).UniformBuffers.at(binding); }
		const std::vector<ShaderResource::PushConstantRange>& GetPushConstantRanges() const { return m_ReflectionData.PushConstantRanges; }

		struct ShaderMaterialDescriptorSet
		{
			VkDescriptorPool Pool = nullptr;
			std::vector<VkDescriptorSet> DescriptorSets;
		};

	private:
		RendererID m_RendererID;
		std::string m_FilePath;
		std::string m_VertexPath;
		std::string m_FragmentPath;
		std::unordered_map<std::string, int> m_UniformLocationCache;

		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;

		std::filesystem::path m_AssetPath;
		std::string m_Name;
		bool m_DisableOptimization = false;

		ReflectionData m_ReflectionData;

		std::map<VkShaderStageFlagBits, std::vector<uint32_t>> m_ShaderData;

		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkDescriptorSet m_DescriptorSet;
    };
}
