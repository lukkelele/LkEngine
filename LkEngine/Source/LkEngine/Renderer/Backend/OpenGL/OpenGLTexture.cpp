#include "LKpch.h"
#include "OpenGLTexture.h"

#include "LkOpenGL.h"

#include "LkEngine/Core/IO/FileSystem.h"

//#ifndef STB_IMAGE_RESIZE2_IMPLEMENTATION
//#define STB_IMAGE_RESIZE2_IMPLEMENTATION
//#endif
//#include <stb/stb_image_resize2.h>

namespace LkEngine {

	LOpenGLTexture2D::LOpenGLTexture2D(const FTextureSpecification& InSpec, FBuffer InBuffer)
		: Specification(InSpec)
		, m_Width(InSpec.Width)
		, m_Height(InSpec.Height)
		, m_FilePath(std::filesystem::path(InSpec.Path))
		, FileName(m_FilePath.filename().string())
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), "Invalid texture: {}", m_FilePath.string());

		FImageSpecification ImageSpec(InSpec);
		Image = LImage2D::Create(ImageSpec, InBuffer);
	}

	LOpenGLTexture2D::LOpenGLTexture2D(const FTextureSpecification& InSpec)
		: Specification(InSpec)
		, m_Width(InSpec.Width)
		, m_Height(InSpec.Height)
		, m_FilePath(std::filesystem::path(InSpec.Path))
		, FileName(m_FilePath.filename().string())
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(std::filesystem::is_regular_file(m_FilePath), "Invalid texture: {}", m_FilePath.string());

		if (!Specification.Path.empty())
		{
			LK_CORE_TRACE_TAG("OpenGLTexture2D", "Loading image from: '{}'", m_FilePath.string());
			stbi_set_flip_vertically_on_load(Specification.bFlipVertical);

			const bool TextureIsHdr = stbi_is_hdr(Specification.Path.c_str());

			stbi_uc* TextureData = nullptr;
			int ReadWidth, ReadHeight, ReadChannels;
			if (TextureIsHdr)
			{
				LK_CORE_WARN_TAG("OpenGLTexture2D", "Loading HDR texture: {}", Specification.Path);
				TextureData = (byte*)stbi_loadf(Specification.Path.c_str(), &ReadWidth, &ReadHeight, &ReadChannels, 4);
			}
			else
			{
				TextureData = stbi_load(Specification.Path.c_str(), &ReadWidth, &ReadHeight, &ReadChannels, 4);
			}
			LK_CORE_ASSERT(TextureData && (ReadWidth > 0) && (ReadHeight > 0), "Corrupt image data: {}", Specification.Path);

			FImageSpecification ImageSpec(Specification);
			ImageSpec.Size = (uint64_t)ReadWidth * (uint64_t)ReadHeight * (uint64_t)ReadChannels;
			if (TextureIsHdr)
			{
				ImageSpec.Size *= sizeof(float);
			}

			const uint32_t DataSize = ImageUtils::GetMemorySize(
				Specification.Format, 
				Specification.Width, 
				Specification.Height
			);

			if (Specification.bUseReadDimensions)
			{
				LK_CORE_DEBUG_TAG("OpenGLTexture2D", "Using size read from file: ({}, {})", ReadWidth, ReadHeight);
				Specification.Width = ReadWidth;
				Specification.Height = ReadHeight;
			}

			if (ImageSpec.Size != DataSize)
			{
				LK_CORE_TRACE_TAG("OpenGLTexture2D", "Creating image, resizing it to ({}, {}) from ({}, {})  (Size: {}, Spec.Size: {})", 
								  InSpec.Width, InSpec.Height, ReadWidth, ReadHeight, DataSize, ImageSpec.Size);
				TextureData = MemoryUtils::ResizeImageData(
					TextureData, 
					DataSize, 
					ReadWidth, 
					ReadHeight, 
					Specification.Width,  /* Target Width */
					Specification.Height, /* Target Height */
					STBIR_RGBA
				);
				LK_CORE_ASSERT(TextureData, "Resizing of image data failed");
			}

			Image = LImage2D::Create(ImageSpec, TextureData);
		}
		else
		{
			LK_CORE_WARN_TAG("OpenGLTexture2D", "Passed buffer is nullptr");
			FImageSpecification ImageSpec(Specification);
			Image = LImage2D::Create(ImageSpec, nullptr);
		}
	}

	LOpenGLTexture2D::~LOpenGLTexture2D()
	{
		if (Image)
		{
			Image->Release();
		}
	}

	void LOpenGLTexture2D::Bind(const uint32_t Slot) const
	{
		LK_CORE_ASSERT(Image, "Invalid image reference, cannot bind slot {}", Slot);
		LK_OpenGL_Verify(glBindTextureUnit(Slot, Image->GetRendererID()));
	}

	void LOpenGLTexture2D::Unbind(const uint32_t Slot) const
	{
		LK_OpenGL_Verify(glBindTextureUnit(Slot, 0));
	}

	void LOpenGLTexture2D::SetData(void* InData, const uint32_t InSize)
	{
		LK_OpenGL_Verify(glTextureSubImage2D(
			Image->GetRendererID(),
			0, 
			0, 
			0,
			Specification.Width,
			Specification.Height,
			LOpenGL::ImageFormatToDataFormat(Specification.Format), 
			GL_UNSIGNED_BYTE, 
			InData)
		);
	}

	void LOpenGLTexture2D::Lock()
	{
		bLocked = true;
	}

	void LOpenGLTexture2D::Unlock()
	{
		bLocked = false;
	}

	void LOpenGLTexture2D::Load()
	{
		bLoaded = true;
	}

	void LOpenGLTexture2D::Unload()
	{
		bLoaded = false;
	}

	uint32_t LOpenGLTexture2D::GetMipLevelCount() const
	{
		LK_CORE_ASSERT(Image);
		return Image->GetSpecification().Mips;
	}

	void LOpenGLTexture2D::Resize(const uint32_t NewWidth, const uint32_t NewHeight)
	{
		LK_CORE_ASSERT(Image);
		Image->Resize(NewWidth, NewHeight);
	}

	void LOpenGLTexture2D::Invalidate()
	{
		LK_CORE_ASSERT(Image);
		Image->Invalidate();
	}

	uint64_t LOpenGLTexture2D::GetARBHandle() const
	{
		LK_CORE_ASSERT(Image);
		return glGetTextureHandleARB(Image->GetRendererID());
	}


	/******************************************************************
	 * LOpenGLTextureCube
	 ******************************************************************/
	LOpenGLTextureCube::LOpenGLTextureCube(const FTextureSpecification& InSpec, 
										   std::vector<std::filesystem::path> InFacePaths)
		: Specification(InSpec)
		, m_Width(InSpec.Width)
		, m_Height(InSpec.Height)
	{
		LOBJECT_REGISTER();

		LK_OpenGL_Verify(glGenTextures(1, &m_RendererID));
		LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

		int Width, Height, Channels;
		stbi_set_flip_vertically_on_load(Specification.bFlipVertical);
		for (std::size_t i = 0; i < InFacePaths.size(); i++)
		{
			std::filesystem::path& FacePath = InFacePaths[i];
			stbi_uc* Data = stbi_load(FacePath.string().c_str(), &Width, &Height, &Channels, 4);
			LK_CORE_VERIFY(Data, "OpenGLTextureCube failed to load data for face path: '{}'", FacePath.string());

			LK_OpenGL_Verify(glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGBA32F,
				Width,
				Height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				Data)
			);

			stbi_image_free(Data);
			LK_CORE_TRACE_TAG("OpenGLTextureCube", "Created face {} with: '{}'", i, FacePath.string());
		}

		//stbi_set_flip_vertically_on_load(true);
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
