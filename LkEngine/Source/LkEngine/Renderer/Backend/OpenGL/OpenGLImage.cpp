#include "LKpch.h"
#include "OpenGLImage.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLImage::LOpenGLImage(const FImageSpecification& InSpecification, FBuffer&& InBuffer)
		: Specification(InSpecification)
		, m_ImageData(std::move(InBuffer))
	{
		LCLASS_REGISTER();

		if (m_ImageData.Data)
		{
			LK_OpenGL_Verify(glGenTextures(1, &m_RendererID));
			LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			LK_OpenGL_Verify(
				glTexImage2D(GL_TEXTURE_2D,
							 0,
							 LOpenGL::GetImageInternalFormat(Specification.Format),
							 Specification.Width,
							 Specification.Height,
							 0,
							 LOpenGL::GetImageFormat(Specification.Format),
							 GL_UNSIGNED_BYTE,
							 (const void*)m_ImageData.Data)
			);

			if (Specification.Mips > 1)
			{
				LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			}

			LOpenGL::ApplyTextureWrap(Specification.Wrap);
			LOpenGL::ApplyTextureFilter(Specification.Filter, Specification.Mips > 1);

			LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_2D, 0));
		}
	}

	LOpenGLImage::LOpenGLImage(const FImageSpecification& InSpecification, void* InData)
		: Specification(InSpecification)
	{
		LCLASS_REGISTER();

		if (InData)
		{
			m_ImageData = FBuffer::Copy(InData, Specification.Size);

			LK_OpenGL_Verify(glGenTextures(1, &m_RendererID));
			LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			LK_OpenGL_Verify(
				glTexImage2D(GL_TEXTURE_2D,
							 0,
							 LOpenGL::GetImageInternalFormat(Specification.Format),
							 Specification.Width,
							 Specification.Height,
							 0,
							 LOpenGL::GetImageFormat(Specification.Format),
							 GL_UNSIGNED_BYTE,
							 (const void*)m_ImageData.Data));

			if (Specification.Mips > 1)
			{
				LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			}

			LOpenGL::ApplyTextureWrap(Specification.Wrap);
			LOpenGL::ApplyTextureFilter(Specification.Filter, Specification.Mips > 1);

			/* Anistropic Filtering */
			GLfloat MaxAnisotropy;
			LK_OpenGL_Verify(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &MaxAnisotropy));
			LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_2D, 0));

			/* Clean up resources. */
			stbi_image_free(InData);
		}
	}

	LOpenGLImage::~LOpenGLImage()
	{
#if 0
		LK_OpenGL(glMakeTextureHandleNonResidentARB(m_HandleARB));
#endif
	}

	void LOpenGLImage::SetData(const void* InData)
	{
		if (InData)
		{
			m_ImageData.Release();
			m_ImageData.Data = (void*)InData;
		}
	}

	void LOpenGLImage::Resize(const uint32_t NewWidth, const uint32_t NewHeight)
	{
		LK_UNUSED(NewWidth && NewHeight);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLImage::Invalidate()
	{
		FBuffer ImageDataCopy = FBuffer::Copy(m_ImageData);
		if (m_RendererID)
		{
			Release();
		}

		m_ImageData = ImageDataCopy;
		GLenum InternalFormat = LOpenGL::GetImageInternalFormat(Specification.Format);

		LK_OpenGL_Verify(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
		LK_OpenGL_Verify(glTextureStorage2D(m_RendererID,
											ImageUtils::CalculateMipCount(Specification.Width, Specification.Height),
											LOpenGL::GetImageInternalFormat(Specification.Format),
											Specification.Width,
											Specification.Height));

		if (m_ImageData)
		{
			LK_OpenGL_Verify(glTextureSubImage2D(m_RendererID,
												 0,
												 0,
												 0,
												 Specification.Width,
												 Specification.Height,
												 LOpenGL::GetImageFormat(Specification.Format),
												 LOpenGL::GetFormatDataType(Specification.Format),
												 (const void*)m_ImageData.Data));

			if (Specification.Mips > 1)
			{
				LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			}
		}
	}

	void LOpenGLImage::RT_Invalidate()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLImage::AllocateMemory(uint64_t size)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLImage::Release()
	{
		if (m_RendererID)
		{
			LK_OpenGL_Verify(glDeleteTextures(1, &m_RendererID));
			m_RendererID = 0;
		}

		m_ImageData.Release();
	}

	/********************************************************************************
	 ********************************************************************************/

	LOpenGLImage2D::LOpenGLImage2D(const FImageSpecification& InSpecification, FBuffer&& InBuffer)
		: Specification(InSpecification)
		, m_ImageData(std::move(InBuffer))
	{
		LCLASS_REGISTER();

		if (m_ImageData)
		{
			LK_OpenGL_Verify(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
			LK_CORE_ASSERT(m_RendererID != 0, "RendererID is 0");

			LK_OpenGL_Verify(glTextureStorage2D(m_RendererID,
												Specification.Mips,
												LOpenGL::GetImageInternalFormat(Specification.Format),
												Specification.Width,
												Specification.Height));

			LK_CORE_VERIFY(m_ImageData.Data, "Image data is nullptr");
			LK_OpenGL_Verify(glTextureSubImage2D(m_RendererID,
												 0,
												 0,
												 0,
												 Specification.Width,
												 Specification.Height,
												 LOpenGL::GetImageFormat(Specification.Format),
												 GL_UNSIGNED_BYTE,
												 (const void*)m_ImageData.Data));

			if (Specification.Mips > 1)
			{
				LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			}

			LOpenGL::ApplyTextureWrap(m_RendererID, Specification.Wrap);
			LOpenGL::ApplyTextureFilter(m_RendererID, Specification.Filter, (Specification.Mips > 1));
		}
	}

	LOpenGLImage2D::LOpenGLImage2D(const FImageSpecification& InSpecification, void* InData)
		: Specification(InSpecification)
	{
		LCLASS_REGISTER();

		if (InData)
		{
			const uint64_t ImageSize = ImageUtils::GetMemorySize(Specification.Format, Specification.Width, Specification.Height);
			m_ImageData = FBuffer(InData, ImageSize);

			LK_OpenGL_Verify(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
			LK_OpenGL_Verify(glTextureStorage2D(m_RendererID,
												ImageUtils::CalculateMipCount(Specification.Width, Specification.Height),
												LOpenGL::GetImageInternalFormat(Specification.Format),
												Specification.Width,
												Specification.Height));

			LK_OpenGL_Verify(
				glTextureSubImage2D(m_RendererID,
									0,
									0,
									0,
									InSpecification.Width,
									InSpecification.Height,
									LOpenGL::GetImageFormat(InSpecification.Format),
									GL_UNSIGNED_BYTE,
									(const void*)m_ImageData.Data)
			);

			if (Specification.Mips > 1)
			{
				LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			}

			LOpenGL::ApplyTextureWrap(m_RendererID, Specification.Wrap);
			LOpenGL::ApplyTextureFilter(m_RendererID, Specification.Filter, (Specification.Mips > 1));

			/* Clean up image data. */
			stbi_image_free(InData);
		}
	}

	LOpenGLImage2D::~LOpenGLImage2D()
	{
		Release();
	}

	void LOpenGLImage2D::SetData(const void* InData)
	{
		if (InData)
		{
			m_ImageData.Release();
			m_ImageData.Data = (void*)InData;
		}
	}

	void LOpenGLImage2D::Resize(const uint32_t NewWidth, const uint32_t NewHeight)
	{
		LK_CORE_DEBUG_TAG("LOpenGLImage", "Resize ({}, {})", NewWidth, NewHeight);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLImage2D::Invalidate()
	{
		FBuffer ImageDataCopy = m_ImageData;
		if (m_RendererID)
		{
			Release();
		}

		m_ImageData = ImageDataCopy;

		const GLenum InternalFormat = LOpenGL::GetImageInternalFormat(Specification.Format);
		const uint32_t MipCount = ImageUtils::CalculateMipCount(Specification.Width, Specification.Height);

		LK_OpenGL_Verify(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
		LK_OpenGL_Verify(glTextureStorage2D(m_RendererID,
											MipCount,
											InternalFormat,
											Specification.Width,
											Specification.Height));
		if (m_ImageData)
		{
			LK_OpenGL_Verify(glTextureSubImage2D(m_RendererID,
												 0,
												 0,
												 0,
												 Specification.Width,
												 Specification.Height,
												 LOpenGL::GetImageFormat(Specification.Format),
												 LOpenGL::GetFormatDataType(Specification.Format),
												 (const void*)m_ImageData.Data));

			if (Specification.Mips > 1)
			{
				LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			}
		}
	}

	void LOpenGLImage2D::RT_Invalidate()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLImage2D::AllocateMemory(const uint64_t NewSize)
	{
		LK_UNUSED(NewSize);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LOpenGLImage2D::Release()
	{
		if (m_RendererID)
		{
			LK_OpenGL_Verify(glDeleteTextures(1, &m_RendererID));
			m_RendererID = 0;
		}

		m_ImageData.Release();
	}


}
