#pragma once

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {
	
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
		OpenGLFramebuffer(const FramebufferSpecification& framebufferSpecification);
        ~OpenGLFramebuffer();
        
		void Invalidate() override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		void ClearAttachment(uint32_t attachmentIndex, int value) override;
		void Clear() override;
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;
		void AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func) override;
		Ref<Image> GetImage(uint32_t attachmentIndex = 0) const override;

		Ref<Image> GetDepthImage() const override;
		size_t GetColorAttachmentCount() const override;
		bool HasDepthAttachment() const override;

		void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const;
		void Bind() const override;
		void Unbind() const override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		uint64_t GetSize() const { return (m_Width * m_Height); }

		const FramebufferSpecification& GetSpecification() const override;

		RendererID& GetRendererID() { return m_RendererID; }

		static void TargetSwapChain();

	public:
		//static const uint32_t MaxFramebufferSize = 8192;
	private:
		RendererID m_RendererID;

		uint64_t m_Width;
		uint64_t m_Height;

		unsigned int m_TextureAttachment0;
		unsigned int m_TextureAttachment1;
		unsigned int m_DepthAttachment = 0;

		glm::vec4 m_ClearColor;
		std::vector<Ref<Image2D>> m_ColorAttachments;
		std::vector<uint32_t> m_ColorAttachmentRendererIDs;
		//std::vector<Ref<Image2D>> m_ColorAttachmentImages;

		FramebufferSpecification m_Specification;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;

		Ref<Image2D> m_Image;
    };


}