#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLImage.h"


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
		LK_ASSERT(false);
		return 0;
	}

	GLenum OpenGLSamplerFilter(TextureFilter filter, bool mipmap)
	{
		switch (filter)
		{
			case TextureFilter::Linear:   return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
			case TextureFilter::Nearest:  return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		}
		LK_ASSERT(false);
		return 0;
	}


    OpenGLImage::OpenGLImage(ImageSpecification spec, Buffer buffer)
        : m_Specification(spec)
        , m_Width(spec.Width)
        , m_Height(spec.Height)
        , m_ImageData(buffer)
    {
    }

    OpenGLImage::OpenGLImage(ImageSpecification spec, const void* data)
        : m_Specification(spec)
        , m_Width(spec.Width)
        , m_Height(spec.Height)
    {
        if (data)
            m_ImageData = Buffer::Copy(data, Image::GetMemorySize(spec.Format, spec.Width, spec.Height));
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
            Release();

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

        GLenum internalFormat = OpenGLImageInternalFormat(m_Specification.Format);
        uint32_t mipCount = CalculateMipCount(m_Width, m_Height);
        glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Width, m_Height);
        if (m_ImageData)
        {
            GLenum format = OpenGLImageFormat(m_Specification.Format);
            GLenum dataType = OpenGLFormatDataType(m_Specification.Format);
            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, dataType, m_ImageData.Data);
            glGenerateTextureMipmap(m_RendererID); // TODO: optional
        }
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