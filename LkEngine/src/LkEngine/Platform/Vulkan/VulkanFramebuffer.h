#pragma once

#include "LkEngine/Renderer/Framebuffer.h"
#include "LkVulkan.h"


namespace LkEngine {

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();

		void Invalidate() override;
		void RT_Invalidate();
		void Release();

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;
		virtual void AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func) override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual RendererID GetRendererID() const { return m_RendererID; }
		virtual RendererID GetColorAttachmentRendererID() const { return 0; }
		virtual RendererID GetDepthAttachmentRendererID() const { return 0; }
		virtual RendererID GetColorAttachmentRendererID(uint32_t index = 0) const { return 0; }
		
		virtual Ref<Image> GetImage(uint32_t attachmentIndex = 0) const override { LK_CORE_ASSERT(attachmentIndex < m_AttachmentImages.size()); return m_AttachmentImages[attachmentIndex]; }
		virtual Ref<Image> GetDepthImage() const override { return m_DepthAttachmentImage; }
		virtual size_t GetColorAttachmentCount() const override { return m_Specification.SwapChainTarget ? 1 : m_AttachmentImages.size(); }
		virtual bool HasDepthAttachment() const override { return (bool)m_DepthAttachmentImage; }

		VkRenderPass GetRenderPass() const { return m_RenderPass; }
		VkFramebuffer GetVulkanFramebuffer() const { return m_Framebuffer; }
		const std::vector<VkClearValue>& GetVulkanClearValues() const { return m_ClearValues; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override { return 0; }
		void ClearAttachment(uint32_t attachmentIndex, int value) override {}

		virtual void Bind() const override {}
		virtual void Unbind() const override {}
		virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const override {}

	private:
		FramebufferSpecification m_Specification;
		RendererID m_RendererID = 0;
		uint32_t m_Width = 0, m_Height = 0;

		std::vector<Ref<Image>> m_AttachmentImages;
		Ref<Image> m_DepthAttachmentImage;

		std::vector<VkClearValue> m_ClearValues;

		VkRenderPass m_RenderPass = nullptr;
		VkFramebuffer m_Framebuffer = nullptr;

		std::vector<std::function<void(Ref<Framebuffer>)>> m_ResizeCallbacks;
	};

#if 0
    class VulkanFramebuffer : public Framebuffer
    {
    public:
		VulkanFramebuffer(const FramebufferSpecification& framebufferSpecification);
        ~VulkanFramebuffer();
        
		void Bind() const;
		void Unbind() const;
		void Resize(uint32_t width, uint32_t height, bool forceRecreate = false);
		void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		s_ptr<Image> GetImage(uint32_t attachmentIndex = 0) const { return m_Image; }
		const FramebufferSpecification& GetSpecification() const { return m_Specification; }

	private:
		RendererID m_FramebufferID;
		RendererID m_RenderbufferID;
		RendererID m_ImageID;

		uint64_t m_Width, m_Height;
		glm::vec4 m_ClearColor;

		FramebufferSpecification m_Specification;

		s_ptr<Texture> m_Texture = nullptr;
		s_ptr<Image> m_Image = nullptr;
    };
#endif

}