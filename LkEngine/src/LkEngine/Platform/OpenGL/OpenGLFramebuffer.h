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
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;
		void AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func) override;
		virtual Ref<Image> GetImage(uint32_t attachmentIndex = 0) const override;

		Ref<Image> GetDepthImage() const override;
		size_t GetColorAttachmentCount() const override;
		bool HasDepthAttachment() const override;

		void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const;
		void Bind() const override;
		void Unbind() const override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		const FramebufferSpecification& GetSpecification() const override;

	public:
		static const uint32_t MaxFramebufferSize = 8192;
	private:
		RendererID m_RendererID;
		RendererID m_RenderbufferID;
		RendererID m_ImageID;

		uint64_t m_Width, m_Height;
		glm::vec4 m_ClearColor;

		FramebufferSpecification m_Specification;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;

		Ref<Image> m_Image;
		Ref<Texture> m_Texture;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
    };

}