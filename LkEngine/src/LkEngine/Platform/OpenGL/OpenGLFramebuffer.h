#pragma once

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {
	
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
		OpenGLFramebuffer(const FramebufferSpecification& framebufferSpecification);
        ~OpenGLFramebuffer();
        
		void Bind() const;
		void Unbind() const;
		void Resize(uint32_t width, uint32_t height, bool forceRecreate = false);
		void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		s_ptr<Image> GetImage(uint32_t attachmentIndex = 0) const;
		const FramebufferSpecification& GetSpecification() const;

	private:
		RendererID m_FramebufferID;
		RendererID m_RenderbufferID;
		uint64_t m_Width, m_Height;

		FramebufferSpecification m_Specification;

		s_ptr<Texture> m_Texture = nullptr;
		s_ptr<Image> m_Image = nullptr;
    };

}