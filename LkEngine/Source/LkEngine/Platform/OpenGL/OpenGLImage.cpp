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
			LK_OpenGL(glGenTextures(1, &m_RendererID));
			LK_OpenGL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			LK_OpenGL(
				glTexImage2D(GL_TEXTURE_2D, 
						     0, 
						     GLUtils::OpenGLImageInternalFormat(Specification.Format), 
						     Specification.Width, 
						     Specification.Height, 
						     0, 
						     GLUtils::OpenGLImageFormat(Specification.Format), 
						     GL_UNSIGNED_BYTE, 
						     (const void*)m_ImageData.Data)
			);

			if (Specification.Mips > 1)
			{
				LK_OpenGL(glGenerateTextureMipmap(m_RendererID));
			}

			GLUtils::ApplyTextureWrap(Specification.Wrap);
			GLUtils::ApplyTextureFilter(Specification.Filter, Specification.Mips > 1);

			LK_OpenGL(glBindTexture(GL_TEXTURE_2D, 0));
		}
    }

    LOpenGLImage::LOpenGLImage(const FImageSpecification& InSpecification, void* InData)
        : Specification(InSpecification)
	{
		LCLASS_REGISTER();

		if (InData)
		{
			m_ImageData = FBuffer::Copy(InData, Specification.Size);

			LK_OpenGL(glGenTextures(1, &m_RendererID));
			LK_OpenGL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			glTexImage2D(GL_TEXTURE_2D, 
						 0, 
						 GLUtils::OpenGLImageInternalFormat(Specification.Format), 
						 Specification.Width, 
						 Specification.Height, 
						 0, 
						 GLUtils::OpenGLImageFormat(Specification.Format), 
						 GL_UNSIGNED_BYTE, 
						 (const void*)m_ImageData.Data);

			if (Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}

			GLUtils::ApplyTextureWrap(Specification.Wrap);
			GLUtils::ApplyTextureFilter(Specification.Filter, Specification.Mips > 1);

			/* Anistropic Filtering */
			GLfloat MaxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &MaxAnisotropy);

			glBindTexture(GL_TEXTURE_2D, 0);

			/* Clean up resources. */
			stbi_image_free(InData);
		}
    }

    LOpenGLImage::~LOpenGLImage()
    {
		//LK_OpenGL(glMakeTextureHandleNonResidentARB(m_HandleARB));
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
		FBuffer ImageData = FBuffer::Copy(m_ImageData);
		if (m_RendererID)
		{
            Release();
		}

		m_ImageData = ImageData;
        GLenum InternalFormat = GLUtils::OpenGLImageInternalFormat(Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 
						   ImageUtils::CalculateMipCount(Specification.Width, Specification.Height), 
						   GLUtils::OpenGLImageInternalFormat(Specification.Format), 
						   Specification.Width, 
						   Specification.Height);

        if (m_ImageData)
        {
			glTextureSubImage2D(m_RendererID, 
								0, 
								0, 
								0, 
								Specification.Width, 
								Specification.Height, 
								GLUtils::OpenGLImageFormat(Specification.Format), 
								GLUtils::OpenGLFormatDataType(Specification.Format), 
								(const void*)m_ImageData.Data);

			if (Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}
        }
    }

	void LOpenGLImage::RT_Invalidate()
	{
	}

    void LOpenGLImage::AllocateMemory(uint64_t size)
    {
    }

    void LOpenGLImage::Release()
    {
        if (m_RendererID)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }

        m_ImageData.Release();
    }


    LOpenGLImage2D::LOpenGLImage2D(const FImageSpecification& InSpecification, FBuffer&& InBuffer)
		: Specification(InSpecification)
		, m_ImageData(std::move(InBuffer))
    {
		LCLASS_REGISTER();

		if (m_ImageData)
		{
			LK_OpenGL(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
			LK_CORE_ASSERT(m_RendererID != 0, "RendererID is 0");

			LK_OpenGL(glTextureStorage2D(m_RendererID,
					  Specification.Mips,
					  GLUtils::OpenGLImageInternalFormat(Specification.Format),
					  Specification.Width,
					  Specification.Height));

			LK_CORE_VERIFY(m_ImageData.Data, "Image data is nullptr");
			LK_OpenGL(glTextureSubImage2D(m_RendererID,
					  0,
					  0,
					  0,
					  Specification.Width,
					  Specification.Height,
					  GLUtils::OpenGLImageFormat(Specification.Format),
					  GL_UNSIGNED_BYTE,
					  (const void*)m_ImageData.Data));

			if (Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}

			GLUtils::ApplyTextureWrap(m_RendererID, Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_RendererID, 
										Specification.Filter, 
										Specification.Mips > 1);
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

			LK_OpenGL(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
			LK_OpenGL(glTextureStorage2D(m_RendererID, 
					  ImageUtils::CalculateMipCount(Specification.Width, Specification.Height), 
					  GLUtils::OpenGLImageInternalFormat(Specification.Format), 
					  Specification.Width, 
					  Specification.Height));

			LK_OpenGL(glTextureSubImage2D(
				m_RendererID, 
				0, 
				0, 
				0, 
				InSpecification.Width, 
				InSpecification.Height, 
				GLUtils::OpenGLImageFormat(InSpecification.Format), 
				GL_UNSIGNED_BYTE, 
				(const void*)m_ImageData.Data)
			);

			if (Specification.Mips > 1)
			{
				LK_OpenGL(glGenerateTextureMipmap(m_RendererID));
			}

			GLUtils::ApplyTextureWrap(m_RendererID, Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_RendererID,
										Specification.Filter,
										(Specification.Mips > 1));

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

        const GLenum InternalFormat = GLUtils::OpenGLImageInternalFormat(Specification.Format);
        const uint32_t MipCount = ImageUtils::CalculateMipCount(Specification.Width, Specification.Height);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 
						   MipCount, 
						   InternalFormat, 
						   Specification.Width, 
						   Specification.Height);
        if (m_ImageData)
        {
			LK_OpenGL(glTextureSubImage2D(m_RendererID, 
					  0, 
					  0, 
					  0, 
					  Specification.Width, 
					  Specification.Height, 
					  GLUtils::OpenGLImageFormat(Specification.Format), 
					  GLUtils::OpenGLFormatDataType(Specification.Format),
					  (const void*)m_ImageData.Data)); 

			if (Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}
        }
    }

	void LOpenGLImage2D::RT_Invalidate()
	{
	}

    void LOpenGLImage2D::AllocateMemory(uint64_t size)
    {
    }

    void LOpenGLImage2D::Release()
    {
        if (m_RendererID)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }

        m_ImageData.Release();
    }



}