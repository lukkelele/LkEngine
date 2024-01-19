#include "LKpch.h"
#include "VulkanImage.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    VulkanImage::VulkanImage(ImageSpecification spec, const void* data)
        : m_Specification(spec)
        , m_Width(spec.Width)
        , m_Height(spec.Height)
    {
		if (data)
		{
            uint32_t memorySize = Image::GetMemorySize(spec.Format, spec.Width, spec.Height);
			LK_CORE_DEBUG("VulkanImage::Memory size: {}    texture: {}", memorySize, spec.Name);
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

    void VulkanImage::RT_Invalidate()
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

    void VulkanImage::UpdateDescriptor()
    {
		if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8 || m_Specification.Format == ImageFormat::DEPTH32F || m_Specification.Format == ImageFormat::DEPTH32FSTENCIL8UINT)
			m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		else if (m_Specification.Usage == ImageUsage::Storage)
			m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		else
			m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		if (m_Specification.Usage == ImageUsage::Storage)
			m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		else if (m_Specification.Usage == ImageUsage::HostRead)
			m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		m_DescriptorImageInfo.imageView = m_Info.ImageView;
		m_DescriptorImageInfo.sampler = m_Info.Sampler;
    }

	void VulkanImage::CreatePerLayerImageViews()
	{
		Ref<VulkanImage> instance = this;
		Renderer::Submit([instance]() mutable
		{
			instance->RT_CreatePerLayerImageViews();
		});
	}

	void VulkanImage::RT_CreatePerLayerImageViews()
	{
		LK_CORE_ASSERT(m_Specification.Layers > 1);

		VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

		VkImageAspectFlags aspectMask = Image::IsDepthFormat(m_Specification.Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8)
			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		const VkFormat vulkanFormat = VKUtils::VulkanImageFormat(m_Specification.Format);

		m_PerLayerImageViews.resize(m_Specification.Layers);
		for (uint32_t layer = 0; layer < m_Specification.Layers; layer++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = vulkanFormat;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.subresourceRange = {};
			imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = m_Specification.Mips;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = layer;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.image = m_Info.Image;
			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_PerLayerImageViews[layer]));
			VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_IMAGE_VIEW, fmt::format("{} image view layer: {}", m_Specification.DebugName, layer), m_PerLayerImageViews[layer]);
		}
	}

	void VulkanImage::RT_CreatePerSpecificLayerImageViews(const std::vector<uint32_t>& layerIndices)
	{
		LK_CORE_ASSERT(m_Specification.Layers > 1);

		VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

		VkImageAspectFlags aspectMask = Image::IsDepthFormat(m_Specification.Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8)
			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		const VkFormat vulkanFormat = VKUtils::VulkanImageFormat(m_Specification.Format);

		if (m_PerLayerImageViews.empty())
			m_PerLayerImageViews.resize(m_Specification.Layers);

		for (uint32_t layer : layerIndices)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = vulkanFormat;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.subresourceRange = {};
			imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = m_Specification.Mips;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = layer;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.image = m_Info.Image;
			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_PerLayerImageViews[layer]));
			VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_IMAGE_VIEW, fmt::format("{} image view layer: {}", m_Specification.DebugName, layer), m_PerLayerImageViews[layer]);
		}

	}

}

