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
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		m_ColorAttachments.clear();
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			m_ColorAttachments.clear();
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
			//m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Creating {} color attachments {} multisampling", m_ColorAttachments.size(), multisample ? "with" : "without");

			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				ImageSpecification imageSpec;
				imageSpec.Width = m_Specification.Width;
				imageSpec.Height = m_Specification.Height;
				imageSpec.Format = m_ColorAttachmentSpecifications[i].Format;
				imageSpec.Name = "Framebuffer-Image-" + Utils::ImageFormatToString(m_ColorAttachmentSpecifications[i].Format);
				imageSpec.DebugName = "Framebuffer-Image-" + Utils::ImageFormatToString(m_ColorAttachmentSpecifications[i].Format);
				imageSpec.Format = ImageFormat::RGBA32F;
				imageSpec.Wrap = TextureWrap::None;
				imageSpec.Filter = TextureFilter::None;
				imageSpec.Mips = 1; // No mipmapping
				imageSpec.Path = "Assets/textures/white-texture.png";

				Buffer imageData = Buffer(TextureLibrary::Get()->GetWhiteTexture2D()->GetImageBuffer());
				Ref<Image2D> image = Image2D::Create(imageSpec, imageData);
				m_ColorAttachments.push_back(image);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[0]->GetRendererID(), 0);
				LK_CORE_WARN_TAG("Framebuffer", "Color Attachment ID {}", m_ColorAttachments[i]->GetRendererID());
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Clear()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		//BindTexture(0);

		auto& c = Renderer::ClearColor;
        glClearColor(c.r, c.g, c.b, c.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		
		Invalidate();
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		//glActiveTexture(GL_TEXTURE0 + slot);
		//glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[attachmentIndex]->GetRendererID());
		glBindTextureUnit(slot, m_ColorAttachments[attachmentIndex]->GetRendererID());
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
		LK_CORE_VERIFY(attachmentIndex < m_ColorAttachments.size());
		LK_CORE_VERIFY(m_ColorAttachments[attachmentIndex], "Color attachment {} is nullptr!", attachmentIndex);
		return m_ColorAttachments[attachmentIndex];
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

	void OpenGLFramebuffer::TargetSwapChain()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}
