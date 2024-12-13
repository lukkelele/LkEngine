#include "LKpch.h"
#include "OpenGLFramebuffer.h"

#include "LkEngine/Renderer/Backend/OpenGL/LkOpenGL.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Texture.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine {

	LOpenGLFramebuffer::LOpenGLFramebuffer(const FFramebufferSpecification& InSpecification)
		: m_Specification(InSpecification)
		, m_Width(InSpecification.Width)
		, m_Height(InSpecification.Height)
		, m_ClearColor(InSpecification.ClearColor)
		, m_DepthAttachmentSpecification(EImageFormat::DEPTH24STENCIL8)
	{
		LOBJECT_REGISTER();

		for (FFramebufferTextureSpecification& FramebufferTextureSpec : m_Specification.Attachments.Attachments)
		{
			LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Iterating framebuffer texture: {}", Enum::ToString(FramebufferTextureSpec.ImageFormat));
			if (!ImageUtils::IsDepthFormat(FramebufferTextureSpec.ImageFormat))
			{
				m_ColorAttachmentSpecifications.emplace_back(FramebufferTextureSpec);
			}
			else
			{
				m_DepthAttachmentSpecification = FramebufferTextureSpec;
			}
		}

		Invalidate();
	}

	LOpenGLFramebuffer::~LOpenGLFramebuffer()
	{
		LK_OpenGL_Verify(glDeleteFramebuffers(1, &RendererID));
		m_ColorAttachments.clear();

		LK_OpenGL_Verify(glDeleteTextures(1, &m_DepthAttachment));
	}

	void LOpenGLFramebuffer::Invalidate()
	{
		if (RendererID)
		{
			LK_OpenGL_Verify(glDeleteFramebuffers(1, &RendererID));
			m_ColorAttachments.clear();
			LK_OpenGL_Verify(glDeleteTextures(1, &m_DepthAttachment));
			
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		const bool bMultisample = m_Specification.Samples > 1;

		LK_OpenGL_Verify(glCreateFramebuffers(1, &RendererID));
		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, RendererID));

		/* Color Attachments. */
		if (m_ColorAttachmentSpecifications.size())
		{
			LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Creating {} color attachments {} multisampling", 
							  m_ColorAttachmentSpecifications.size(), bMultisample ? "with" : "without");

			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				FImageSpecification ImageSpec;
				ImageSpec.Width = m_Specification.Width;
				ImageSpec.Height = m_Specification.Height;
				ImageSpec.Format = m_ColorAttachmentSpecifications[i].ImageFormat;
				ImageSpec.Name = LK_FORMAT_STRING("Framebuffer_Image-{}", 
												  Enum::ToString(m_ColorAttachmentSpecifications[i].ImageFormat));
				ImageSpec.DebugName = LK_FORMAT_STRING("Framebuffer_Image-{}", 
													   Enum::ToString(m_ColorAttachmentSpecifications[i].ImageFormat));
				ImageSpec.Format = EImageFormat::RGBA32F;
				ImageSpec.Wrap = ETextureWrap::None;
				ImageSpec.Filter = ETextureFilter::None;
				ImageSpec.Mips = 1; // No mipmapping
				ImageSpec.Path = "Assets/Textures/white-texture.png"; /* FIXME: Do not use raw pathing like this. */

				FBuffer ImageData = FBuffer(LTextureLibrary::Get().GetWhiteTexture()->GetImageBuffer());
				LK_CORE_VERIFY(ImageData.Data, "Image data from white texture is NULL");
				TObjectPtr<LImage2D> Image = LImage2D::Create(ImageSpec, ImageData);
				m_ColorAttachments.push_back(Image);

				LK_OpenGL_Verify(glFramebufferTexture2D(GL_FRAMEBUFFER, 
					                             (GL_COLOR_ATTACHMENT0 + i), 
					                             GL_TEXTURE_2D, 
					                             m_ColorAttachments[0]->GetRendererID(), 0));
				LK_CORE_TRACE_TAG("Framebuffer", "Color Attachment ID {}", m_ColorAttachments[i]->GetRendererID());
			}
		}

		/* Depth Attachment. */
		if (m_DepthAttachmentSpecification.ImageFormat != EImageFormat::None)
		{
			LOpenGL::CreateTextures(bMultisample, &m_DepthAttachment, 1);
			LOpenGL::BindTexture(bMultisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.ImageFormat)
			{
				case EImageFormat::DEPTH24STENCIL8:
					LOpenGL::AttachDepthTexture(m_DepthAttachment, 
												m_Specification.Samples, 
												GL_DEPTH24_STENCIL8, 
												GL_DEPTH_STENCIL_ATTACHMENT, 
												m_Specification.Width, 
												m_Specification.Height);
					break;
			}

			LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Created depth texture with an image format of DEPTH24STENCIL8");
		}

		LK_CORE_VERIFY(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		LK_CORE_TRACE_TAG("OpenGLFramebuffer", "Created framebuffer with {} color attachments", m_ColorAttachments.size());

		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void LOpenGLFramebuffer::Clear()
	{
		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, RendererID));

		const glm::vec4& c = LRenderer::ClearColor;
        LK_OpenGL_Verify(glClearColor(c.r, c.g, c.b, c.a));
        LK_OpenGL_Verify(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	int LOpenGLFramebuffer::ReadPixel(uint32_t AttachmentIndex, int x, int y)
	{
		LK_OpenGL_Verify(glReadBuffer(GL_COLOR_ATTACHMENT0 + AttachmentIndex));
		int PixelData;
		LK_OpenGL_Verify(glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &PixelData));

		return PixelData;
	}

	void LOpenGLFramebuffer::ClearAttachment(uint32_t AttachmentIndex, int Value)
	{
		LK_CORE_ASSERT(AttachmentIndex < m_ColorAttachments.size(), "Attachment Index < m_ColorAttachments.size()");
		FFramebufferTextureSpecification& FramebufferTextureSpec = m_ColorAttachmentSpecifications[AttachmentIndex];
		LK_OpenGL_Verify(
			glClearTexImage(
				m_ColorAttachments[AttachmentIndex], 
				0, 
				LOpenGL::GetFramebufferTextureFormat(FramebufferTextureSpec.ImageFormat), 
				GL_INT, 
				&Value)
		);
	}

	uint32_t LOpenGLFramebuffer::GetColorAttachmentRendererID(uint32_t Index) const
	{
		LK_CORE_ASSERT(Index < m_ColorAttachments.size(), 
					   "Index is larger than the size of the color attachments array!");
		return m_ColorAttachments[Index];
	}

	void LOpenGLFramebuffer::Bind() const
	{
		LK_CORE_VERIFY(m_Specification.Width > 0 && m_Specification.Height > 0);
		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, RendererID));

		LK_OpenGL_Verify(glViewport(0, 0, m_Specification.Width, m_Specification.Height));
	}

	void LOpenGLFramebuffer::Unbind() const
	{
		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void LOpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		
		Invalidate();
	}

	void LOpenGLFramebuffer::BindTexture(uint32_t AttachmentIndex, uint32_t slot) const
	{
		LK_OpenGL_Verify(glBindTextureUnit(slot, m_ColorAttachments[AttachmentIndex]->GetRendererID()));
	}

	TObjectPtr<LImage> LOpenGLFramebuffer::GetImage(uint32_t AttachmentIndex) const
	{
		LK_CORE_VERIFY(AttachmentIndex < m_ColorAttachments.size());
		LK_CORE_VERIFY(m_ColorAttachments[AttachmentIndex], "Color attachment {} is nullptr!", AttachmentIndex);
		return m_ColorAttachments[AttachmentIndex];
	}
	
	TObjectPtr<LImage> LOpenGLFramebuffer::GetDepthImage() const
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		return nullptr;
	}

	size_t LOpenGLFramebuffer::GetColorAttachmentCount() const
	{
		return m_ColorAttachments.size();
	}

	void LOpenGLFramebuffer::AddResizeCallback(const FResizeCallback& InFunction)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLFramebuffer::TargetSwapChain()
	{
		LK_OpenGL_Verify(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

}
