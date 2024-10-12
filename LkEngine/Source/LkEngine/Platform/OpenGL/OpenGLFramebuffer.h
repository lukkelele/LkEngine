#pragma once

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {
	
    class LOpenGLFramebuffer : public LFramebuffer
    {
    public:
		LOpenGLFramebuffer(const FramebufferSpecification& framebufferSpecification);
        ~LOpenGLFramebuffer();
        
		virtual void Invalidate() override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
		virtual void Clear() override;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;
		virtual void AddResizeCallback(const FResizeCallback& InFunction) override;
		virtual TObjectPtr<LImage> GetImage(uint32_t attachmentIndex = 0) const override;

		virtual TObjectPtr<LImage> GetDepthImage() const override;
		virtual size_t GetColorAttachmentCount() const override;
		virtual bool HasDepthAttachment() const override;

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

		uint64_t m_Width{};
		uint64_t m_Height{};

		unsigned int m_TextureAttachment0;
		unsigned int m_TextureAttachment1;
		unsigned int m_DepthAttachment = 0;

		glm::vec4 m_ClearColor;
		std::vector<TObjectPtr<LImage2D>> m_ColorAttachments;
		std::vector<uint32_t> m_ColorAttachmentRendererIDs;

		FramebufferSpecification m_Specification;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
    };


}