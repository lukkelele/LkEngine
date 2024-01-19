#include "LKpch.h"
#include "VulkanAllocator.h"
#include "VulkanContext.h"

#include "LkEngine/Utilities/StringUtils.h"


namespace LkEngine {

	struct VulkanAllocatorData
	{
		VmaAllocator Allocator;
		uint64_t TotalAllocatedBytes = 0;
		
		uint64_t MemoryUsage = 0; // all heaps
	};

	enum class AllocationType : uint8_t
	{
		None = 0, Buffer = 1, Image = 2
	};

	static VulkanAllocatorData* Data = nullptr;

	struct AllocInfo
	{
		uint64_t AllocatedSize = 0;
		AllocationType Type = AllocationType::None;
	};
	static std::map<VmaAllocation, AllocInfo> s_AllocationMap;

	VulkanAllocator::VulkanAllocator(const std::string& tag)
		: m_Tag(tag)
	{
	}

	VulkanAllocator::~VulkanAllocator()
	{
	}

	void VulkanAllocator::Allocate(VkMemoryRequirements requirements, VkDeviceMemory* dest, VkMemoryPropertyFlags flags /*= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT*/)
	{
		LK_CORE_ASSERT(m_Device);

		LK_CORE_TRACE("VulkanAllocator ({0}): allocating {1}", m_Tag, Utils::String::BytesToString(requirements.size));
		{
			static uint64_t totalAllocatedBytes = 0;
			totalAllocatedBytes += requirements.size;
			LK_CORE_TRACE("VulkanAllocator ({0}): total allocated since start is {1}", m_Tag, Utils::String::BytesToString(totalAllocatedBytes));
		}

		VkMemoryAllocateInfo memAlloc = {};
		memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAlloc.allocationSize = requirements.size;
		memAlloc.memoryTypeIndex = m_Device->GetPhysicalDevice()->GetMemoryTypeIndex(requirements.memoryTypeBits, flags);
		VK_CHECK_RESULT(vkAllocateMemory(m_Device->GetVulkanDevice(), &memAlloc, nullptr, dest));
	}

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage, VkBuffer& outBuffer)
	{
		LK_CORE_VERIFY(bufferCreateInfo.size > 0);

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = usage;

		VmaAllocation allocation;
		vmaCreateBuffer(Data->Allocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &allocation, nullptr);
		if (allocation == nullptr)
		{
			LK_CORE_ERROR_TAG("Renderer", "Failed to allocate GPU buffer!");
			LK_CORE_ERROR_TAG("Renderer", "  Requested size: {}", Utils::String::BytesToString(bufferCreateInfo.size));
			//auto stats = GetStats();
			//LK_CORE_ERROR_TAG("Renderer", "  GPU mem usage: {}/{}", Utils::String::BytesToString(stats.Used), Utils::String::BytesToString(stats.TotalAvailable));
		}

		VmaAllocationInfo allocInfo{};
		vmaGetAllocationInfo(Data->Allocator, allocation, &allocInfo);
		LK_CORE_TRACE_TAG("VulkanAllocator ({0}): allocating buffer; size = {1}", m_Tag, Utils::String::BytesToString(allocInfo.size));
		{
			Data->TotalAllocatedBytes += allocInfo.size;
			//LK_ALLOCATOR_LOG("VulkanAllocator ({0}): total allocated since start is {1}", m_Tag, Utils::String::BytesToString(Data->TotalAllocatedBytes));
		}

		return allocation;
	}

	VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* allocatedSize)
	{
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = usage;

		VmaAllocation allocation;
		vmaCreateImage(Data->Allocator, &imageCreateInfo, &allocCreateInfo, &outImage, &allocation, nullptr);
		if (allocation == nullptr)
		{
			LK_CORE_ERROR_TAG("Renderer", "Failed to allocate GPU image!");
			LK_CORE_ERROR_TAG("Renderer", "  Requested size: {}x{}x{}", imageCreateInfo.extent.width, imageCreateInfo.extent.height, imageCreateInfo.extent.depth);
			LK_CORE_ERROR_TAG("Renderer", "  Mips: {}", imageCreateInfo.mipLevels);
			LK_CORE_ERROR_TAG("Renderer", "  Layers: {}", imageCreateInfo.arrayLayers);
			//auto stats = GetStats();
		}

		VmaAllocationInfo allocInfo;
		vmaGetAllocationInfo(Data->Allocator, allocation, &allocInfo);
		if (allocatedSize)
			*allocatedSize = allocInfo.size;
		{
			Data->TotalAllocatedBytes += allocInfo.size;
		}

	#if LK_GPU_TRACK_MEMORY_ALLOCATION
		auto& allocTrack = s_AllocationMap[allocation];
		allocTrack.AllocatedSize = allocInfo.size;
		allocTrack.Type = AllocationType::Image;
		Data->MemoryUsage += allocInfo.size;
	#endif

		return allocation;
	}

	void VulkanAllocator::Free(VmaAllocation allocation)
	{
		vmaFreeMemory(Data->Allocator, allocation);

#if LK_GPU_TRACK_MEMORY_ALLOCATION
		auto it = s_AllocationMap.find(allocation);
		if (it != s_AllocationMap.end())
		{
			Data->MemoryUsage -= it->second.AllocatedSize;
			s_AllocationMap.erase(it);
		}
		else
		{
			LK_CORE_ERROR("Could not find GPU memory allocation: {}", (void*)allocation);
		}
#endif
	}

	void VulkanAllocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		LK_CORE_ASSERT(image);
		LK_CORE_ASSERT(allocation);
		vmaDestroyImage(Data->Allocator, image, allocation);

