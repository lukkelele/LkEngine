#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLImage.h"


namespace LkEngine {

#if 0
    OpenGLImage::OpenGLImage(std::string_view stringView)
        : m_FilePath(stringView)
    {
        
    }
#endif

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


    void OpenGLImage::AllocateMemory(uint64_t size)
    {
    }

    void OpenGLImage::ReleaseMemory()
    {

    }

}