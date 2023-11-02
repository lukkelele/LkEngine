#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLTexture.h"


namespace LkEngine {

	static GLenum ImageFormatToGLDataFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB:  
			case ImageFormat::RGB8:    return GL_RGB;
			case ImageFormat::RGBA:   
			case ImageFormat::RGBA8:   
			case ImageFormat::RGBA32F: return GL_RGBA;
		}
		return 0;
	}

	static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB:     return GL_RGB32F;
			case ImageFormat::RGB8:    return GL_RGB8;
			case ImageFormat::RGBA8:   return GL_RGBA8;
			case ImageFormat::RGBA:    return GL_RGBA8;
			case ImageFormat::RGBA32F: return GL_RGBA32F;
		}
		return 0;
	}

	OpenGLTexture::OpenGLTexture(const std::string& filePath)
	{
		LOG_TRACE("OpenGLTexture created: {}", filePath);
		m_RendererID = 0;
		m_FilePath = filePath;
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

		ImageSpecification imageSpec;
		imageSpec.Width = width;
		imageSpec.Height = height;
		m_Width = width;
		m_Height = height;
		m_Image = Image::Create(imageSpec, data);
		//m_Image->Invalidate();
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& textureSpec)
	{
		m_RendererID = 0;
		ImageSpecification imageSpec;
		m_Image = Image::Create(imageSpec, nullptr);
		//m_Image->Invalidate();
	}

	OpenGLTexture::~OpenGLTexture()
	{
		GL_CALL(glDeleteTextures(1, &m_RendererID));
	}

	void OpenGLTexture::Bind(unsigned int slot /*= 0*/) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID()));
	}

	void OpenGLTexture::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void OpenGLTexture::SetData(void* data, uint32_t size)
	{
		GLenum dataFormat = ImageFormatToGLDataFormat(m_Specification.Format);
		uint32_t bpp = ImageFormatToGLDataFormat(m_Specification.Format) == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture::Lock()
	{
		m_Locked = true;
	}

	void OpenGLTexture::Unlock()
	{
		m_Locked = false;
		// Submit to renderer
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		ImageSpecification imageSpec;
		m_Image = Image::Create(imageSpec, nullptr);
		//m_Image->Invalidate();
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath)
	{
		LOG_TRACE("OpenGLTexture2D created: {}", filePath);
		m_FilePath = filePath;
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

		ImageSpecification imageSpec;
		imageSpec.Width = width;
		imageSpec.Height = height;
		m_Width = width;
		m_Height = height;

		m_Image = Image::Create(imageSpec, data);
		//m_Image->Invalidate();
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
	}

	void OpenGLTexture2D::Bind(unsigned int slot /*= 0*/) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID()));
	}

	void OpenGLTexture2D::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	// TODO: Remove this ?
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		GLenum dataFormat = ImageFormatToGLDataFormat(m_Specification.Format);
		uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;
		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		glTextureSubImage2D(m_Image->GetRendererID(), 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Lock()
	{
		m_Locked = true;
	}

	void OpenGLTexture2D::Unlock()
	{
		m_Locked = false;
		// Submit to renderer
	}


}