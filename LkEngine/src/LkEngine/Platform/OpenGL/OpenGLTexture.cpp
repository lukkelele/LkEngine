#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkOpenGL.h"

#include "LkEngine/Core/Application.h"

#include <stb_image/stb_image_resize2.h>


namespace LkEngine {

	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
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
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
        imageSpec.Size = Utils::GetMemorySize(specification.Format, specification.Width, specification.Height);

		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		m_Image = Image::Create(imageSpec, imageData);
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
		if (m_InTextureArray)
			return;
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID()));
	}

	void OpenGLTexture::Unbind(unsigned slot)
	{
		if (m_InTextureArray)
			return;
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
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
		GLenum dataFormat = GLUtils::ImageFormatToGLDataFormat(m_Specification.Format);
		uint32_t bpp = GLUtils::ImageFormatToGLDataFormat(m_Specification.Format) == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_Image->GetRendererID(), 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
	}

	uint32_t OpenGLTexture::GetMipLevelCount() const
	{
		return 1;
	}

	uint64_t OpenGLTexture::GetARBHandle() const
	{ 
		return glGetTextureHandleARB(m_Image->GetRendererID()); 
	}

	void OpenGLTexture::Invalidate()
	{
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

	void OpenGLTexture::Resize(uint32_t width, uint32_t height)
	{
		m_Image->Resize(width, height);
	}

	//------------------------------------------------------------------------------------
	// OpenGLTexture 2D
	//------------------------------------------------------------------------------------
	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, Buffer imageData)
		: m_Specification(specification)
	{
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.DebugName = specification.DebugName;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.SamplerFilter;
		imageSpec.Wrap = specification.SamplerWrap;

		imageSpec.Size = imageData.GetSize();
		LK_CORE_WARN_TAG("OpenGLTexture2D", "imageSpec.Size={}", imageSpec.Size);
        uint32_t memorySize = Utils::GetMemorySize(specification.Format, specification.Width, specification.Height);
		if (imageSpec.Size != memorySize)
		{
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);
			LK_CORE_INFO_TAG("OpenGLTexture2D", "Image {} specification doesn't match the read data size, resizing texture...", m_Specification.DebugName);
			imageData.Data = MemoryUtils::ResizeImageData(data, memorySize, width, height, specification.Width, specification.Height, STBIR_RGBA);
		}
		m_Image = Image2D::Create(imageSpec, imageData);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
	{
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.DebugName = specification.DebugName;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Path = specification.Path;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.SamplerFilter;
		imageSpec.Wrap = specification.SamplerWrap;

		// Try to read data from path
		if (specification.Path.empty() == false)
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);

			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);
            uint32_t memorySize = Utils::GetMemorySize(specification.Format, specification.Width, specification.Height);
			imageSpec.Size = (uint64_t)width * (uint64_t)height * (uint64_t)channels;
			if (imageSpec.Size != memorySize)
			{
				data = MemoryUtils::ResizeImageData(data, memorySize, width, height, specification.Width, specification.Height, STBIR_RGBA);
				LK_CORE_INFO_TAG("OpenGLTexture2D", "Image {} specification doesn't match the read data size, resizing texture...", m_Specification.DebugName);
			}
			m_Image = Image2D::Create(imageSpec, data);
		}
		else
		{
			LK_CORE_WARN_TAG("OpenGLTexture", "Passed buffer is nullptr");
			m_Image = Image2D::Create(imageSpec, nullptr);
		}
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

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		GLenum dataFormat = GLUtils::ImageFormatToGLDataFormat(m_Specification.Format);
		uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_Image->GetRendererID(), 0, 0, 0, m_Specification.Width, m_Specification.Height, dataFormat, GL_UNSIGNED_BYTE, data);
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

	Ref<Image2D> OpenGLTexture2D::GetImage()
	{
		return m_Image;
	}

	uint32_t OpenGLTexture2D::GetMipLevelCount() const
	{
		return 1;
	}

	uint64_t OpenGLTexture2D::GetARBHandle() const
	{ 
		return glGetTextureHandleARB(m_Image->GetRendererID()); 
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		m_Image->Resize(width, height);
	}

	void OpenGLTexture2D::Invalidate()
	{
	}

}