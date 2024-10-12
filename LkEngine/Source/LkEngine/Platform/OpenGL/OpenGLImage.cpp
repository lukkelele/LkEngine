#include "LKpch.h"
#include "OpenGLImage.h"

#include "LkOpenGL.h"


namespace LkEngine {

    LOpenGLImage::LOpenGLImage(const ImageSpecification& InImageSpecification, FBuffer&& InBuffer)
		: m_Specification(InImageSpecification)
		, m_ImageData(std::move(InBuffer))
    {
		if (m_ImageData.Data)
		{
		#if 0
            const uint64_t ImageSize = Utils::GetMemorySize(InImageSpecification.Format, 
															InImageSpecification.Width, 
															InImageSpecification.Height);
		#endif

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexImage2D(GL_TEXTURE_2D, 
						 0, 
						 GLUtils::OpenGLImageInternalFormat(InImageSpecification.Format), 
						 InImageSpecification.Width, 
						 InImageSpecification.Height, 
						 0, 
						 GLUtils::OpenGLImageFormat(InImageSpecification.Format), 
						 GL_UNSIGNED_BYTE, 
						 (const void*)m_ImageData.Data);

			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}

			GLUtils::ApplyTextureWrap(m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_Specification.Filter, m_Specification.Mips > 1);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	#if 0 /* FIXME: Remove */
		else
		{
			// Data is null, try to load it from path
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			m_ImageData = FBuffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));

