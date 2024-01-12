#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkOpenGL.h"

#include "LkEngine/Core/Application.h"


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


	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification)
	{
		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		if (specification.Path != "")
		{
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);

			m_Image = Image::Create(imageSpec, data);
			stbi_image_free(data);
		}
		else
		{
			m_Image = Image::Create(imageSpec, nullptr);
		}
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification, Buffer imageData)
	{
		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		m_Image = Image::Create(imageSpec, imageData);
#if 0
		if (specification.Path != "")
		{
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);

			m_Image = Image::Create(imageSpec, data);
			stbi_image_free(data);
		}
		else
		{
			m_Image = Image::Create(imageSpec, nullptr);
		}
#endif
	}


	OpenGLTexture::OpenGLTexture(const std::string& filePath)
	{
		LOG_TRACE("OpenGLTexture created: {}", filePath);
		m_FilePath = filePath;
		m_Name = File::ExtractFilenameWithoutExtension(filePath);

		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

		ImageSpecification imageSpec;
		imageSpec.Width = width;
		imageSpec.Height = height;
		m_Width = width;
		m_Height = height;
		imageSpec.Path = filePath;
		m_Image = Image::Create(imageSpec, data);
	}

	OpenGLTexture::OpenGLTexture(const OpenGLTexture& texture)
		: OpenGLTexture(texture.GetSpecification())
	{
	}

	OpenGLTexture::~OpenGLTexture()
	{
		GL_CALL(glDeleteTextures(1, &m_Image->GetRendererID()));
	}

	void OpenGLTexture::Bind(unsigned int slot /*= 0*/) 
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID()));
		//m_Loaded = true;
	}

	void OpenGLTexture::Unbind(unsigned slot)
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		//m_Loaded = false;
	}

	RendererID OpenGLTexture::GetRendererID() const
	{
		return m_Image->GetRendererID();
	}

	RendererID& OpenGLTexture::GetRendererID() 
	{
		return m_Image->GetRendererID();
	}

	void OpenGLTexture::SetData(void* data, uint32_t size)
	{
		GLenum dataFormat = ImageFormatToGLDataFormat(m_Specification.Format);
		uint32_t bpp = ImageFormatToGLDataFormat(m_Specification.Format) == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_Image->GetRendererID(), 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
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

	void OpenGLTexture::Load()
	{
		m_Loaded = true;
	}

	void OpenGLTexture::Unload()
	{
		m_Loaded = false;
	}

	s_ptr<Image> OpenGLTexture::GetImage()
	{
		return m_Image;
	}

	//=====================================================
	// OpenGLTexture 2D
	//=====================================================
	OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath)
	{
		LOG_TRACE("OpenGLTexture2D created: {}", filePath);
		m_FilePath = filePath;
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		ImageSpecification imageSpec;
		imageSpec.Width = width;
		imageSpec.Height = height;
		m_Width = width;
		m_Height = height;
		m_Name = File::ExtractFilenameWithoutExtension(filePath);

		m_Image = Image::Create(imageSpec, data);
		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, Buffer imageData)
	{
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.Filter;
		imageSpec.Wrap = specification.Wrap;

		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		m_Image = Image::Create(imageSpec, imageData);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Path = specification.Path;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.Filter;
		imageSpec.Wrap = specification.Wrap;

		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		// Try to read data from path
		if (specification.Path.empty() == false)
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);

			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);
            uint32_t memorySize = Image::GetMemorySize(specification.Format, specification.Width, specification.Height);

			imageSpec.Width = width;
			imageSpec.Height = height;
			m_Image = Image::Create(imageSpec, data);
		}
		else
			m_Image = Image::Create(imageSpec, nullptr);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
	}

	void OpenGLTexture2D::Bind(unsigned int slot /*= 0*/)
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID()));
	}

	void OpenGLTexture2D::Unbind(unsigned slot)
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	RendererID OpenGLTexture2D::GetRendererID() const
	{
		return m_Image->GetRendererID();
	}

	RendererID& OpenGLTexture2D::GetRendererID() 
	{
		return m_Image->GetRendererID();
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

	void OpenGLTexture2D::Load()
	{
		m_Loaded = true;
	}

	void OpenGLTexture2D::Unload()
	{
		m_Loaded = false;
	}

	s_ptr<Image> OpenGLTexture2D::GetImage()
	{
		return m_Image;
	}

}