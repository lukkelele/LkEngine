#include "LKpch.h"
#include "OpenGLImage.h"

#include "LkOpenGL.h"


namespace LkEngine {

	//-------------------------------------------------------------------------------
	// OpenGLImage
	//-------------------------------------------------------------------------------
    OpenGLImage::OpenGLImage(ImageSpecification spec, void* data)
        : m_Specification(spec)
	{
		if (data)
		{
			m_ImageData = Buffer::Copy(data, m_Specification.Size);

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexImage2D(GL_TEXTURE_2D, 0, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height, 0, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, (const void*)m_ImageData.Data);
			LK_CORE_TRACE("Created OpenGLImage");
			//glGenerateTextureMipmap(m_RendererID);

			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
				LK_CORE_DEBUG_TAG("OpenGLImage", "ImageSpecification::Mips > 1 ==> Generating mipmap");
			}
			else
				LK_CORE_DEBUG_TAG("OpenGLImage", "ImageSpecification::Mips < 1 ==> Not generating mipmap");

			GLUtils::ApplyTextureWrap(m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_Specification.Filter, m_Specification.Mips > 1);

			// Anistropic Filtering
			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

			glBindTexture(GL_TEXTURE_2D, 0);

			stbi_image_free(data);
		}
		else
		{
			// Data is null, try to load it from path
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));
			stbi_image_free(data);

			Invalidate();
		}
    }

    OpenGLImage::OpenGLImage(ImageSpecification spec, Buffer buffer)
		: m_Specification(spec)
    {
		if (buffer.Data)
		{
			//m_ImageData = Buffer::Copy(data, spec.Size);
            uint64_t memorySize = Utils::GetMemorySize(spec.Format, spec.Width, spec.Height);
			//m_ImageData = Buffer::Copy(data, memorySize);

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexImage2D(GL_TEXTURE_2D, 0, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height, 0, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, (const void*)m_ImageData.Data);
			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
				LK_CORE_DEBUG_TAG("OpenGLImage", "ImageSpecification::Mips > 1 ==> Generating mipmap");
			}
			else
				LK_CORE_DEBUG_TAG("OpenGLImage", "ImageSpecification::Mips < 1 ==> Not generating mipmap");

			GLUtils::ApplyTextureWrap(m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_Specification.Filter, m_Specification.Mips > 1);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			// Data is null, try to load it from path
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));

			stbi_image_free(data);
		}
    }

    OpenGLImage::~OpenGLImage()
    {
		//GL_CALL(glMakeTextureHandleNonResidentARB(m_HandleARB));
    }

	void OpenGLImage::SetData(const void* data)
    {
		m_ImageData.Release();
		m_ImageData.Data = (void*)data;
    }

    void OpenGLImage::Resize(uint32_t width, uint32_t height)
    {
    }

    void OpenGLImage::Invalidate()
    {
		Buffer imageData = Buffer::Copy(m_ImageData);
		if (m_RendererID)
            Release();

		m_ImageData = imageData;
        GLenum internalFormat = GLUtils::OpenGLImageInternalFormat(m_Specification.Format);
        //uint32_t mipCount = Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height), 
			GLUtils::OpenGLImageInternalFormat(m_Specification.Format), m_Specification.Width, m_Specification.Height);
        if (m_ImageData)
        {
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, 
				GLUtils::OpenGLImageFormat(m_Specification.Format), GLUtils::OpenGLFormatDataType(m_Specification.Format), m_ImageData.Data);
			if (m_Specification.Mips > 1)
				glGenerateTextureMipmap(m_RendererID);
        }

    }

	void OpenGLImage::RT_Invalidate()
	{
	}

    void OpenGLImage::AllocateMemory(uint64_t size)
    {
    }

    void OpenGLImage::Release()
    {
        if (m_RendererID)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
        m_ImageData.Release();
    }


	//-------------------------------------------------------------------------------
	// OpenGLImage2D
	//-------------------------------------------------------------------------------
    OpenGLImage2D::OpenGLImage2D(const ImageSpecification spec, Buffer buffer)
		: m_Specification(spec)
    {
		if (buffer.Data)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, spec.Mips, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, spec.Width, spec.Height, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, m_ImageData.Data);

			//if (m_Specification.Mips > 1)
			//	glGenerateTextureMipmap(m_RendererID);

			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
				LK_CORE_DEBUG_TAG("OpenGLImage2D", "ImageSpecification::Mips > 1 ==> Generating mipmap");
			}
			else
				LK_CORE_DEBUG_TAG("OpenGLImage2D", "ImageSpecification::Mips < 1 ==> Not generating mipmap");

			GLUtils::ApplyTextureWrap(m_RendererID, m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_RendererID, m_Specification.Filter, m_Specification.Mips > 1);
		}
		else
		{
			// Data is null, try to load it from path
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			if (!data)
				data = stbi_load("assets/Textures/white-texture.png", &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));
			stbi_image_free(data);
		}

    }

    OpenGLImage2D::OpenGLImage2D(const ImageSpecification spec, void* data)
        : m_Specification(spec)
	{
		if (data)
		{
            uint64_t memorySize = Utils::GetMemorySize(spec.Format, spec.Width, spec.Height);
			m_ImageData = Buffer::Copy(data, memorySize);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, Utils::CalculateMipCount(spec.Width, spec.Height), GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, spec.Width, spec.Height, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, m_ImageData.Data);

			//if (m_Specification.Mips > 1)
			//	glGenerateTextureMipmap(m_RendererID);

			if (m_Specification.Mips > 1)
			{
				glGenerateTextureMipmap(m_RendererID);
				LK_CORE_DEBUG_TAG("OpenGLImage2D", "ImageSpecification::Mips > 1 ==> Generating mipmap");
			}
			else
				LK_CORE_DEBUG_TAG("OpenGLImage2D", "ImageSpecification::Mips < 1 ==> Not generating mipmap");

			GLUtils::ApplyTextureWrap(m_RendererID, m_Specification.Wrap);
			GLUtils::ApplyTextureFilter(m_RendererID, m_Specification.Filter, m_Specification.Mips > 1);

			stbi_image_free(data);
		}
		else
		{
			LK_CORE_ASSERT(false, "Passed data was NULL, image spec: {}", spec.Name);
		}
    }

    OpenGLImage2D::~OpenGLImage2D()
    {
		Release();
    }

	void OpenGLImage2D::SetData(const void* data)
    {
		m_ImageData.Release();
		m_ImageData.Data = (void*)data;
    }

    void OpenGLImage2D::Resize(uint32_t width, uint32_t height)
    {
		LK_CORE_DEBUG_TAG("OpenGLImage", "Resize ({}, {})", width, height);
    }

    void OpenGLImage2D::Invalidate()
    {
		Buffer imageData = Buffer::Copy(m_ImageData);
		if (m_RendererID)
            Release();

		m_ImageData = imageData;
        GLenum internalFormat = GLUtils::OpenGLImageInternalFormat(m_Specification.Format);
        uint32_t mipCount = Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Specification.Width, m_Specification.Height);
        if (m_ImageData)
        {
            GLenum format = GLUtils::OpenGLImageFormat(m_Specification.Format);
            GLenum dataType = GLUtils::OpenGLFormatDataType(m_Specification.Format);
			LK_CORE_VERIFY(m_ImageData.Data != nullptr, "Image data is nullptr");
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, GLUtils::OpenGLImageFormat(m_Specification.Format), dataType, m_ImageData.Data);
			if (m_Specification.Mips > 1)
				glGenerateTextureMipmap(m_RendererID);
        }
    }

	void OpenGLImage2D::RT_Invalidate()
	{
	}

    void OpenGLImage2D::AllocateMemory(uint64_t size)
    {
    }

    void OpenGLImage2D::Release()
    {
        if (m_RendererID)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
        m_ImageData.Release();
    }



}