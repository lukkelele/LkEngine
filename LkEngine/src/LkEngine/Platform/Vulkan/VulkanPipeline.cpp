#include "LKpch.h"
#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanFramebuffer.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& specification)
		: m_Specification(specification)
	{
		m_Specification.RenderAPI = RendererAPIType::Vulkan;
		LK_CORE_ASSERT(specification.Shader);
		LK_CORE_ASSERT(specification.TargetFramebuffer);
		Invalidate();
		//Renderer::RegisterShaderDependency(specification.Shader, this);
	}

	VulkanPipeline::~VulkanPipeline()
	{
#if 0
		Renderer::SubmitResourceFree([pipeline = m_VulkanPipeline, pipelineCache = m_PipelineCache, pipelineLayout = m_PipelineLayout]()
		{
			const auto vulkanDevice = VulkanContext::GetCurrentDevice()->GetVulkanDevice();
			vkDestroyPipeline(vulkanDevice, pipeline, nullptr);
			vkDestroyPipelineCache(vulkanDevice, pipelineCache, nullptr);
			vkDestroyPipelineLayout(vulkanDevice, pipelineLayout, nullptr);
		});
#endif
	}

	void VulkanPipeline::Invalidate()
	{
#if 0
		Ref<VulkanPipeline> instance = this;
		Renderer::Submit([instance]() mutable
		{
			// LK_CORE_WARN("[VulkanPipeline] Creating pipeline {0}", instance->m_Specification.DebugName);

			VkDevice device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();
			LK_CORE_ASSERT(instance->m_Specification.Shader);
			Ref<VulkanShader> vulkanShader = Ref<VulkanShader>(instance->m_Specification.Shader);
			Ref<VulkanFramebuffer> framebuffer = instance->m_Specification.TargetFramebuffer.As<VulkanFramebuffer>();

			auto descriptorSetLayouts = vulkanShader->GetAllDescriptorSetLayouts();

			const auto& pushConstantRanges = vulkanShader->GetPushConstantRanges();

			// TODO: should come from shader
			std::vector<VkPushConstantRange> vulkanPushConstantRanges(pushConstantRanges.size());
			for (uint32_t i = 0; i < pushConstantRanges.size(); i++)
			{
				const auto& pushConstantRange = pushConstantRanges[i];
				auto& vulkanPushConstantRange = vulkanPushConstantRanges[i];

				vulkanPushConstantRange.stageFlags = pushConstantRange.ShaderStage;
				vulkanPushConstantRange.offset = pushConstantRange.Offset;
				vulkanPushConstantRange.size = pushConstantRange.Size;
			}

			// Create the pipeline layout that is used to generate the rendering pipelines that are based on this descriptor set layout
			// In a more complex scenario you would have different pipeline layouts for different descriptor set layouts that could be reused
			VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
			pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pPipelineLayoutCreateInfo.pNext = nullptr;
			pPipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
			pPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
			pPipelineLayoutCreateInfo.pushConstantRangeCount = (uint32_t)vulkanPushConstantRanges.size();
			pPipelineLayoutCreateInfo.pPushConstantRanges = vulkanPushConstantRanges.data();

			VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &instance->m_PipelineLayout));

			// Create the graphics pipeline used in this example
			// Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
			// A pipeline is then stored and hashed on the GPU making pipeline changes very fast
			// Note: There are still a few dynamic states that are not directly part of the pipeline (but the info that they are used is)

			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			// The layout used for this pipeline (can be shared among multiple pipelines using the same layout)
			pipelineCreateInfo.layout = instance->m_PipelineLayout;
			// Renderpass this pipeline is attached to
			pipelineCreateInfo.renderPass = framebuffer->GetRenderPass();

			// Construct the different states making up the pipeline

			// Input assembly state describes how primitives are assembled
			// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
			inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyState.topology = VKUtils::GetVulkanTopology(instance->m_Specification.Topology);

			// Rasterization state
			VkPipelineRasterizationStateCreateInfo rasterizationState = {};
			rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationState.polygonMode = instance->m_Specification.Wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
			rasterizationState.cullMode = instance->m_Specification.BackfaceCulling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
			rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizationState.depthClampEnable = VK_FALSE;
			rasterizationState.rasterizerDiscardEnable = VK_FALSE;
			rasterizationState.depthBiasEnable = VK_FALSE;
			rasterizationState.lineWidth = instance->m_Specification.LineWidth; // this is dynamic

			// Color blend state describes how blend factors are calculated (if used)
			// We need one blend attachment state per color attachment (even if blending is not used)
			size_t colorAttachmentCount = framebuffer->GetSpecification().SwapChainTarget ? 1 : framebuffer->GetColorAttachmentCount();
			std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates(colorAttachmentCount);
			if (framebuffer->GetSpecification().SwapChainTarget)
			{
				blendAttachmentStates[0].colorWriteMask = 0xf;
				blendAttachmentStates[0].blendEnable = VK_TRUE;
				blendAttachmentStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				blendAttachmentStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachmentStates[0].colorBlendOp = VK_BLEND_OP_ADD;
				blendAttachmentStates[0].alphaBlendOp = VK_BLEND_OP_ADD;
				blendAttachmentStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				blendAttachmentStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			}
			else
			{
				for (size_t i = 0; i < colorAttachmentCount; i++)
				{
					if (!framebuffer->GetSpecification().Blend)
						break;

					blendAttachmentStates[i].colorWriteMask = 0xf;
					if (!framebuffer->GetSpecification().Blend)
						break;

					const auto& attachmentSpec = framebuffer->GetSpecification().Attachments.Attachments[i];
					FramebufferBlendMode blendMode = framebuffer->GetSpecification().BlendMode == FramebufferBlendMode::None
						? attachmentSpec.BlendMode
						: framebuffer->GetSpecification().BlendMode;

					blendAttachmentStates[i].blendEnable = attachmentSpec.Blend ? VK_TRUE : VK_FALSE;

					blendAttachmentStates[i].colorBlendOp = VK_BLEND_OP_ADD;
					blendAttachmentStates[i].alphaBlendOp = VK_BLEND_OP_ADD;
					blendAttachmentStates[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
					blendAttachmentStates[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

					switch (blendMode)
					{
						case FramebufferBlendMode::SrcAlphaOneMinusSrcAlpha:
							blendAttachmentStates[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
							blendAttachmentStates[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
							blendAttachmentStates[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
							blendAttachmentStates[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
							break;
						case FramebufferBlendMode::OneZero:
							blendAttachmentStates[i].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
							blendAttachmentStates[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
							break;
						case FramebufferBlendMode::Zero_SrcColor:
							blendAttachmentStates[i].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
							blendAttachmentStates[i].dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
							break;

						default:
							LK_CORE_VERIFY(false);
					}
				}
			}

			VkPipelineColorBlendStateCreateInfo colorBlendState = {};
			colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendState.attachmentCount = (uint32_t)blendAttachmentStates.size();
			colorBlendState.pAttachments = blendAttachmentStates.data();

			// Viewport state sets the number of viewports and scissor used in this pipeline
			// Note: This is actually overriden by the dynamic states (see below)
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			// Enable dynamic states
			// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
			// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
			// For this example we will set the viewport and scissor using dynamic states
			std::vector<VkDynamicState> dynamicStateEnables;
			dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
			dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
			if (instance->IsDynamicLineWidth())
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);

			VkPipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.pDynamicStates = dynamicStateEnables.data();
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

			// Depth and stencil state containing depth and stencil compare and test operations
			// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
			VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
			depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilState.depthTestEnable = instance->m_Specification.DepthTest ? VK_TRUE : VK_FALSE;
			depthStencilState.depthWriteEnable = instance->m_Specification.DepthWrite ? VK_TRUE : VK_FALSE;
			depthStencilState.depthCompareOp = VKUtils::GetVulkanCompareOperator(instance->m_Specification.DepthOperator);
			depthStencilState.depthBoundsTestEnable = VK_FALSE;
			depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
			depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
			depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilState.stencilTestEnable = VK_FALSE;
			depthStencilState.front = depthStencilState.back;

			// Multi sampling state
			VkPipelineMultisampleStateCreateInfo multisampleState = {};
			multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleState.pSampleMask = nullptr;

			// Vertex input descriptor
			VertexBufferLayout& vertexLayout = instance->m_Specification.Layout;
			VertexBufferLayout& instanceLayout = instance->m_Specification.InstanceLayout;
			VertexBufferLayout& boneInfluenceLayout = instance->m_Specification.BoneInfluenceLayout;

			std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;

			VkVertexInputBindingDescription& vertexInputBinding = vertexInputBindingDescriptions.emplace_back();
			vertexInputBinding.binding = 0;
			vertexInputBinding.stride = vertexLayout.GetStride();
			vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			if (instanceLayout.GetElementCount())
			{
				VkVertexInputBindingDescription& instanceInputBinding = vertexInputBindingDescriptions.emplace_back();
				instanceInputBinding.binding = 1;
				instanceInputBinding.stride = instanceLayout.GetStride();
				instanceInputBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
			}

			if (boneInfluenceLayout.GetElementCount())
			{
				VkVertexInputBindingDescription& boneInfluenceInputBinding = vertexInputBindingDescriptions.emplace_back();
				boneInfluenceInputBinding.binding = 2;
				boneInfluenceInputBinding.stride = boneInfluenceLayout.GetStride();
				boneInfluenceInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			}

			// Input attribute bindings describe shader attribute locations and memory layouts
			std::vector<VkVertexInputAttributeDescription> vertexInputAttributes(vertexLayout.GetElementCount() + instanceLayout.GetElementCount() + boneInfluenceLayout.GetElementCount());

			uint32_t binding = 0;
			uint32_t location = 0;
			for (const auto& layout : { vertexLayout, instanceLayout, boneInfluenceLayout })
			{
				for (const auto& element : layout)
				{
					vertexInputAttributes[location].binding = binding;
					vertexInputAttributes[location].location = location;
					vertexInputAttributes[location].format = VKUtils::ShaderDataTypeToVulkanFormat(element.Type);
					vertexInputAttributes[location].offset = element.Offset;
					location++;
				}
				binding++;
			}

			// Vertex input state used for pipeline creation
			VkPipelineVertexInputStateCreateInfo vertexInputState = {};
			vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputState.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescriptions.size();
			vertexInputState.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
			vertexInputState.vertexAttributeDescriptionCount = (uint32_t)vertexInputAttributes.size();
			vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

			const auto& shaderStages = vulkanShader->GetPipelineShaderStageCreateInfos();

			// Set pipeline shader stage info
			pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineCreateInfo.pStages = shaderStages.data();

			// Assign the pipeline states to the pipeline creation info structure
			pipelineCreateInfo.pVertexInputState = &vertexInputState;
			pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
			pipelineCreateInfo.pRasterizationState = &rasterizationState;
			pipelineCreateInfo.pColorBlendState = &colorBlendState;
			pipelineCreateInfo.pMultisampleState = &multisampleState;
			pipelineCreateInfo.pViewportState = &viewportState;
			pipelineCreateInfo.pDepthStencilState = &depthStencilState;
			pipelineCreateInfo.renderPass = framebuffer->GetRenderPass();
			pipelineCreateInfo.pDynamicState = &dynamicState;

			// What is this pipeline cache?
			VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
			pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &instance->m_PipelineCache));

			// Create rendering pipeline using the specified states
			VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, instance->m_PipelineCache, 1, &pipelineCreateInfo, nullptr, &instance->m_VulkanPipeline));
			VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_PIPELINE, instance->m_Specification.DebugName, instance->m_VulkanPipeline);
		});
#endif
	}

	bool VulkanPipeline::IsDynamicLineWidth() const
	{
		return m_Specification.Topology == PrimitiveTopology::Lines || m_Specification.Topology == PrimitiveTopology::LineStrip || m_Specification.Wireframe;
	}

	void VulkanPipeline::SetLayout(const VertexBufferLayout& layout)
	{
		m_Specification.Layout = layout;
	}
	

}