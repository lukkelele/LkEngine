#pragma once

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {
	
    class LOpenGLFramebuffer : public LFramebuffer
    {
    public:
		LOpenGLFramebuffer(const FFramebufferSpecification& InSpecification);
		LOpenGLFramebuffer() = delete;
        ~LOpenGLFramebuffer();
        
		virtual void Invalidate() override;
		virtual int ReadPixel(uint32_t AttachmentIndex, int PosX, int PosY) override;
		virtual void ClearAttachment(uint32_t AttachmentIndex, int value) override;
		virtual void Clear() override;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t Index = 0) const override;

		virtual void Resize(const uint32_t NewWidth, const uint32_t NewHeight, bool bForceRecreate = false) override;
		virtual void AddResizeCallback(const FResizeCallback& InFunction) override;
		virtual TObjectPtr<LImage> GetImage(uint32_t AttachmentIndex = 0) const override;

		virtual TObjectPtr<LImage> GetDepthImage() const override;
		virtual size_t GetColorAttachmentCount() const override;

		FORCEINLINE virtual bool HasDepthAttachment() const override 
		{ 
			return (m_DepthAttachment >= 0); 
		}

		virtual void BindTexture(const uint32_t AttachmentIndex = 0, const uint32_t Slot = 0) const override;
		virtual void Bind() const override;
		virtual void Unbind() const override;

		FORCEINLINE const FFramebufferSpecification& GetSpecification() const override 
		{ 
			return m_Specification; 
		}

		FORCEINLINE virtual uint32_t GetWidth() const override { return m_Width; }
		FORCEINLINE virtual uint32_t GetHeight() const override { return m_Height; }
		FORCEINLINE virtual uint64_t GetSize() const override { return (m_Width * m_Height); }

		FORCEINLINE virtual LRendererID GetRendererID() const override { return m_RendererID; }
		FORCEINLINE virtual LRendererID& GetRendererID() override { return m_RendererID; }

		static void TargetSwapChain();

	private:
		LRendererID m_RendererID;

		uint64_t m_Width{};
		uint64_t m_Height{};

		/* FIXME: */
		unsigned int m_TextureAttachment0 = -1;
		unsigned int m_TextureAttachment1 = -1;
		unsigned int m_DepthAttachment = 0;

		glm::vec4 m_ClearColor{};
		std::vector<TObjectPtr<LImage2D>> m_ColorAttachments{};
		std::vector<uint32_t> m_ColorAttachmentRendererIDs{};

		FFramebufferSpecification m_Specification{};
		FFramebufferTextureSpecification m_DepthAttachmentSpecification{};
		std::vector<FFramebufferTextureSpecification> m_ColorAttachmentSpecifications{};
    };


}