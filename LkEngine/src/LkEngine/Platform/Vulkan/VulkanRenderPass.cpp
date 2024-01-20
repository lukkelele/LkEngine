#include "LKpch.h"
#include "VulkanRenderPass.h"


namespace LkEngine {

	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec)
		: m_Specification(spec)
	{
		LK_CORE_VERIFY(spec.Pipeline);

		VulkanDescriptorSetManagerSpecification dmSpec;
		dmSpec.DebugName = spec.DebugName;
		dmSpec.Shader = spec.Pipeline->GetSpecification().Shader.As<VulkanShader>();
		dmSpec.StartSet = 1;
		m_DescriptorSetManager = VulkanDescriptorSetManager(dmSpec);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
	}

	bool VulkanRenderPass::IsInvalidated(uint32_t set, uint32_t binding) const
	{
		return m_DescriptorSetManager.IsInvalidated(set, binding);
	}

	const std::vector<VkDescriptorSet>& VulkanRenderPass::GetDescriptorSets(uint32_t frameIndex) const
	{
		LK_CORE_ASSERT(!m_DescriptorSetManager.m_DescriptorSets.empty());
		if (frameIndex > 0 && m_DescriptorSetManager.m_DescriptorSets.size() == 1)
			return m_DescriptorSetManager.m_DescriptorSets[0]; // Frame index is irrelevant for this type of render pass
		return m_DescriptorSetManager.m_DescriptorSets[frameIndex];
	}

	bool VulkanRenderPass::IsInputValid(std::string_view name) const
	{
		std::string nameStr(name);
		return m_DescriptorSetManager.InputDeclarations.find(nameStr) != m_DescriptorSetManager.InputDeclarations.end();
	}

	void VulkanRenderPass::SetInput(std::string_view name, Ref<UniformBuffer> uniformBuffer)
	{
		//m_DescriptorSetManager.SetInput(name, uniformBuffer);
	}

#if 0
	void VulkanRenderPass::SetInput(std::string_view name, Ref<StorageBufferSet> storageBufferSet)
	{
		m_DescriptorSetManager.SetInput(name, storageBufferSet);
	}

	void VulkanRenderPass::SetInput(std::string_view name, Ref<StorageBuffer> storageBuffer)
	{
		m_DescriptorSetManager.SetInput(name, storageBuffer);
	}
#endif

	void VulkanRenderPass::SetInput(std::string_view name, Ref<Texture2D> texture)
	{
		//m_DescriptorSetManager.SetInput(name, texture);
	}

#if 0
	void VulkanRenderPass::SetInput(std::string_view name, Ref<TextureCube> textureCube)
	{
		m_DescriptorSetManager.SetInput(name, textureCube);
	}
#endif

	void VulkanRenderPass::SetInput(std::string_view name, Ref<Image> image)
	{
		//m_DescriptorSetManager.SetInput(name, image);
	}

	Ref<Image> VulkanRenderPass::GetOutput(uint32_t index)
	{
		Ref<Framebuffer> framebuffer = m_Specification.Pipeline->GetSpecification().TargetFramebuffer;
		if (index > framebuffer->GetColorAttachmentCount() + 1)
			return nullptr; // Invalid index
		if (index < framebuffer->GetColorAttachmentCount())
			return framebuffer->GetImage(index);
		return framebuffer->GetDepthImage();
	}

	Ref<Image> VulkanRenderPass::GetDepthOutput()
	{
		Ref<Framebuffer> framebuffer = m_Specification.Pipeline->GetSpecification().TargetFramebuffer;
		if (!framebuffer->HasDepthAttachment())
			return nullptr; // No depth output
		return framebuffer->GetDepthImage();
	}

	uint32_t VulkanRenderPass::GetFirstSetIndex() const
	{
		//return m_DescriptorSetManager.GetFirstSetIndex();
		return 0;
	}

	Ref<Framebuffer> VulkanRenderPass::GetTargetFramebuffer() const
	{
		return m_Specification.Pipeline->GetSpecification().TargetFramebuffer;
	}

	Ref<Pipeline> VulkanRenderPass::GetPipeline() const
	{
		return m_Specification.Pipeline;
	}

	bool VulkanRenderPass::Validate()
	{
		return m_DescriptorSetManager.Validate();
	}

	void VulkanRenderPass::Bake()
	{
		m_DescriptorSetManager.Bake();
	}

	void VulkanRenderPass::Prepare()
	{
		m_DescriptorSetManager.InvalidateAndUpdate();
	}

	bool VulkanRenderPass::HasDescriptorSets() const
	{
		return m_DescriptorSetManager.HasDescriptorSets();
	}

	const RenderPassInputDeclaration* VulkanRenderPass::GetInputDeclaration(std::string_view name) const
	{
		std::string nameStr(name);
		if (m_DescriptorSetManager.InputDeclarations.find(nameStr) == m_DescriptorSetManager.InputDeclarations.end())
			return nullptr;
		const RenderPassInputDeclaration& decl = m_DescriptorSetManager.InputDeclarations.at(nameStr);
		return &decl;
	}

}