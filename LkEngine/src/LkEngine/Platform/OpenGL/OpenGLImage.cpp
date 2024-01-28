#include "LKpch.h"
#include "OpenGLImage.h"

#include "LkOpenGL.h"


namespace LkEngine {

	//-------------------------------------------------------------------------------
	// OpenGLImage
	//-------------------------------------------------------------------------------
    OpenGLImage::OpenGLImage(ImageSpecification spec, void* data)
        : m_Specification(spec)
	{
		if (data)
		{
			//m_ImageData = Buffer::Copy(data, spec.Size);
            uint64_t memorySize = Utils::GetMemorySize(spec.Format, spec.Width, spec.Height);
			m_ImageData = Buffer::Copy(data, memorySize);

			GL_CALL(glGenTextures(1, &m_RendererID));
			GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData.Data));
			GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height, 0, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, (const void*)m_ImageData.Data));
			GL_CALL(glGenerateTextureMipmap(m_RendererID));
			//GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

			// Texture Wrap
			if (m_Specification.Wrap == TextureWrap::Clamp)
			{
				// S == x, T == y
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); 
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); 

			}
			else if (m_Specification.Wrap == TextureWrap::Repeat)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // S: x
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)); // T: y
			}

			// Texture Filter
			if (m_Specification.Filter == TextureFilter::Linear)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			}
			else if (m_Specification.Filter == TextureFilter::Nearest)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			}

			// Anistropic Filtering
			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
#if 0
			// Create ARB handle
			GL_CALL(m_HandleARB = glGetTextureHandleARB(m_RendererID));
			GL_CALL(glMakeTextureHandleResidentARB(m_HandleARB));
			LK_CORE_TRACE_TAG("OpenGLImage", "ARB Handle set to {}", m_HandleARB);
#endif
			GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		}
		else
		{
			// Data is null, try to load it from path
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));
			stbi_image_free(data);
		}
    }

    OpenGLImage::OpenGLImage(ImageSpecification spec, Buffer buffer)
    {
		if (buffer.Data)
		{
			//m_ImageData = Buffer::Copy(data, spec.Size);
            uint64_t memorySize = Utils::GetMemorySize(spec.Format, spec.Width, spec.Height);
			//m_ImageData = Buffer::Copy(data, memorySize);

			GL_CALL(glGenTextures(1, &m_RendererID));
			GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData.Data));
			GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height, 0, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, (const void*)m_ImageData.Data));
			GL_CALL(glGenerateTextureMipmap(m_RendererID));
			//GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

			// Texture Wrap
			if (m_Specification.Wrap == TextureWrap::Clamp)
			{
				// S == x, T == y
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); 
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); 

			}
			else if (m_Specification.Wrap == TextureWrap::Repeat)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // S: x
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)); // T: y
			}

			// Texture Filter
			if (m_Specification.Filter == TextureFilter::Linear)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			}
			else if (m_Specification.Filter == TextureFilter::Nearest)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			}

			// Anistropic Filtering
			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
#if 0
			// Create ARB handle
			GL_CALL(m_HandleARB = glGetTextureHandleARB(m_RendererID));
			GL_CALL(glMakeTextureHandleResidentARB(m_HandleARB));
			LK_CORE_TRACE_TAG("OpenGLImage", "ARB Handle set to {}", m_HandleARB);
