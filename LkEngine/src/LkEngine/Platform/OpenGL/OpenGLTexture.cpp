#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkEngine/Core/Application.h"

#include <stb_image/stb_image_resize2.h>

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLTexture::OpenGLTexture(const TextureSpecification& textureSpec, Buffer imageData)
		: m_Specification(textureSpec)
		, m_Width(textureSpec.Width)
		, m_Height(textureSpec.Height)
		, m_FilePath(std::filesystem::path(textureSpec.Path))
	{
		ImageSpecification imageSpec(textureSpec);
		m_Image = Image::Create(imageSpec, imageData);
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& textureSpec)
		: m_Specification(textureSpec)
		, m_Width(textureSpec.Width)
		, m_Height(textureSpec.Height)
		, m_FilePath(std::filesystem::path(textureSpec.Path))
	{
		ImageSpecification imageSpec(textureSpec);

		// FIXME: This is bugged like biigggg time
		if (!m_Specification.Path.empty())
		{
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			stbi_uc* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);

			// Check if texture isn't quadratic 
			if (width != height)
			{
				if (width > height)
					height = width;
				else
					width = height;

				imageSpec.Width = width;
				imageSpec.Height = height;
			}
			imageSpec.Size = imageSpec.Width * imageSpec.Height;

			m_Image = Image::Create(imageSpec, data);
		}
		else
		{
			m_Image = Image::Create(imageSpec, nullptr);
		}
	}

	OpenGLTexture::OpenGLTexture(const OpenGLTexture& texture)
		: OpenGLTexture(texture.GetSpecification())
	{
	}

	OpenGLTexture::~OpenGLTexture()
	{
		m_Image->Release();
	}

	void OpenGLTexture::Bind(unsigned int slot) 
	{
		glBindTextureUnit(slot, m_Image->GetRendererID());
	}

	void OpenGLTexture::Unbind(unsigned slot)
	{
		glBindTextureUnit(slot, 0);
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
	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& textureSpec, Buffer imageData)
		: m_Specification(textureSpec)
		, m_Width(textureSpec.Width)
		, m_Height(textureSpec.Height)
		, m_FilePath(std::filesystem::path(textureSpec.Path))
	{
		ImageSpecification imageSpec(textureSpec);
		uint32_t memorySize = imageSpec.Size; // Calculated in ImageSpecification constructor

		if (imageSpec.Size != imageData.GetSize())
		{
			int width, height, channels;
			stbi_uc* data = stbi_load(textureSpec.Path.c_str(), &width, &height, &channels, 4);
			LK_CORE_TRACE_TAG("OpenGLTexture2D", "Image {} specification doesn't match the read data size, resizing texture...", m_Specification.DebugName);
			imageData.Data = MemoryUtils::ResizeImageData(data, memorySize, width, height, textureSpec.Width, textureSpec.Height, STBIR_RGBA);
		}
		m_Image = Image2D::Create(imageSpec, imageData);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& textureSpec)
		: m_Specification(textureSpec)
		, m_Width(textureSpec.Width)
		, m_Height(textureSpec.Height)
		, m_FilePath(std::filesystem::path(textureSpec.Path))
	{
		ImageSpecification imageSpec(textureSpec);

		if (!textureSpec.Path.empty())
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);

			stbi_uc* data = stbi_load(textureSpec.Path.c_str(), &width, &height, &channels, 4);
            uint32_t dataSize = Utils::GetMemorySize(textureSpec.Format, textureSpec.Width, textureSpec.Height);
			imageSpec.Size = (uint64_t)width * (uint64_t)height * (uint64_t)channels;
			if (imageSpec.Size != dataSize)
				data = MemoryUtils::ResizeImageData(data, dataSize, width, height, textureSpec.Width, textureSpec.Height, STBIR_RGBA);
			m_Image = Image2D::Create(imageSpec, data);
		}
		else
		{
			LK_CORE_WARN_TAG("OpenGLTexture2D", "Passed buffer is nullptr");
			m_Image = Image2D::Create(imageSpec, nullptr);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		m_Image->Release();
	}

	void OpenGLTexture2D::Bind(unsigned int slot /*= 0*/)
	{
		glBindTextureUnit(slot, m_Image->GetRendererID());
	}

	void OpenGLTexture2D::Unbind(unsigned slot)
	{
		glBindTextureUnit(slot, 0);
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
		return m_Image->GetSpecification().Mips;
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		m_Image->Resize(width, height);
	}

	void OpenGLTexture2D::Invalidate()
	{
		m_Image->Invalidate();
	}

	uint64_t OpenGLTexture2D::GetARBHandle() const
	{ 
		return glGetTextureHandleARB(m_Image->GetRendererID()); 
	}

}