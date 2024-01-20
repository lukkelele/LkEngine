#pragma once

#include "LkVulkan.h"
#include "VulkanDescriptorSetManager.h"

#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass();

		virtual RenderPassSpecification& GetSpecification() override { return m_Specification; }
		virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

		void SetInput(std::string_view name, Ref<Image> image) override;
		virtual void SetInput(std::string_view name, Ref<Texture2D> texture) override;
		virtual void SetInput(std::string_view name, Ref<UniformBuffer> uniformBuffer) override;

		//virtual void SetInput(std::string_view name, Ref<UniformBufferSet> uniformBufferSet) override;
		//virtual void SetInput(std::string_view name, Ref<StorageBufferSet> storageBufferSet) override;
		//virtual void SetInput(std::string_view name, Ref<StorageBuffer> storageBuffer) override;
		//virtual void SetInput(std::string_view name, Ref<TextureCube> textureCube) override;

		virtual Ref<Image> GetOutput(uint32_t index) override;
		virtual Ref<Image> GetDepthOutput() override;
		virtual uint32_t GetFirstSetIndex() const override;

		virtual Ref<Framebuffer> GetTargetFramebuffer() const override;
		virtual Ref<Pipeline> GetPipeline() const override;

		virtual bool Validate() override;
		virtual void Bake() override;
		virtual bool Baked() const override { return (bool)m_DescriptorSetManager.GetDescriptorPool(); }
		virtual void Prepare() override;

		bool HasDescriptorSets() const;
		const std::vector<VkDescriptorSet>& GetDescriptorSets(uint32_t frameIndex) const;

		bool IsInputValid(std::string_view name) const;
		const RenderPassInputDeclaration* GetInputDeclaration(std::string_view name) const;

	private:
		bool IsInvalidated(uint32_t set, uint32_t binding) const;
	private:
		RenderPassSpecification m_Specification;
		VulkanDescriptorSetManager m_DescriptorSetManager;

	};

}