#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkEngine/Core/Application.h"

#include <stb_image/stb_image_resize2.h>

#include "LkOpenGL.h"


namespace LkEngine {

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

			LK_CORE_DEBUG_TAG("OpenGLTexture2D", "Loading image from path \"{}\"", m_FilePath.string());
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

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_Image->GetRendererID());
	}

	void OpenGLTexture2D::Unbind(uint32_t slot) const
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



	OpenGLTextureCube::OpenGLTextureCube(const TextureSpecification& specification, std::vector<std::filesystem::path> facePaths)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		//glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		//glBindTextureUnit(0, m_RendererID);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		for (unsigned int i = 0; i < facePaths.size(); i++)
		{
			std::filesystem::path& facePath = facePaths[i];
			stbi_uc* data = stbi_load(facePath.string().c_str(), &width, &height, &channels, 4);
			LK_CORE_VERIFY(data, "OpenGLTextureCube, failed to load data for face path \"{}\"", facePath.string());

			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);

			LK_CORE_DEBUG_TAG("OpenGLTextureCube", "Created face {} with texture image: {}", i, facePath.string());
		}
		stbi_set_flip_vertically_on_load(true);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	ImageFormat OpenGLTextureCube::GetFormat() const
	{
		return m_Specification.Format;
	}

	uint32_t OpenGLTextureCube::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OpenGLTextureCube::GetHeight() const
	{
		return m_Height;
	}

	uint32_t OpenGLTextureCube::GetMipLevelCount() const
	{
		return 0;
	}

	RendererID OpenGLTextureCube::GetRendererID() const
	{
		return m_RendererID;
	}

	RendererID& OpenGLTextureCube::GetRendererID() 
	{
		return m_RendererID;
	}

	const std::filesystem::path& OpenGLTextureCube::GetPath() const
	{
		return m_Filepath;
	}

	const std::string& OpenGLTextureCube::GetName() const
	{
		return m_Specification.Name;
	}


}