#endif
			GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		}
		else
		{
			// Data is null, try to load it from path
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));
			stbi_image_free(data);
		}
    }

    OpenGLImage::~OpenGLImage()
    {
		//GL_CALL(glMakeTextureHandleNonResidentARB(m_HandleARB));
    }

	void OpenGLImage::SetData(const void* data)
    {
		m_ImageData.Release();
		m_ImageData.Data = (void*)data;
    }

    void OpenGLImage::Resize(uint32_t width, uint32_t height)
    {
#if 0
		glDeleteTextures(1, &m_RendererID);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		m_Specification.Width = width;
		m_Specification.Height = height;
		//Invalidate();
		//  1 pixel buffer (void* buffer) with RGBA format
		static uint32_t whiteTextureData = 0xFFFFFFFF; // White Pixel

		// Create a new buffer with the size of the new texture
		//std::vector<uint32_t> increasedBuffer(width * height, *originalPixel);
		std::vector<uint32_t> increasedBuffer(width * height, whiteTextureData);
		m_ImageData.Release();
		m_ImageData.Data = increasedBuffer.data();
		LK_CORE_DEBUG_TAG("Buffer", "New size from 1x1 --> {}", 4 * width * height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Allocate the new texture data (with newWidth and newHeight)
		GL_CALL(glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GLUtils::OpenGLImageInternalFormat(m_Specification.Format), 
			m_Specification.Width, 
			m_Specification.Height, 
			0, 
			GLUtils::OpenGLImageFormat(m_Specification.Format), 
			GL_UNSIGNED_BYTE, 
			(const void*)m_ImageData.Data)
		);

		glBindTexture(GL_TEXTURE_2D, 0);
#endif
		LK_CORE_DEBUG_TAG("OpenGLImage", "Resize ({}, {})", width, height);
    }

    void OpenGLImage::Invalidate()
    {
		Buffer imageData = Buffer::Copy(m_ImageData);
		if (m_RendererID)
		{
            Release();
		}

		m_ImageData = imageData;
        GLenum internalFormat = GLUtils::OpenGLImageInternalFormat(m_Specification.Format);
        uint32_t mipCount = Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height);
		std::string imageFormatString = Utils::ImageFormatToString(m_Specification.Format);

		LK_CORE_DEBUG_TAG("Image Format", "Internal: {}", imageFormatString);

		GL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
		GL_CALL(glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Specification.Width, m_Specification.Height));
        if (m_ImageData)
        {
            GLenum format = GLUtils::OpenGLImageFormat(m_Specification.Format);
            GLenum dataType = GLUtils::OpenGLFormatDataType(m_Specification.Format);
			LK_CORE_VERIFY(m_ImageData.Data != nullptr, "Image data is nullptr");
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, format, dataType, m_ImageData.Data);
			GL_CALL(glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, format, dataType, m_ImageData.Data));
			GL_CALL(glGenerateTextureMipmap(m_RendererID));
        }
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

	void OpenGLImage::RT_Invalidate()
	{
	}

    void OpenGLImage::AllocateMemory(uint64_t size)
    {
    }

    void OpenGLImage::Release()
    {
        if (m_RendererID)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
        m_ImageData.Release();
    }

