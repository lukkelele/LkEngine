#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkOpenGL.h"

#include <stb_image/stb_image_resize2.h>


namespace LkEngine {

	LOpenGLTexture2D::LOpenGLTexture2D(const FTextureSpecification& InSpecification, FBuffer InBuffer)
		: Specification(InSpecification)
		, m_Width(InSpecification.Width)
		, m_Height(InSpecification.Height)
		, m_FilePath(std::filesystem::path(InSpecification.Path))
		, FileName(m_FilePath.filename().string())
	{
		LCLASS_REGISTER();

		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), "Texture file is not valid, \"{}\"", m_FilePath.string());

		FImageSpecification ImageSpec(InSpecification);
		m_Image = LImage2D::Create(ImageSpec, InBuffer);
	}

	LOpenGLTexture2D::LOpenGLTexture2D(const FTextureSpecification& InSpecification)
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
			stbi_uc* LoadedData = stbi_load(Specification.Path.c_str(), &Width, &Height, &Channels, 4);

			const uint32_t DataSize = ImageUtils::GetMemorySize(Specification.Format,
																Specification.Width,
																Specification.Height);

			ImageSpec.Size = (uint64_t)Width * (uint64_t)Height * (uint64_t)Channels;
			if (ImageSpec.Size != DataSize)
			{
				LoadedData = MemoryUtils::ResizeImageData(LoadedData, 
														  DataSize, 
														  Width, 
														  Height, 
														  InSpecification.Width, 
														  InSpecification.Height, 
														  STBIR_RGBA);
			}

			m_Image = LImage2D::Create(ImageSpec, LoadedData);
		}
		else
		{
			LK_CORE_WARN_TAG("OpenGLTexture2D", "Passed buffer is nullptr");
			m_Image = LImage2D::Create(ImageSpec, nullptr);
		}
	}

	LOpenGLTexture2D::~LOpenGLTexture2D()
	{
		if (m_Image)
		{
			m_Image->Release();
		}
	}

	void LOpenGLTexture2D::Bind(uint32_t Slot) const
	{
		LK_CORE_ASSERT(m_Image, "Invalid image reference, cannot bind slot {}", Slot);
		LK_OpenGL_Verify(glBindTextureUnit(Slot, m_Image->GetRendererID()));
	}

	void LOpenGLTexture2D::Unbind(uint32_t Slot) const
	{
		LK_OpenGL_Verify(glBindTextureUnit(Slot, 0));
	}

	void LOpenGLTexture2D::SetData(void* InData, const uint32_t InSize)
	{
		LK_OpenGL_Verify(glTextureSubImage2D(m_Image->GetRendererID(),
											 0,
											 0,
											 0,
											 Specification.Width,
											 Specification.Height,
											 LOpenGL::ImageFormatToDataFormat(Specification.Format),
											 GL_UNSIGNED_BYTE,
											 InData));
	}

	void LOpenGLTexture2D::Lock()
	{
		m_Locked = true;
	}

	void LOpenGLTexture2D::Unlock()
	{
		m_Locked = false;
	}

	void LOpenGLTexture2D::Load()
	{
		m_Loaded = true;
	}

	void LOpenGLTexture2D::Unload()
	{
		m_Loaded = false;
	}

	uint32_t LOpenGLTexture2D::GetMipLevelCount() const
	{
		return m_Image->GetSpecification().Mips;
	}

	void LOpenGLTexture2D::Resize(const uint32_t NewWidth, const uint32_t NewHeight)
	{
		m_Image->Resize(NewWidth, NewHeight);
	}

	void LOpenGLTexture2D::Invalidate()
	{
		m_Image->Invalidate();
	}

	uint64_t LOpenGLTexture2D::GetARBHandle() const
	{
		return glGetTextureHandleARB(m_Image->GetRendererID());
	}


	LOpenGLTextureCube::LOpenGLTextureCube(const FTextureSpecification& InSpecification, 
										 std::vector<std::filesystem::path> InFacePaths)
		: Specification(InSpecification)
		, m_Width(InSpecification.Width)
		, m_Height(InSpecification.Height)
	{
		LCLASS_REGISTER();

		LK_OpenGL_Verify(glGenTextures(1, &m_RendererID));
		LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

		int Width, Height, Channels;
		stbi_set_flip_vertically_on_load(false);
		for (std::size_t i = 0; i < InFacePaths.size(); i++)
		{
			std::filesystem::path& FacePath = InFacePaths[i];
			stbi_uc* Data = stbi_load(FacePath.string().c_str(), &Width, &Height, &Channels, 4);
			LK_CORE_VERIFY(Data, "OpenGLTextureCube failed to load data for face path: \"{}\"", FacePath.string());

			LK_OpenGL_Verify(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
										  0,
										  GL_RGBA32F,
										  Width,
										  Height,
										  0,
										  GL_RGBA,
										  GL_UNSIGNED_BYTE,
										  Data));

			stbi_image_free(Data);
			LK_CORE_DEBUG_TAG("OpenGLTextureCube", "Created face {} with: '{}'", i, FacePath.string());
		}

		stbi_set_flip_vertically_on_load(true);

		LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	}

	LOpenGLTextureCube::~LOpenGLTextureCube()
	{
		/* TODO: Release resources. */
	}

	void LOpenGLTextureCube::Bind(const uint32_t Slot) const
	{
		LK_OpenGL_Verify(glActiveTexture(GL_TEXTURE0 + Slot));
		LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));
	}

}