#if LK_GPU_TRACK_MEMORY_ALLOCATION
		auto it = s_AllocationMap.find(allocation);
		if (it != s_AllocationMap.end())
		{
			Data->MemoryUsage -= it->second.AllocatedSize;
			s_AllocationMap.erase(it);
		}
		else
		{
			LK_CORE_ERROR("Could not find GPU memory allocation: {}", (void*)allocation);
		}
#endif
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		LK_CORE_ASSERT(buffer);
		LK_CORE_ASSERT(allocation);
		vmaDestroyBuffer(Data->Allocator, buffer, allocation);

	#if LK_GPU_TRACK_MEMORY_ALLOCATION
		auto it = s_AllocationMap.find(allocation);
		if (it != s_AllocationMap.end())
		{
			Data->MemoryUsage -= it->second.AllocatedSize;
			s_AllocationMap.erase(it);
		}
		else
		{
			LK_CORE_ERROR("Could not find GPU memory allocation: {}", (void*)allocation);
		}
	#endif
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(Data->Allocator, allocation);
	}

	void VulkanAllocator::DumpStats()
	{
		const auto& memoryProps = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetMemoryProperties();
		std::vector<VmaBudget> budgets(memoryProps.memoryHeapCount);
		vmaGetBudget(Data->Allocator, budgets.data());

		LK_CORE_WARN("-----------------------------------");
		for (VmaBudget& b : budgets)
		{
			LK_CORE_WARN("VmaBudget.allocationBytes = {0}", Utils::String::BytesToString(b.allocationBytes));
			LK_CORE_WARN("VmaBudget.blockBytes = {0}", Utils::String::BytesToString(b.blockBytes));
			LK_CORE_WARN("VmaBudget.usage = {0}", Utils::String::BytesToString(b.usage));
			LK_CORE_WARN("VmaBudget.budget = {0}", Utils::String::BytesToString(b.budget));
		}
		LK_CORE_WARN("-----------------------------------");
	}

	GPUMemoryStats VulkanAllocator::GetStats()
	{
		const auto& memoryProps = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetMemoryProperties();
		std::vector<VmaBudget> budgets(memoryProps.memoryHeapCount);
		vmaGetBudget(Data->Allocator, budgets.data());

		uint64_t budget = 0;
		for (VmaBudget& b : budgets)
			budget += b.budget;

		GPUMemoryStats result;
		for (const auto& [k, v] : s_AllocationMap)
		{
			if (v.Type == AllocationType::Buffer)
			{
				result.BufferAllocationCount++;
				result.BufferAllocationSize += v.AllocatedSize;
			}
			else if (v.Type == AllocationType::Image)
			{
				result.ImageAllocationCount++;
				result.ImageAllocationSize += v.AllocatedSize;
			}
		}

		result.AllocationCount = s_AllocationMap.size();
		result.Used = Data->MemoryUsage;
		result.TotalAvailable = budget;
		return result;
#if 0
		VmaStats stats;
		vmaCalculateStats(Data->Allocator, &stats);

		uint64_t usedMemory = stats.total.usedBytes;
		uint64_t freeMemory = stats.total.unusedBytes;

		return { usedMemory, freeMemory };
#endif
	}

	void VulkanAllocator::Init(Ref<VulkanDevice> device)
	{
		Data = new VulkanAllocatorData();

		// Initialize VulkanMemoryAllocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
		allocatorInfo.physicalDevice = device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		allocatorInfo.device = device->GetVulkanDevice();
		allocatorInfo.instance = VulkanContext::GetInstance();

		vmaCreateAllocator(&allocatorInfo, &Data->Allocator);
	}

	void VulkanAllocator::Shutdown()
	{
		vmaDestroyAllocator(Data->Allocator);

		delete Data;
		Data = nullptr;
	}

	VmaAllocator& VulkanAllocator::GetVMAAllocator()
	{
		return Data->Allocator;
	}

}
