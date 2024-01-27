#include "LKpch.h"
#include "OpenGLFramebuffer.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Texture.h"

#include "LkEngine/Core/Window.h"


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
			{
				m_ColorAttachmentSpecifications.emplace_back(spec);
			}
			else
			{
				m_DepthAttachmentSpecification = spec;
			}
			LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Iterating framebuffer texture specification: {}", Utils::ImageFormatToString(spec.Format));
		}
		Invalidate();

#if 0
        glGenFramebuffers(1, &m_RendererID);
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Generated renderer ID for framebuffer: {}", m_RendererID);

        // Create a color attachment texture
		m_ColorAttachments.resize(m_ColorAttachments.size() + 1); // Add slot
        glGenTextures(1, &m_ColorAttachments[0]);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::Get().GetWidth(), Window::Get().GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachments[0], 0);

        // Create depth texture
        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Window::Get().GetWidth(), Window::Get().GetHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
		LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created depth attachment {}", m_DepthAttachment);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LK_CORE_ERROR_TAG("OpenGLRenderer2D", "Framebuffer is not complete!");
            exit(EXIT_FAILURE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		//LK_CORE_ASSERT(false, "Disabled for now");
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
			
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		bool multisample = m_Specification.Samples > 1;

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Creating {} color attachments {} multisampling", m_ColorAttachments.size(), multisample ? "with" : "without");
			GLUtils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				ImageSpecification imageSpec;
				imageSpec.Width = m_Specification.Width;
				imageSpec.Height = m_Specification.Height;
				imageSpec.Format = m_ColorAttachmentSpecifications[i].Format;
				imageSpec.DebugName = "Framebuffer-Image-" + Utils::ImageFormatToString(m_ColorAttachmentSpecifications[i].Format);
				Ref<Image2D> image = Image2D::Create(imageSpec);
				m_ColorAttachmentImages.push_back(image);

				//GLUtils::BindTexture(multisample, m_ColorAttachments[i]);
				//GLUtils::AttachColorTexture(
				//	m_ColorAttachments[i],
				//	m_Specification.Samples, 
				//	GLUtils::ImageFormatToGLInternalFormat(m_ColorAttachmentSpecifications[i].Format), 
				//	GLUtils::ImageFormatToGLDataFormat(m_ColorAttachmentSpecifications[i].Format),     
				//	m_Specification.Width, 
				//	m_Specification.Height, 
				//	i
				//);
				LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created color attachment {} with format {}", i, Utils::ImageFormatToString(m_ColorAttachmentSpecifications[i].Format));
			}
		}

		// Depth Attachement
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

		LK_CORE_VERIFY(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created framebuffer with {} color attachments", m_ColorAttachments.size());
		//LK_CORE_DEBUG_TAG("OpenGLFramebuffer", "Created framebuffer with {} color attachments", m_ColorAttachments.size());

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
		LK_CORE_VERIFY(m_Specification.Width > 0 && m_Specification.Height > 0);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glEnable(GL_DEPTH_TEST); 

		auto& c = Renderer::ClearColor;
        glClearColor(c.r, c.g, c.b, c.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		//GL_CALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[attachmentIndex]));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentImages[attachmentIndex]->GetRendererID()));
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
		//return m_Image;
		LK_CORE_VERIFY(attachmentIndex < m_ColorAttachmentImages.size(), "Attachment Index {} is too large, only {} images in vector", attachmentIndex, m_ColorAttachmentImages.size());
		return m_ColorAttachmentImages[attachmentIndex];
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