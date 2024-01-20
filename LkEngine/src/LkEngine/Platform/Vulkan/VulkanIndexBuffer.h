#pragma once

#include "LkEngine/Renderer/IndexBuffer.h"

#include "LkVulkan.h"
#include "VulkanAllocator.h"


namespace LkEngine {

    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
		VulkanIndexBuffer(uint64_t size);
		VulkanIndexBuffer(void* data, uint64_t size = 0);
		~VulkanIndexBuffer();

		void SetData(void* buffer, uint64_t size, uint64_t offset = 0) override;

		uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }
		uint64_t GetSize() const override { return m_Size; }

		VkBuffer GetVulkanBuffer() { return m_VulkanBuffer; }
		Buffer GetLocalData() { return m_LocalData; }

		void Bind() const override;

	private:
		uint64_t m_Size = 0;
		Buffer m_LocalData;

		VkBuffer m_VulkanBuffer = nullptr;
		VmaAllocation m_MemoryAllocation;
    };

}