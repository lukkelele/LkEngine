#include "LKpch.h"
#include "OpenGLFramebuffer.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"

#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& framebufferSpecification)
		: m_Specification(framebufferSpecification)
		, m_Width(framebufferSpecification.Width)
		, m_Height(framebufferSpecification.Height)
		, m_ClearColor(framebufferSpecification.ClearColor)
		, m_DepthAttachmentSpecification(ImageFormat::DEPTH24STENCIL8)
	{
		for (auto& spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.Format))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
			LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Iterating framebuffer texture specification: {}", Utils::ImageFormatToString(spec.Format));
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
			
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Creating {} color attachments {} multisampling", m_ColorAttachments.size(), multisample ? "with" : "without");
			GLUtils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				GLUtils::BindTexture(multisample, m_ColorAttachments[i]);
				//GLUtils::AttachColorTexture(
				//	m_ColorAttachments[i], 
				//	m_Specification.Samples,
				//	GLUtils::ImageFormatToGLInternalFormat(m_ColorAttachmentSpecifications[i].Format),
				//	GLUtils::ImageFormatToGLDataFormat(m_ColorAttachmentSpecifications[i].Format), 
				//	m_Specification.Width, 
				//	m_Specification.Height,
				//	i);

				switch (m_ColorAttachmentSpecifications[i].Format)
				{
					case ImageFormat::RGBA8:
						GLUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case ImageFormat::RGBA16F:
						GLUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GLUtils::ImageFormatToGLInternalFormat(m_ColorAttachmentSpecifications[i].Format), GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case ImageFormat::RGBA32F:
						GLUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA32F, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case ImageFormat::RED32UI:
						GLUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						break;
					case ImageFormat::RED32F:
						GLUtils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32F, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						break;
				}
				LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created color attachment {} with format {}", i, Utils::ImageFormatToString(m_ColorAttachmentSpecifications[i].Format));
			}
		}

		if (m_DepthAttachmentSpecification.Format != ImageFormat::None)
		{
			GLUtils::CreateTextures(multisample, &m_DepthAttachment, 1);
			GLUtils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.Format)
			{
				case ImageFormat::DEPTH24STENCIL8:
					GLUtils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
			}
			LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created depth texture with an image format of DEPTH24STENCIL8");
		}

		if (m_ColorAttachments.size() > 1)
		{
			LK_CORE_ASSERT(m_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
			LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "No color attachments, only depth pass...");
		}

		LK_CORE_VERIFY(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created framebuffer with {} color attachments", m_ColorAttachments.size());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		LK_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Attachment index < m_ColorAttachments.size()");
		FramebufferTextureSpecification& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GLUtils::FramebufferTextureFormatToGL(spec.Format), GL_INT, &value);
	}

	uint32_t OpenGLFramebuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		LK_CORE_ASSERT(index < m_ColorAttachments.size(), "Index is larger than the size of the color attachments array!");
		return m_ColorAttachments[index];
	}

	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
		{
			LK_CORE_WARN_TAG("OpenGLFramebuffer", "Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		
		Invalidate();
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[attachmentIndex]));
	}

	uint32_t OpenGLFramebuffer::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OpenGLFramebuffer::GetHeight() const
	{
		return m_Height;
	}

	Ref<Image> OpenGLFramebuffer::GetImage(uint32_t attachmentIndex) const
	{
		return m_Image;
	}
	
	const FramebufferSpecification& OpenGLFramebuffer::GetSpecification() const
	{
		return m_Specification;
	}

	Ref<Image> OpenGLFramebuffer::GetDepthImage() const
	{
		return nullptr;
	}

	size_t OpenGLFramebuffer::GetColorAttachmentCount() const
	{
		return m_ColorAttachments.size();
	}

	bool OpenGLFramebuffer::HasDepthAttachment() const
	{
		return m_DepthAttachment >= 0;
	}

	void OpenGLFramebuffer::AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func)
	{
	}

}