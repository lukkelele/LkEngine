#include "LKpch.h"
#include "LkVulkan.h"
#include "VulkanContext.h"
#include "VulkanShader.h"

#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Renderer/Pipeline.h"


namespace LkEngine {

	namespace Vulkan {

		VkDescriptorSetAllocateInfo DescriptorSetAllocInfo(const VkDescriptorSetLayout* layouts, uint32_t count, VkDescriptorPool pool)
		{
			VkDescriptorSetAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			info.pSetLayouts = layouts;
			info.descriptorSetCount = count;
			info.descriptorPool = pool;
			return info;
		}

		VkSampler CreateSampler(VkSamplerCreateInfo samplerCreateInfo)
		{
			auto device = VulkanContext::GetCurrentDevice();
			VkDevice vulkanDevice = device->GetVulkanDevice();

			VkSampler sampler;
			VK_CHECK_RESULT(vkCreateSampler(vulkanDevice, &samplerCreateInfo, nullptr, &sampler));

			Renderer::IncrementSamplerCount();

			return sampler;
		}

		void DestroySampler(VkSampler sampler)
		{
			auto device = VulkanContext::GetCurrentDevice();
			VkDevice vulkanDevice = device->GetVulkanDevice();
			vkDestroySampler(vulkanDevice, sampler, nullptr);

			Renderer::DecreaseSamplerCount();
		}

	}

    namespace VKUtils {

		void VulkanLoadDebugUtilsExtensions(VkInstance instance)
		{
			fpSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
			if (fpSetDebugUtilsObjectNameEXT == nullptr)
				fpSetDebugUtilsObjectNameEXT = [](VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo) { return VK_SUCCESS; };

			fpCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)(vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
			if (fpCmdBeginDebugUtilsLabelEXT == nullptr)
				fpCmdBeginDebugUtilsLabelEXT = [](VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) {};

			fpCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)(vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
			if (fpCmdEndDebugUtilsLabelEXT == nullptr)
				fpCmdEndDebugUtilsLabelEXT = [](VkCommandBuffer commandBuffer) {};

			fpCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)(vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
			if (fpCmdInsertDebugUtilsLabelEXT == nullptr)
				fpCmdInsertDebugUtilsLabelEXT = [](VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) {};
		}

		const char* VkDebugUtilsMessageType(const VkDebugUtilsMessageTypeFlagsEXT type)
		{
			switch (type)
			{
				case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:		return "General";
				case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:	return "Validation";
				case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:	return "Performance";
				default:												return "Unknown";
			}
		}
	
		const char* VkDebugUtilsMessageSeverity(const VkDebugUtilsMessageSeverityFlagBitsEXT severity)
		{
			switch (severity)
			{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:		return "error";
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	return "warning";
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		return "info";
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:	return "verbose";
				default:												return "unknown";
			}
		}

		VkPrimitiveTopology GetVulkanTopology(PrimitiveTopology topology)
		{
			switch (topology)
			{
				case PrimitiveTopology::Points:			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
				case PrimitiveTopology::Lines:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
				case PrimitiveTopology::Triangles:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				case PrimitiveTopology::LineStrip:		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
				case PrimitiveTopology::TriangleStrip:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
				case PrimitiveTopology::TriangleFan:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			}

			LK_CORE_ASSERT(false, "Unknown toplogy");
			return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
		}

		VkCompareOp GetVulkanCompareOperator(const DepthCompareOperator compareOp)
		{
			switch (compareOp)
			{
				case DepthCompareOperator::Never:			return VK_COMPARE_OP_NEVER;
				case DepthCompareOperator::NotEqual:		return VK_COMPARE_OP_NOT_EQUAL;
				case DepthCompareOperator::Less:			return VK_COMPARE_OP_LESS;
				case DepthCompareOperator::LessOrEqual:		return VK_COMPARE_OP_LESS_OR_EQUAL;
				case DepthCompareOperator::Greater:			return VK_COMPARE_OP_GREATER;
				case DepthCompareOperator::GreaterOrEqual:	return VK_COMPARE_OP_GREATER_OR_EQUAL;
				case DepthCompareOperator::Equal:			return VK_COMPARE_OP_EQUAL;
				case DepthCompareOperator::Always:			return VK_COMPARE_OP_ALWAYS;
			}
			LK_CORE_ASSERT(false, "Unknown Operator");
			return VK_COMPARE_OP_MAX_ENUM;
		}


		static VkFormat ShaderDataTypeToVulkanFormat(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:     return VK_FORMAT_R32_SFLOAT;
				case ShaderDataType::Float2:    return VK_FORMAT_R32G32_SFLOAT;
				case ShaderDataType::Float3:    return VK_FORMAT_R32G32B32_SFLOAT;
				case ShaderDataType::Float4:    return VK_FORMAT_R32G32B32A32_SFLOAT;
				case ShaderDataType::Int:       return VK_FORMAT_R32_SINT;
				case ShaderDataType::Int2:      return VK_FORMAT_R32G32_SINT;
				case ShaderDataType::Int3:      return VK_FORMAT_R32G32B32_SINT;
				case ShaderDataType::Int4:      return VK_FORMAT_R32G32B32A32_SINT;
			}
			LK_CORE_ASSERT(false);
			return VK_FORMAT_UNDEFINED;
		}



