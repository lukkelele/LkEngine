#pragma once

#include "LkVulkan.h"
#include "VulkanUniformBuffer.h"

#include <unordered_set>


namespace LkEngine {

	// Forward declaration
	class VulkanCommandPool;

	class VulkanPhysicalDevice : public RefCounted
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;
		bool IsExtensionSupported(const std::string& extensionName) const;

		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		const VkPhysicalDeviceProperties& GetProperties() const { return m_Properties; }
		const VkPhysicalDeviceLimits& GetLimits() const { return m_Properties.limits; }
		const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return m_MemoryProperties; }

		VkFormat GetDepthFormat() const { return m_DepthFormat; }

		static Ref<VulkanPhysicalDevice> Select();

	private:
		VkFormat FindDepthFormat() const;
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);

	private:
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;

		QueueFamilyIndices m_QueueFamilyIndices;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
		std::unordered_set<std::string> m_SupportedExtensions;

		friend class VulkanDevice;
	};


	class VulkanDevice : public RefCounted
	{
	public:
		VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanDevice();

		void Destroy();

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetComputeQueue() { return m_ComputeQueue; }

		VkCommandBuffer GetCommandBuffer(bool begin, bool compute = false);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);

		VkCommandBuffer CreateSecondaryCommandBuffer(const char* debugName);

		VkDevice GetVulkanDevice() const { return m_LogicalDevice; }
		const Ref<VulkanPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }

	private:
		Ref<VulkanCommandPool> GetThreadLocalCommandPool();
		Ref<VulkanCommandPool> GetOrCreateThreadLocalCommandPool();

	private:
		VkDevice m_LogicalDevice = nullptr;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		VkPhysicalDeviceFeatures m_EnabledFeatures;

		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;

		std::map<std::thread::id, Ref<VulkanCommandPool>> m_CommandPools;

		bool m_EnableDebugMarkers = false;
	};

}