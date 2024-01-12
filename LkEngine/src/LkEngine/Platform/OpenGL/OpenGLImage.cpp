#include "LKpch.h"
#include "OpenGLImage.h"

#include "LkOpenGL.h"


namespace LkEngine {

	GLenum OpenGLImageFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB:     return GL_RGB;
			case ImageFormat::RGBA:
			case ImageFormat::RGBA8:
			case ImageFormat::RGBA16F:
			case ImageFormat::RGBA32F: return GL_RGBA;
		}
		LK_ASSERT(false);
		return 0;
	}

	GLenum OpenGLImageInternalFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB:             return GL_RGB8;
			case ImageFormat::RGBA:            return GL_RGBA8;
			case ImageFormat::RGBA8:           return GL_RGBA8;
			case ImageFormat::RGBA16F:         return GL_RGBA16F;
			case ImageFormat::RGBA32F:         return GL_RGBA32F;
			case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			case ImageFormat::DEPTH32F:        return GL_DEPTH_COMPONENT32F;
		}
		LK_ASSERT(false);
		return 0;
	}

	GLenum OpenGLFormatDataType(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGB:
			case ImageFormat::RGBA:    
			case ImageFormat::RGBA8:   return GL_UNSIGNED_BYTE;
			case ImageFormat::RGBA16F:
			case ImageFormat::RGBA32F: return GL_FLOAT;
		}
		LK_ASSERT(false);
		return 0;
	}

	GLenum OpenGLSamplerWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
			case TextureWrap::Clamp:   return GL_CLAMP_TO_EDGE;
			case TextureWrap::Repeat:  return GL_REPEAT;
		}
		throw std::runtime_error("OpenGLSamplerWrap failed, TextureWrap::Clamp or TextureWrap::Repeat not detected");
	}

	GLenum OpenGLSamplerFilter(TextureFilter filter, bool mipmap)
	{
		switch (filter)
		{
			case TextureFilter::Linear:  return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
			case TextureFilter::Nearest: return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		}
		throw std::runtime_error("OpenGLSamplerFilter failed, TextureFilter::Linear or TextureFilter::Nearest not detected");
	}

    OpenGLImage::OpenGLImage(ImageSpecification spec, const void* data)
        : m_Specification(spec)
        , m_Width(spec.Width)
        , m_Height(spec.Height)
    {
		if (data)
		{
            uint32_t memorySize = Image::GetMemorySize(spec.Format, spec.Width, spec.Height);
			LOG_DEBUG("OpenGLImage::Memory size: {}    texture: {}", memorySize, spec.Name);
			m_ImageData = Buffer::Copy(data, memorySize);

			GL_CALL(glGenTextures(1, &m_RendererID));
			GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

			GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData.Data));
			GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

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

			if (spec.AnistropicFiltering == TextureAnistropicFiltering::Bilnear)
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
			}
			else if (spec.AnistropicFiltering == TextureAnistropicFiltering::Trilnear)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
			}

			GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		}
		else
		{
			throw std::runtime_error("OpenGLImage could not be created, no data detected");
		}
    }

    OpenGLImage::OpenGLImage(ImageSpecification spec, Buffer buffer)
		: OpenGLImage(spec, buffer.Data)
    {
    }

    OpenGLImage::~OpenGLImage()
    {
    }

	void OpenGLImage::SetData(const void* data)
    {
    }

    void OpenGLImage::Resize(uint32_t width, uint32_t height)
    {
    }

    int64_t OpenGLImage::GetImageFormat(ImageFormat fmt)
    {
        return Image::GetImageFormat(fmt);
    }

    void OpenGLImage::Invalidate()
    {
		if (m_RendererID)
		{
            Release();
		}
        GLenum internalFormat = OpenGLImageInternalFormat(m_Specification.Format);
        uint32_t mipCount = CalculateMipCount(m_Width, m_Height);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Width, m_Height);
        if (m_ImageData)
        {
            GLenum format = OpenGLImageFormat(m_Specification.Format);
            GLenum dataType = OpenGLFormatDataType(m_Specification.Format);
			LOG_WARN("glTextureSubImage2D | RendererID: {}", m_RendererID);
            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, dataType, m_ImageData.Data);
            //glGenerateTextureMipmap(m_RendererID); // TODO: optional
        }
		glBindTexture(GL_TEXTURE_2D, 0);
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

}