			stbi_image_free(data);
		}
	#endif
    }

    LOpenGLImage::LOpenGLImage(const ImageSpecification& InImageSpecification, void* InData)
        : m_Specification(InImageSpecification)
	{
		if (InData)
		{
			m_ImageData = FBuffer::Copy(InData, m_Specification.Size);

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexImage2D(GL_TEXTURE_2D, 
						 0, 
						 GLUtils::OpenGLImageInternalFormat(InImageSpecification.Format), 
						 InImageSpecification.Width, 
						 InImageSpecification.Height, 
						 0, 
						 GLUtils::OpenGLImageFormat(InImageSpecification.Format), 
						 GL_UNSIGNED_BYTE, 
						 (const void*)m_ImageData.Data);

			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}

			GLUtils::ApplyTextureWrap(m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_Specification.Filter, m_Specification.Mips > 1);

			// Anistropic Filtering
			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

			glBindTexture(GL_TEXTURE_2D, 0);

			/* Clean up resources. */
			stbi_image_free(InData);
		}
	#if 0 /* FIXME: Remove */
		else
		{
			/* TODO: I don't really like this approach. Should prob just crash here. */
			/* Data is null, try to load it from path. */

			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);

			void* LoadedImageData = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			m_ImageData = FBuffer(LoadedImageData, Utils::GetMemorySize(m_Specification.Format, width, height));

			/* Clean up resources. */
			stbi_image_free(LoadedImageData);

			Invalidate();
		}
	#endif
    }

    LOpenGLImage::~LOpenGLImage()
    {
		//LK_OpenGL(glMakeTextureHandleNonResidentARB(m_HandleARB));
    }

	void LOpenGLImage::SetData(const void* data)
    {
		m_ImageData.Release();
		m_ImageData.Data = (void*)data;
    }

    void LOpenGLImage::Resize(uint32_t width, uint32_t height)
    {
    }

    void LOpenGLImage::Invalidate()
    {
		FBuffer imageData = FBuffer::Copy(m_ImageData);
		if (m_RendererID)
		{
            Release();
		}

		m_ImageData = imageData;
        GLenum internalFormat = GLUtils::OpenGLImageInternalFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 
						   Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height), 
						   GLUtils::OpenGLImageInternalFormat(m_Specification.Format), 
						   m_Specification.Width, 
						   m_Specification.Height);

        if (m_ImageData)
        {
			glTextureSubImage2D(m_RendererID, 
								0, 
								0, 
								0, 
								m_Specification.Width, 
								m_Specification.Height, 
								GLUtils::OpenGLImageFormat(m_Specification.Format), 
								GLUtils::OpenGLFormatDataType(m_Specification.Format), 
								(const void*)m_ImageData.Data);

			if (m_Specification.Mips > 1)
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


    LOpenGLImage2D::LOpenGLImage2D(const ImageSpecification& InImageSpecification, FBuffer&& InBuffer)
		: m_Specification(InImageSpecification)
		, m_ImageData(std::move(InBuffer))
    {
		if (m_ImageData)
		{
			LK_OpenGL(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
			LK_CORE_ASSERT(m_RendererID != 0, "RendererID is 0");

			LK_OpenGL(glTextureStorage2D(m_RendererID,
					  m_Specification.Mips,
					  GLUtils::OpenGLImageInternalFormat(m_Specification.Format),
					  m_Specification.Width,
					  m_Specification.Height));

			LK_CORE_VERIFY(m_ImageData.Data, "Image data is nullptr");
			LK_OpenGL(glTextureSubImage2D(m_RendererID,
					  0,
					  0,
					  0,
					  m_Specification.Width,
					  m_Specification.Height,
					  GLUtils::OpenGLImageFormat(m_Specification.Format),
					  GL_UNSIGNED_BYTE,
					  (const void*)m_ImageData.Data));

			//LK_CORE_DEBUG_TAG("OpenGLImage2D", "Generate Texture Mipmap");
			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
			}

			GLUtils::ApplyTextureWrap(m_RendererID, m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_RendererID, 
										m_Specification.Filter, 
										m_Specification.Mips > 1);
		}
    }

    LOpenGLImage2D::LOpenGLImage2D(const ImageSpecification& InImageSpecification, void* InData)
        : m_Specification(InImageSpecification)
	{
		if (InData)
		{
            const uint64_t ImageSize = Utils::GetMemorySize(InImageSpecification.Format, 
															InImageSpecification.Width, 
															InImageSpecification.Height);
			m_ImageData = FBuffer(InData, ImageSize);

			LK_OpenGL(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));

			LK_OpenGL(glTextureStorage2D(m_RendererID, 
					  Utils::CalculateMipCount(InImageSpecification.Width, InImageSpecification.Height), 
					  GLUtils::OpenGLImageInternalFormat(InImageSpecification.Format), 
					  InImageSpecification.Width, 
					  InImageSpecification.Height));

			LK_OpenGL(glTextureSubImage2D(
				m_RendererID, 
				0, 
				0, 
				0, 
				InImageSpecification.Width, 
				InImageSpecification.Height, 
				GLUtils::OpenGLImageFormat(InImageSpecification.Format), 
				GL_UNSIGNED_BYTE, 
				(const void*)m_ImageData.Data)
			);

			if (m_Specification.Mips > 1)
			{
				LK_OpenGL(glGenerateTextureMipmap(m_RendererID));
			}

			GLUtils::ApplyTextureWrap(m_RendererID, m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_RendererID,
										m_Specification.Filter,
										(m_Specification.Mips > 1));

			/* Clean up used assigned image data. */
			stbi_image_free(InData);
		}
    }

    LOpenGLImage2D::~LOpenGLImage2D()
    {
		Release();
    }

	void LOpenGLImage2D::SetData(const void* data)
    {
		m_ImageData.Release();
		m_ImageData.Data = (void*)data;
    }

    void LOpenGLImage2D::Resize(uint32_t width, uint32_t height)
    {
		LK_CORE_DEBUG_TAG("LOpenGLImage", "Resize ({}, {})", width, height);
    }

    void LOpenGLImage2D::Invalidate()
    {
		//FBuffer imageData = FBuffer::Copy(m_ImageData);
		FBuffer imageData = m_ImageData;
		if (m_RendererID)
		{
            Release();
		}

		m_ImageData = imageData;
        const GLenum internalFormat = GLUtils::OpenGLImageInternalFormat(m_Specification.Format);
        const uint32_t mipCount = Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 
						   mipCount, 
						   internalFormat, 
						   m_Specification.Width, 
						   m_Specification.Height);
        if (m_ImageData)
        {
			LK_OpenGL(glTextureSubImage2D(m_RendererID, 
					  0, 
					  0, 
					  0, 
					  m_Specification.Width, 
					  m_Specification.Height, 
					  GLUtils::OpenGLImageFormat(m_Specification.Format), 
					  GLUtils::OpenGLFormatDataType(m_Specification.Format),
					  (const void*)m_ImageData.Data)); 

			if (m_Specification.Mips > 1)
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