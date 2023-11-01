#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLTexture.h"


namespace LkEngine {

	static GLenum ImageFormatToGLDataFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB8:  return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
		}
		return 0;
	}

	static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
		}
		return 0;
	}

	OpenGLTexture::OpenGLTexture(const std::string& filePath)
	{
		m_RendererID = 0;
		m_FilePath = filePath;
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

		GL_CALL(glGenTextures(1, &m_RendererID));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // S: x
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // T: y

		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

		stbi_image_free(data);
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& textureSpec)
	{
		m_RendererID = 0;
		m_InternalFormat = ImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = ImageFormatToGLDataFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		GL_CALL(glDeleteTextures(1, &m_RendererID));
	}

	void OpenGLTexture::Bind(unsigned int slot /*= 0*/) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	}

	void OpenGLTexture::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void OpenGLTexture::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
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
	{

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath)
	{

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
	}

	void OpenGLTexture2D::Bind(unsigned int slot /*= 0*/) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	}

	void OpenGLTexture2D::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
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