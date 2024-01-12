#include "LKpch.h"
#include "VulkanImage.h"


namespace LkEngine {

    VulkanImage::VulkanImage(ImageSpecification spec, const void* data)
        : m_Specification(spec)
        , m_Width(spec.Width)
        , m_Height(spec.Height)
    {
		if (data)
		{
            uint32_t memorySize = Image::GetMemorySize(spec.Format, spec.Width, spec.Height);
			LOG_DEBUG("VulkanImage::Memory size: {}    texture: {}", memorySize, spec.Name);
			m_ImageData = Buffer::Copy(data, memorySize);

			// Texture Wrap
			if (m_Specification.Wrap == TextureWrap::Clamp)
			{

			}
			else if (m_Specification.Wrap == TextureWrap::Repeat)
			{
			}

			// Texture Filter
			if (m_Specification.Filter == TextureFilter::Linear)
			{
			}
			else if (m_Specification.Filter == TextureFilter::Nearest)
			{
			}

			// Anistropic Filtering
			if (spec.AnistropicFiltering == TextureAnistropicFiltering::Bilnear)
			{
			}
			else if (spec.AnistropicFiltering == TextureAnistropicFiltering::Trilnear)
			{
			}
		}
		else
		{
			throw std::runtime_error("VulkanImage could not be created, no data detected");
		}
    }

    VulkanImage::VulkanImage(ImageSpecification spec, Buffer buffer)
		: VulkanImage(spec, buffer.Data)
    {
    }

    VulkanImage::~VulkanImage()
    {
    }

	void VulkanImage::SetData(const void* data)
    {
    }

    void VulkanImage::Resize(uint32_t width, uint32_t height)
    {
    }

    int64_t VulkanImage::GetImageFormat(ImageFormat fmt)
    {
        return Image::GetImageFormat(fmt);
    }

    void VulkanImage::Invalidate()
    {
		if (m_RendererID)
		{
            Release();
		}
        uint32_t mipCount = CalculateMipCount(m_Width, m_Height);

        if (m_ImageData)
        {
        }
    }

    void VulkanImage::AllocateMemory(uint64_t size)
    {
    }

    void VulkanImage::Release()
    {
        if (m_RendererID)
        {
            m_RendererID = 0;
        }
        m_ImageData.Release();
    }

}