		void InsertImageMemoryBarrier(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresourceRange)
		{
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			imageMemoryBarrier.srcAccessMask = srcAccessMask;
			imageMemoryBarrier.dstAccessMask = dstAccessMask;
			imageMemoryBarrier.oldLayout = oldImageLayout;
			imageMemoryBarrier.newLayout = newImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;

			vkCmdPipelineBarrier(
				cmdbuffer,
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);
		}

		void SetImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask)
		{
			// Create an image barrier object
			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = oldImageLayout;
			imageMemoryBarrier.newLayout = newImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;

			// Source layouts (old)
			// Source access mask controls actions that have to be finished on the old layout
			// before it will be transitioned to the new layout
			switch (oldImageLayout)
			{
				case VK_IMAGE_LAYOUT_UNDEFINED:
					// Image layout is undefined (or does not matter)
					// Only valid as initial layout
					// No flags required, listed only for completeness
					imageMemoryBarrier.srcAccessMask = 0;
					break;

				case VK_IMAGE_LAYOUT_PREINITIALIZED:
					// Image is preinitialized
					// Only valid as initial layout for linear images, preserves memory contents
					// Make sure host writes have been finished
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					// Image is a color attachment
					// Make sure any writes to the color buffer have been finished
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					// Image is a depth/stencil attachment
					// Make sure any writes to the depth/stencil buffer have been finished
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					// Image is a transfer source
					// Make sure any reads from the image have been finished
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					break;

				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					// Image is a transfer destination
					// Make sure any writes to the image have been finished
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					// Image is read by a shader
					// Make sure any shader reads from the image have been finished
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
					break;
				default:
					// Other source layouts aren't handled (yet)
					break;
			}

			// Target layouts (new)
			// Destination access mask controls the dependency for the new image layout
			switch (newImageLayout)
			{
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					// Image will be used as a transfer destination
					// Make sure any writes to the image have been finished
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					// Image will be used as a transfer source
					// Make sure any reads from the image have been finished
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					break;

				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					// Image will be used as a color attachment
					// Make sure any writes to the color buffer have been finished
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					// Image layout will be used as a depth/stencil attachment
					// Make sure any writes to depth/stencil buffer have been finished
					imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					break;

				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					// Image will be read in a shader (sampler, input attachment)
					// Make sure any writes to the image have been finished
					if (imageMemoryBarrier.srcAccessMask == 0)
					{
						imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
					}
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					break;
				default:
					// Other source layouts aren't handled (yet)
					break;
			}

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(
				cmdbuffer,
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);
		}

		void SetImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask)
		{
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = aspectMask;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			SetImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
		}

		// Declared in Shader.h
		constexpr std::string_view UniformTypeToString(const ShaderUniformType type)
		{
			if (type == ShaderUniformType::Bool)
			{
				return std::string("Boolean");
			}
			else if (type == ShaderUniformType::Int)
			{
				return std::string("Int");
			}
			else if (type == ShaderUniformType::Float)
			{
				return std::string("Float");
			}
	
			return std::string("None");
		}

		inline VkFormat VulkanImageFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RED8UN:               return VK_FORMAT_R8_UNORM;
				case ImageFormat::RED8UI:               return VK_FORMAT_R8_UINT;
				case ImageFormat::RED16UI:              return VK_FORMAT_R16_UINT;
				case ImageFormat::RED32UI:              return VK_FORMAT_R32_UINT;
				case ImageFormat::RED32F:               return VK_FORMAT_R32_SFLOAT;
				case ImageFormat::RG8:                  return VK_FORMAT_R8G8_UNORM;
				case ImageFormat::RG16F:                return VK_FORMAT_R16G16_SFLOAT;
				case ImageFormat::RG32F:                return VK_FORMAT_R32G32_SFLOAT;
				case ImageFormat::RGBA:                 return VK_FORMAT_R8G8B8A8_UNORM;
				case ImageFormat::SRGBA:                return VK_FORMAT_R8G8B8A8_SRGB;
				case ImageFormat::RGBA16F:              return VK_FORMAT_R16G16B16A16_SFLOAT;
				case ImageFormat::RGBA32F:              return VK_FORMAT_R32G32B32A32_SFLOAT;
				case ImageFormat::B10R11G11UF:          return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
				case ImageFormat::DEPTH32FSTENCIL8UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
				case ImageFormat::DEPTH32F:             return VK_FORMAT_D32_SFLOAT;
				case ImageFormat::DEPTH24STENCIL8:      return VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetDepthFormat();
			}
			LK_CORE_ASSERT(false);
			return VK_FORMAT_UNDEFINED;
		}

    }

}