#if 0
	void OpenGLImage::ApplySpecificationProperties()
	{
		ApplyWrap();
		ApplyFilter();
	}

	void OpenGLImage::ApplyWrap()
	{
		// Texture Wrap
		if (m_Specification.Wrap == TextureWrap::Clamp)
		{
			// S == x, T == y
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); 
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); 

		}
		else if (m_Specification.Wrap == TextureWrap::Repeat)
		{
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // S: x
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)); // T: y
		}
	}

	void OpenGLImage::ApplyFilter()
	{
		// Texture Filter
		if (m_Specification.Filter == TextureFilter::Linear)
		{
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
		else if (m_Specification.Filter == TextureFilter::Nearest)
		{
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		}
	}
#endif


	//-------------------------------------------------------------------------------
	// OpenGLImage2D
	//-------------------------------------------------------------------------------
    OpenGLImage2D::OpenGLImage2D(ImageSpecification spec, void* data)
        : m_Specification(spec)
	{
		if (data)
		{
			//m_ImageData = Buffer::Copy(data, spec.Size);
            uint64_t memorySize = Utils::GetMemorySize(spec.Format, spec.Width, spec.Height);
			m_ImageData = Buffer::Copy(data, memorySize);

			GL_CALL(glGenTextures(1, &m_RendererID));
			GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData.Data));
			GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height, 0, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, (const void*)m_ImageData.Data));
			GL_CALL(glGenerateTextureMipmap(m_RendererID));
			//GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

			// Texture Wrap
			if (m_Specification.Wrap == TextureWrap::Clamp)
			{
				// S == x, T == y
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); 
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); 

			}
			else if (m_Specification.Wrap == TextureWrap::Repeat)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // S: x
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)); // T: y
			}

			// Texture Filter
			if (m_Specification.Filter == TextureFilter::Linear)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			}
			else if (m_Specification.Filter == TextureFilter::Nearest)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			}

			// Anistropic Filtering
			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			// Data is null, try to load it from path
			LK_CORE_TRACE_TAG("OpenGLImage2D", "No data passed to image, tried loading white texture");
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			if (!data)
				data = stbi_load("assets/textures/white-texture.png", &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));
			stbi_image_free(data);
		}
    }

    OpenGLImage2D::OpenGLImage2D(ImageSpecification spec, Buffer buffer)
		: m_Specification(spec)
    {
		if (buffer.Data)
		{
			//m_ImageData = Buffer::Copy(data, spec.Size);
            //uint64_t memorySize = Utils::GetMemorySize(spec.Format, spec.Width, spec.Height);
			//m_ImageData = Buffer::Copy(data, memorySize);

			GL_CALL(glGenTextures(1, &m_RendererID));
			GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData.Data));
			GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GLUtils::OpenGLImageInternalFormat(spec.Format), spec.Width, spec.Height, 0, GLUtils::OpenGLImageFormat(spec.Format), GL_UNSIGNED_BYTE, (const void*)m_ImageData.Data));
			GL_CALL(glGenerateTextureMipmap(m_RendererID));
			//GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

			// Texture Wrap
			if (m_Specification.Wrap == TextureWrap::Clamp)
			{
				// S == x, T == y
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); 
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); 

			}
			else if (m_Specification.Wrap == TextureWrap::Repeat)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); // S: x
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)); // T: y
			}

			// Texture Filter
			if (m_Specification.Filter == TextureFilter::Linear)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			}
			else if (m_Specification.Filter == TextureFilter::Nearest)
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			}

			// Anistropic Filtering
			GLfloat maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			// Data is null, try to load it from path
			LK_CORE_TRACE_TAG("OpenGLImage2D", "No data passed to image, tried loading white texture");
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			void* data = stbi_load(m_Specification.Path.c_str(), &width, &height, &channels, 4);
			if (!data)
				data = stbi_load("assets/textures/white-texture.png", &width, &height, &channels, 4);
			m_ImageData = Buffer(data, Utils::GetMemorySize(m_Specification.Format, width, height));
			stbi_image_free(data);
		}

    }

    OpenGLImage2D::~OpenGLImage2D()
    {
		//GL_CALL(glMakeTextureHandleNonResidentARB(m_HandleARB));
    }

	void OpenGLImage2D::SetData(const void* data)
    {
		m_ImageData.Release();
		m_ImageData.Data = (void*)data;
    }

    void OpenGLImage2D::Resize(uint32_t width, uint32_t height)
    {
#if 0
		glDeleteTextures(1, &m_RendererID);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		m_Specification.Width = width;
		m_Specification.Height = height;
		//Invalidate();
		//  1 pixel buffer (void* buffer) with RGBA format
		static uint32_t whiteTextureData = 0xFFFFFFFF; // White Pixel

		// Create a new buffer with the size of the new texture
		//std::vector<uint32_t> increasedBuffer(width * height, *originalPixel);
		std::vector<uint32_t> increasedBuffer(width * height, whiteTextureData);
		m_ImageData.Release();
		m_ImageData.Data = increasedBuffer.data();
		LK_CORE_DEBUG_TAG("Buffer", "New size from 1x1 --> {}", 4 * width * height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Allocate the new texture data (with newWidth and newHeight)
		GL_CALL(glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GLUtils::OpenGLImageInternalFormat(m_Specification.Format), 
			m_Specification.Width, 
			m_Specification.Height, 
			0, 
			GLUtils::OpenGLImageFormat(m_Specification.Format), 
			GL_UNSIGNED_BYTE, 
			(const void*)m_ImageData.Data)
		);

		glBindTexture(GL_TEXTURE_2D, 0);
#endif
		LK_CORE_DEBUG_TAG("OpenGLImage", "Resize ({}, {})", width, height);
    }

    void OpenGLImage2D::Invalidate()
    {
		Buffer imageData = Buffer::Copy(m_ImageData);
		if (m_RendererID)
		{
            Release();
		}

		m_ImageData = imageData;
        GLenum internalFormat = GLUtils::OpenGLImageInternalFormat(m_Specification.Format);
        uint32_t mipCount = Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height);
		std::string imageFormatString = Utils::ImageFormatToString(m_Specification.Format);

		LK_CORE_DEBUG_TAG("Image Format", "Internal: {}", imageFormatString);

		GL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
		GL_CALL(glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Specification.Width, m_Specification.Height));
        if (m_ImageData)
        {
            GLenum format = GLUtils::OpenGLImageFormat(m_Specification.Format);
            GLenum dataType = GLUtils::OpenGLFormatDataType(m_Specification.Format);
			LK_CORE_VERIFY(m_ImageData.Data != nullptr, "Image data is nullptr");
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, format, dataType, m_ImageData.Data);
			GL_CALL(glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, format, dataType, m_ImageData.Data));
			GL_CALL(glGenerateTextureMipmap(m_RendererID));
        }
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

	void OpenGLImage2D::RT_Invalidate()
	{
	}

    void OpenGLImage2D::AllocateMemory(uint64_t size)
    {
    }

    void OpenGLImage2D::Release()
    {
        if (m_RendererID)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
        m_ImageData.Release();
    }

}