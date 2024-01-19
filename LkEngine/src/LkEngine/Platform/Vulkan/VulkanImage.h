#pragma once

#include "LkEngine/Renderer/Image.h"

#include "VulkanDescriptorSetManager.h"
#include "VulkanMemoryAllocator/vk_mem_alloc.h"
#include "VulkanResourceDescriptorInfo.h"

#include "VulkanContext.h"


namespace LkEngine {

	struct VulkanImageInfo
	{
		VkImage Image = nullptr;
		VkImageView ImageView = nullptr;
		VkSampler Sampler = nullptr;
		VmaAllocation MemoryAlloc = nullptr;
	};

    class VulkanImage : public Image
    {
	public:
		VulkanImage(ImageSpecification spec, Buffer buffer);
		VulkanImage(ImageSpecification spec, const void* data = nullptr);
		~VulkanImage();

		void Invalidate() override;
		void RT_Invalidate() override;

		void Resize(uint32_t width, uint32_t height) override;
		void SetData(const void* data) override;

		RendererID& GetRendererID() { return m_RendererID; }
		RendererID GetRendererID() const { return m_RendererID; }

		Buffer GetBuffer() const override { return m_ImageData; }
		Buffer& GetBuffer() override { return m_ImageData; }	

		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }

		const ImageSpecification GetSpecification() const { return m_Specification; }
		ImageSpecification& GetSpecification() { return m_Specification; }

		int64_t GetImageFormat(ImageFormat fmt) override;
		VulkanImageInfo GetImageInfo() const { return m_Info; }
		VulkanImageInfo& GetImageInfo() { return m_Info; }

		void CreatePerLayerImageViews();
		void RT_CreatePerLayerImageViews();
		void RT_CreatePerSpecificLayerImageViews(const std::vector<uint32_t>& layerIndices);

		virtual VkImageView GetLayerImageView(uint32_t layer)
		{
			LK_CORE_ASSERT(layer < m_PerLayerImageViews.size());
			return m_PerLayerImageViews[layer];
		}

		virtual ResourceDescriptorInfo GetDescriptorInfo() const { return (ResourceDescriptorInfo)&m_DescriptorImageInfo; }
		VkDescriptorImageInfo& GetDescriptorInfoVulkan() const { return *(VkDescriptorImageInfo*)GetDescriptorInfo(); }

		void Release();
		void AllocateMemory(uint64_t size);

		void UpdateDescriptor();

	private:
		RendererID m_RendererID = 0;
		Buffer m_ImageData;
		uint32_t m_Width, m_Height;

		VulkanImageInfo m_Info;
		ImageSpecification m_Specification;
		VkDescriptorImageInfo m_DescriptorImageInfo = {};
		std::vector<VkImageView> m_PerLayerImageViews;

		VkDeviceSize m_GPUAllocationSize;

		std::string m_FilePath;
		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};

}