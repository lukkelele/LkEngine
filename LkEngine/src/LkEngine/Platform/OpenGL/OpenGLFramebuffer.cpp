#include "LKpch.h"
#include "OpenGLFramebuffer.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"

#include "LkEngine/Renderer/Texture.h"

//#include <imgui/imgui_impl_opengl3.h>

namespace LkEngine {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& framebufferSpecification)
		: m_Specification(framebufferSpecification)
		, m_Width(framebufferSpecification.Width)
		, m_Height(framebufferSpecification.Height)
	{
		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		TextureSpecification textureSpecification;
		textureSpecification.Width = framebufferSpecification.Width;
		textureSpecification.Height = framebufferSpecification.Height;

		// Create texture to hold color buffer
		m_Texture = Texture::Create(textureSpecification);
		m_Image = m_Texture->GetImage();
#if 0
		glGenTextures(1, &m_Image->GetRendererID());
		glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Specification.Width, m_Specification.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
		// Attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetRendererID(), 0);

		// Create renderbuffer object for depth and stencil attachment
		glGenRenderbuffers(1, &m_RenderbufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Attach renderbuffer to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_ERROR("ERROR: OpenGLFramebuffer is not complete!");
			throw std::runtime_error("OpenGLFramebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		GL_CALL(glDeleteFramebuffers(1, &m_FramebufferID));
		GL_CALL(glDeleteTextures(1, &m_Texture->GetRendererID()));
		GL_CALL(glDeleteRenderbuffers(1, &m_RenderbufferID));
	}

	void OpenGLFramebuffer::Bind() const
	{
		GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID));
		GL_CALL(glViewport(0, 0, m_Width, m_Height));
	}

	void OpenGLFramebuffer::Unbind() const
	{
		GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
	}

	uint32_t OpenGLFramebuffer::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OpenGLFramebuffer::GetHeight() const
	{
		return m_Height;
	}

	s_ptr<Image> OpenGLFramebuffer::GetImage(uint32_t attachmentIndex) const
	{
		return m_Image;
	}
	
	const FramebufferSpecification& OpenGLFramebuffer::GetSpecification() const
	{
		return m_Specification;
	}

}