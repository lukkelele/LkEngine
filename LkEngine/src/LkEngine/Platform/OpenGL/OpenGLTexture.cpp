#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkEngine/Core/Application.h"

#include <stb_image/stb_image_resize2.h>

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification, Buffer imageData)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
		, m_FilePath(std::filesystem::path(specification.Path))
	{
		if (m_Specification.Name.empty())
		{
			std::string filename = m_FilePath.filename().string().substr(0, m_FilePath.filename().string().size() - 4);
			m_Specification.Name = filename;
			if (m_Specification.DebugName.empty())
				m_Specification.DebugName = m_Specification.Name;
		}

		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
        imageSpec.Size = Utils::GetMemorySize(specification.Format, specification.Width, specification.Height);
		m_Specification.GenerateMips ? imageSpec.Mips = 2 : imageSpec.Mips = 1;

		m_Image = Image::Create(imageSpec, imageData);
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		m_FilePath = std::filesystem::path(specification.Path);
		if (m_Specification.Name.empty())
		{
			std::string filename = m_FilePath.filename().string().substr(0, m_FilePath.filename().string().size() - 4);
			if (m_Specification.DebugName.empty())
				m_Specification.DebugName = m_Specification.Name;
		}
		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;

		m_Specification.GenerateMips ? imageSpec.Mips = 2 : imageSpec.Mips = 1;

		// FIXME: This is bugged like biigggg time
		if (specification.Path != "")
		{
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);

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
		GL_CALL(glDeleteTextures(1, &m_Image->GetRendererID()));
	}

	void OpenGLTexture::Bind(unsigned int slot /*= 0*/) 
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Image->GetRendererID()));
	}

	void OpenGLTexture::Unbind(unsigned slot)
	{
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
		m_FilePath = std::filesystem::path(specification.Path);
		if (m_Specification.Name.empty())
		{
			std::string filename = m_FilePath.filename().string().substr(0, m_FilePath.filename().string().size() - 4);
			m_Specification.Name = filename;
			if (m_Specification.DebugName.empty())
				m_Specification.DebugName = m_Specification.Name;
		}
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.DebugName = specification.DebugName;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.SamplerFilter;
		imageSpec.Wrap = specification.SamplerWrap;
		m_Specification.GenerateMips ? imageSpec.Mips = 2 : imageSpec.Mips = 1;

		//m_FilePath = std::filesystem::path(specification.Path);

		imageSpec.Size = imageData.GetSize();
        uint32_t memorySize = Utils::GetMemorySize(specification.Format, specification.Width, specification.Height);
		if (imageSpec.Size != memorySize)
		{
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);
			LK_CORE_TRACE_TAG("OpenGLTexture2D", "Image {} specification doesn't match the read data size, resizing texture...", m_Specification.DebugName);
			imageData.Data = MemoryUtils::ResizeImageData(data, memorySize, width, height, specification.Width, specification.Height, STBIR_RGBA);
		}
		m_Image = Image2D::Create(imageSpec, imageData);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_FilePath(FilePath(specification.Path))
	{
		if (m_Specification.Name.empty())
		{
			std::string filename = m_FilePath.filename().string().substr(0, m_FilePath.filename().string().size() - 4);
			m_Specification.Name = filename;
			if (m_Specification.DebugName.empty())
				m_Specification.DebugName = m_Specification.Name;
		}
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.DebugName = specification.DebugName;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Path = specification.Path;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.SamplerFilter;
		imageSpec.Wrap = specification.SamplerWrap;
		m_Specification.GenerateMips ? imageSpec.Mips = 2 : imageSpec.Mips = 1;

		// Try to read data from path
		if (specification.Path.empty() == false)
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);

			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);
			LK_CORE_ASSERT(data, "Could not read data from {}", specification.Path.c_str());
            uint32_t memorySize = Utils::GetMemorySize(specification.Format, specification.Width, specification.Height);
			imageSpec.Size = (uint64_t)width * (uint64_t)height * (uint64_t)channels;
			if (imageSpec.Size != memorySize)
			{
				data = MemoryUtils::ResizeImageData(data, memorySize, width, height, specification.Width, specification.Height, STBIR_RGBA);
				//LK_CORE_TAG_TAG("OpenGLTexture2D", "Image {} specification doesn't match the read data size, resizing texture...", m_Specification.DebugName);
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


	//
	// TextureArray
	//
	TextureArray::TextureArray(const TextureArraySpecification& specification)
		: m_Specification(specification)
	{
		auto [width, height]= GLUtils::ConvertDimensionsToWidthAndHeight(m_Specification.Dimension);
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_Width, m_Height, 10 /* layers */, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		//GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 10);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		//AddTextureToArray(TextureLibrary::Get()->GetWhiteTexture2D());
	}

	void TextureArray::Bind()
	{
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	void TextureArray::Unbind()
	{
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	void TextureArray::AddTextureToArray(Ref<Texture> texture)
	{
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		Buffer imageBuffer = texture->GetWriteableBuffer();
		LK_ASSERT(imageBuffer.Data, "Data is nullptr");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Textures.size(), m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer.Data);
		m_Textures.push_back(texture);
		//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Textures.size(), m_Width, m_Height, 1, GLUtils::ImageFormatToGLDataFormat(texture->GetSpecification().Format), GL_UNSIGNED_BYTE, imageBuffer.Data);
		//LK_CORE_WARN_TAG("TextureArray", "Added texture {} to texture array {}x{}  slot={}, i={}", texture->GetName(), m_Width, m_Height, m_Specification.TextureSlot, m_Textures.size());

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}


}