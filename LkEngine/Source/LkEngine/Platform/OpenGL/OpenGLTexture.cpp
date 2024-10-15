#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkEngine/Core/Application.h" /// FIXME: REMOVE

#include <stb_image/stb_image_resize2.h>

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& textureSpec, FBuffer InBuffer)
		: m_Specification(textureSpec)
		, m_Width(textureSpec.Width)
		, m_Height(textureSpec.Height)
		, m_FilePath(std::filesystem::path(textureSpec.Path))
		, FileName(m_FilePath.filename().string())
	{
		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), 
					   "Texture file is not valid, \"{}\"", m_FilePath.string());

		ImageSpecification ImageSpec(textureSpec);
		const uint64_t memorySize = ImageSpec.Size; /* Calculated in ImageSpecification constructor. */

	#if 0
		if (ImageSpec.Size != InBuffer.GetSize())
		{
			int Width, Height, Channels;
			stbi_uc* data = stbi_load(textureSpec.Path.c_str(), &Width, &Height, &Channels, 4);
			LK_CORE_TRACE_TAG("OpenGLTexture2D", "Image {} specification doesn't match the "
							  "read data size, resizing texture...", m_Specification.DebugName);
			InBuffer.Data = MemoryUtils::ResizeImageData(data, memorySize, Width, Height, textureSpec.Width, textureSpec.Height, STBIR_RGBA);
		}
	#endif

		m_Image = LImage2D::Create(ImageSpec, InBuffer);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& textureSpec)
		: m_Specification(textureSpec)
		, m_Width(textureSpec.Width)
		, m_Height(textureSpec.Height)
		, m_FilePath(std::filesystem::path(textureSpec.Path))
		, FileName(m_FilePath.filename().string())
	{
		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), 
					   "Texture file is not valid, \"{}\"", m_FilePath.string());
		ImageSpecification ImageSpec(textureSpec);

		if (!textureSpec.Path.empty())
		{
			int Width, Height, Channels;
			stbi_set_flip_vertically_on_load(1);

			LK_CORE_TRACE_TAG("OpenGLTexture2D", "Loading image from path \"{}\"", m_FilePath.string());
			stbi_uc* data = stbi_load(textureSpec.Path.c_str(), &Width, &Height, &Channels, 4);

            const uint32_t DataSize = Utils::GetMemorySize(textureSpec.Format, textureSpec.Width, textureSpec.Height);

			ImageSpec.Size = (uint64_t)Width * (uint64_t)Height * (uint64_t)Channels;
			if (ImageSpec.Size != DataSize)
			{
				data = MemoryUtils::ResizeImageData(data, DataSize, Width, Height, textureSpec.Width, textureSpec.Height, STBIR_RGBA);
			}

			m_Image = LImage2D::Create(ImageSpec, data);
		}
		else
		{
			LK_CORE_WARN_TAG("OpenGLTexture2D", "Passed buffer is nullptr");
			m_Image = LImage2D::Create(ImageSpec, nullptr);
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

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		const GLenum dataFormat = GLUtils::ImageFormatToGLDataFormat(m_Specification.Format);
		const uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;

		glTextureSubImage2D(m_Image->GetRendererID(), 
							0, 
							0, 
							0, 
							m_Specification.Width, 
							m_Specification.Height, 
							dataFormat, 
							GL_UNSIGNED_BYTE, 
							data);
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

	uint32_t OpenGLTexture2D::GetMipLevelCount() const
	{
		return m_Image->GetSpecification().Mips;
	}

	void OpenGLTexture2D::Resize(uint32_t Width, uint32_t Height)
	{
		m_Image->Resize(Width, Height);
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

		int Width, Height, Channels;
		stbi_set_flip_vertically_on_load(false);
		for (unsigned int i = 0; i < facePaths.size(); i++)
		{
			std::filesystem::path& facePath = facePaths[i];
			stbi_uc* data = stbi_load(facePath.string().c_str(), &Width, &Height, &Channels, 4);
			LK_CORE_VERIFY(data, "OpenGLTextureCube, failed to load data for face path \"{}\"", facePath.string());

			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

}