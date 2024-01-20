#pragma once

#include "LkEngine/Renderer/Pipeline.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"

#include "LkVulkan.h"

#include "VulkanMemoryAllocator/vk_mem_alloc.h"


namespace LkEngine {

    class VulkanPipeline : public Pipeline
    {
    public:
		VulkanPipeline(const PipelineSpecification& specification);
		~VulkanPipeline();

		PipelineSpecification& GetSpecification() override { return m_Specification; }
		const PipelineSpecification& GetSpecification() const override { return m_Specification; }
		void Invalidate() override;
		Ref<Shader> GetShader() const override { return m_Specification.Shader; }

		bool IsDynamicLineWidth() const;

		void SetLayout(const VertexBufferLayout& layout);

		VkPipeline GetVulkanPipeline() { return m_VulkanPipeline; }
		VkPipelineLayout GetVulkanPipelineLayout() { return m_PipelineLayout; }

    private:
		PipelineSpecification m_Specification;

		VkPipeline m_VulkanPipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;
		VkPipelineCache m_PipelineCache = nullptr;
    };

}