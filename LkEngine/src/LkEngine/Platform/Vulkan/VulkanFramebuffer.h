#pragma once

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {

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

}