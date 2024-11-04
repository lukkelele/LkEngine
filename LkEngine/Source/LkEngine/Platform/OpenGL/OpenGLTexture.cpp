#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkEngine/Core/Application.h" /// FIXME: REMOVE

#include <stb_image/stb_image_resize2.h>

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLTexture2D::OpenGLTexture2D(const FTextureSpecification& InSpecification, FBuffer InBuffer)
		: Specification(InSpecification)
		, m_Width(InSpecification.Width)
		, m_Height(InSpecification.Height)
		, m_FilePath(std::filesystem::path(InSpecification.Path))
		, FileName(m_FilePath.filename().string())
	{
		LCLASS_REGISTER();

		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), 
					   "Texture file is not valid, \"{}\"", m_FilePath.string());

		FImageSpecification ImageSpec(InSpecification);
		m_Image = LImage2D::Create(ImageSpec, InBuffer);
	}

	OpenGLTexture2D::OpenGLTexture2D(const FTextureSpecification& InSpecification)
		: Specification(InSpecification)
		, m_Width(InSpecification.Width)
		, m_Height(InSpecification.Height)
		, m_FilePath(std::filesystem::path(InSpecification.Path))
		, FileName(m_FilePath.filename().string())
	{
		LCLASS_REGISTER();

		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), "Path is not valid: \"{}\"", m_FilePath.string());
		FImageSpecification ImageSpec(Specification);

		if (!Specification.Path.empty())
		{
			int Width, Height, Channels;
			stbi_set_flip_vertically_on_load(1);

			LK_CORE_TRACE_TAG("OpenGLTexture2D", "Loading image from path \"{}\"", m_FilePath.string());
			stbi_uc* data = stbi_load(Specification.Path.c_str(), &Width, &Height, &Channels, 4);

            const uint32_t DataSize = ImageUtils::GetMemorySize(Specification.Format, 
																Specification.Width, 
																Specification.Height);

			ImageSpec.Size = (uint64_t)Width * (uint64_t)Height * (uint64_t)Channels;
			if (ImageSpec.Size != DataSize)
			{
				data = MemoryUtils::ResizeImageData(data, DataSize, Width, Height, InSpecification.Width, InSpecification.Height, STBIR_RGBA);
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

	void OpenGLTexture2D::Bind(uint32_t Slot) const
	{
		glBindTextureUnit(Slot, m_Image->GetRendererID());
	}

	void OpenGLTexture2D::Unbind(uint32_t Slot) const
	{
		glBindTextureUnit(Slot, 0);
	}

	void OpenGLTexture2D::SetData(void* InData, const uint32_t InSize)
	{
		const GLenum DataFormat = LOpenGL::ImageFormatToGLDataFormat(Specification.Format);
		//const uint32_t BPP = (DataFormat == GL_RGBA ? 4 : 3);

		glTextureSubImage2D(m_Image->GetRendererID(), 
							0, 
							0, 
							0, 
							Specification.Width, 
							Specification.Height, 
							DataFormat, 
							GL_UNSIGNED_BYTE, 
							InData);
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

	void OpenGLTexture2D::Resize(const uint32_t NewWidth, const uint32_t NewHeight)
	{
		m_Image->Resize(NewWidth, NewHeight);
	}

	void OpenGLTexture2D::Invalidate()
	{
		m_Image->Invalidate();
	}

	uint64_t OpenGLTexture2D::GetARBHandle() const
	{ 
		return glGetTextureHandleARB(m_Image->GetRendererID()); 
	}


	OpenGLTextureCube::OpenGLTextureCube(const FTextureSpecification& InSpecification, 
										 std::vector<std::filesystem::path> InFacePaths)
		: Specification(InSpecification)
		, m_Width(InSpecification.Width)
		, m_Height(InSpecification.Height)
	{
		LCLASS_REGISTER();

		//glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		//glBindTextureUnit(0, m_RendererID);
		LK_OpenGL(glGenTextures(1, &m_RendererID));
		LK_OpenGL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

		int Width, Height, Channels;
		stbi_set_flip_vertically_on_load(false);
		for (std::size_t i = 0; i < InFacePaths.size(); i++)
		{
			std::filesystem::path& FacePath = InFacePaths[i];
			stbi_uc* Data = stbi_load(FacePath.string().c_str(), &Width, &Height, &Channels, 4);
			LK_CORE_VERIFY(Data, "OpenGLTextureCube failed to load data for face path: \"{}\"", FacePath.string());

			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			LK_OpenGL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
					               0, 
						           GL_RGBA32F, 
						           Width, 
						           Height, 
						           0, 
						           GL_RGBA, 
						           GL_UNSIGNED_BYTE, 
						           Data));

			stbi_image_free(Data);
			LK_CORE_DEBUG_TAG("OpenGLTextureCube", "Created face {} with texture image: {}", i, FacePath.string());
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

	void OpenGLTextureCube::Bind(const uint32_t Slot) const
	{
		glActiveTexture(GL_TEXTURE0 + Slot);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

}