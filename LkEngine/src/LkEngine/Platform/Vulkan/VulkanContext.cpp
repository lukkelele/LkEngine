#include "LKpch.h"
#include "VulkanContext.h"
#include "VulkanAllocator.h"
#include "VulkanImGuiLayer.h"

#include "LkEngine/Core/Window.h"

//#if defined(VK_KHR_SURFACE_EXTENSION_NAME) && defined(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
//#include <Windows.h>
//#endif

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	static bool Validate = true;

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsMessengerCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		(void)pUserData; //Unused argument

		const bool performanceWarnings = false;
		if (!performanceWarnings)
		{
			if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
				return VK_FALSE;
		}

		std::string labels, objects;
		if (pCallbackData->cmdBufLabelCount)
		{
			labels = fmt::format("\tLabels({}): \n", pCallbackData->cmdBufLabelCount);
			for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i)
			{
				const auto& label = pCallbackData->pCmdBufLabels[i];
				const std::string colorStr = fmt::format("[ {}, {}, {}, {} ]", label.color[0], label.color[1], label.color[2], label.color[3]);
				labels.append(fmt::format("\t\t- Command Buffer Label[{0}]: name: {1}, color: {2}\n", i, label.pLabelName ? label.pLabelName : "NULL", colorStr));
			}
		}

		if (pCallbackData->objectCount)
		{
			objects = fmt::format("\tObjects({}): \n", pCallbackData->objectCount);
			for (uint32_t i = 0; i < pCallbackData->objectCount; ++i)
			{
				const auto& object = pCallbackData->pObjects[i];
				objects.append(fmt::format("\t\t- Object[{0}] name: {1}, type: {2}, handle: {3:#x}\n", i, object.pObjectName ? object.pObjectName : "NULL", VKUtils::VkObjectTypeToString(object.objectType), object.objectHandle));
			}
		}

		LK_CORE_WARN("{0} {1} message: \n\t{2}\n {3} {4}", VKUtils::VkDebugUtilsMessageType(messageType), VKUtils::VkDebugUtilsMessageSeverity(messageSeverity), pCallbackData->pMessage, labels, objects);
		//[[maybe_unused]] const auto& imageRefs = VulkanImage2D::GetImageRefs();

		return VK_FALSE;
	}

	static bool CheckDriverAPIVersionSupport(uint32_t minimumSupportedVersion)
	{
		uint32_t instanceVersion;
		vkEnumerateInstanceVersion(&instanceVersion);

		if (instanceVersion < minimumSupportedVersion)
		{
			LK_CORE_FATAL("Incompatible Vulkan driver version!");
			LK_CORE_FATAL("  You have {}.{}.{}", VK_API_VERSION_MAJOR(instanceVersion), VK_API_VERSION_MINOR(instanceVersion), VK_API_VERSION_PATCH(instanceVersion));
			LK_CORE_FATAL("  You need at least {}.{}.{}", VK_API_VERSION_MAJOR(minimumSupportedVersion), VK_API_VERSION_MINOR(minimumSupportedVersion), VK_API_VERSION_PATCH(minimumSupportedVersion));

			return false;
		}

		return true;
	}

	VulkanContext::VulkanContext(Window* window)
    {
		//m_Instance = s_ptr<VulkanContext>(this);
	    m_Window = std::shared_ptr<Window>(window);
		m_GlfwWindow = m_Window->GetGlfwWindow();
	}

    VulkanContext::~VulkanContext()
    {
    }

    Ref<VulkanContext> VulkanContext::Get() 
	{ 
		return Ref<VulkanContext>(Renderer::GetContext()); 
	}

    void VulkanContext::Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc)
    {
		// Application Info
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "LkEngine";
		appInfo.pEngineName = "LkEngine";
		appInfo.apiVersion = VK_API_VERSION_1_2;

		// Extensions and Validation
		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Very little performance hit, can be used in Release.

		// Validate
		instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
		VkValidationFeaturesEXT features = {};
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount = 1;
		features.pEnabledValidationFeatures = enables;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr; // &features;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		// TODO: Extract all validation into separate class
		if (Validate)
		{
			const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
			// Check if this layer is available at instance level
			uint32_t instanceLayerCount;
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
			std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
			bool validationLayerPresent = false;
	#ifdef LK_PRINT_VULKAN_LAYERS
			LK_CORE_TRACE_TAG("Renderer", "Vulkan Instance Layers:");
			for (const VkLayerProperties& layer : instanceLayerProperties)
			{
				LK_CORE_TRACE_TAG("Renderer", "  {0}", layer.layerName);
				if (strcmp(layer.layerName, validationLayerName) == 0)
				{
					validationLayerPresent = true;
					break;
				}
			}
	#endif
			if (validationLayerPresent)
			{
				instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
				instanceCreateInfo.enabledLayerCount = 1;
			}
			else
			{
				LK_CORE_ERROR_TAG("Renderer", "Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
			}
		}

		// Instance and surface Creation
		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_VulkanInstance));
		VKUtils::VulkanLoadDebugUtilsExtensions(m_VulkanInstance);

		if (Validate)
		{
#if 0
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(s_VulkanInstance, "vkCreateDebugReportCallbackEXT");
			LK_CORE_ASSERT(vkCreateDebugReportCallbackEXT != NULL, "");
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = VulkanDebugReportCallback;
			debug_report_ci.pUserData = VK_NULL_HANDLE;
			VK_CHECK_RESULT(vkCreateDebugReportCallbackEXT(s_VulkanInstance, &debug_report_ci, nullptr, &m_DebugReportCallback));
#endif

			auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VulkanInstance, "vkCreateDebugUtilsMessengerEXT");
			LK_CORE_ASSERT(vkCreateDebugUtilsMessengerEXT != NULL, "");
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};
			debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsCreateInfo.pfnUserCallback = VulkanDebugUtilsMessengerCallback;
			debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT /*  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT*/;

			VK_CHECK_RESULT(vkCreateDebugUtilsMessengerEXT(m_VulkanInstance, &debugUtilsCreateInfo, nullptr, &m_DebugUtilsMessenger));
		}

		m_PhysicalDevice = VulkanPhysicalDevice::Select();

		VkPhysicalDeviceFeatures enabledFeatures;
		memset(&enabledFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
		enabledFeatures.samplerAnisotropy = true;
		enabledFeatures.wideLines = true;
		enabledFeatures.fillModeNonSolid = true;
		enabledFeatures.independentBlend = true;
		enabledFeatures.pipelineStatisticsQuery = true;

		m_Device = Ref<VulkanDevice>::Create(m_PhysicalDevice, enabledFeatures);
		VulkanAllocator::Init(m_Device);
		LK_CORE_ASSERT(m_Device != nullptr, "VulkanDevice is nullptr!");

		// Pipeline Cache
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreatePipelineCache(m_Device->GetVulkanDevice(), &pipelineCacheCreateInfo, nullptr, &m_PipelineCache));

		//SetBlendingEnabled(true);
		//SetDepthEnabled(false);
		//SetBlendFunction(srcFunc, dstFunc);
    }

	void VulkanContext::SetViewport(const glm::vec2& pos, const glm::vec2& size)
	{
		//glViewport(pos.x, pos.y, size.x, size.y);
		//glViewport(pos.x * m_Window->GetScalerX(), pos.y * m_Window->GetScalerY(), size.x, size.y);
	}

	void VulkanContext::SetBlendingEnabled(bool enabled)
	{
		m_BlendingEnabled = enabled;
		//if (m_BlendingEnabled)
		//	glEnable(GL_BLEND);
		//else
		//	glDisable(GL_BLEND);
	}

    void VulkanContext::Destroy()
    {
    }

	void VulkanContext::UpdateResolution(uint16_t width, uint16_t height)
	{
		//ImGuiViewport* viewport = ImGui::GetMainViewport();
		//ImVec2 pos = viewport->WorkPos;
		//glViewport(pos.x, pos.y, width, height);
		//auto& io = ImGui::GetIO();
		//io.DisplaySize = ImVec2(width, height);
	}

	void VulkanContext::SetDepthEnabled(bool enabled)
	{
		m_DepthEnabled = enabled;
		if (m_DepthEnabled)
		{
			//glEnable(GL_DEPTH_TEST);
			return;
		}
		//glDisable(GL_DEPTH_TEST);
	}

	void VulkanContext::SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc)
	{
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(srcFunc));
		LK_CORE_DEBUG("Setting destination blend function: {}", GetDestinationBlendFunctionName(dstFunc));
		// SET BLEND FUNC HERE
		m_BlendFunction.Source = srcFunc;
		m_BlendFunction.Destination = dstFunc;
	}

	void VulkanContext::SetSourceBlendFunction(const SourceBlendFunction& srcFunc)
	{
		m_BlendFunction.Source = srcFunc;
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(srcFunc));

		//glBlendFunc(GetOpenGLSourceBlendFunction(m_BlendFunction.Source), GetOpenGLDestinationBlendFunction(m_BlendFunction.Destination));
	}

    void VulkanContext::SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc)
	{
		m_BlendFunction.Destination = dstFunc;
		LK_CORE_DEBUG("Setting source blend function: {}", GetDestinationBlendFunctionName(dstFunc));
		//glBlendFunc(GetOpenGLSourceBlendFunction(m_BlendFunction.Source), GetOpenGLDestinationBlendFunction(m_BlendFunction.Destination));
	}

	std::string VulkanContext::GetCurrentSourceBlendFunctionName() const
	{
		return GetSourceBlendFunctionName(m_BlendFunction.Source);
	}

	std::string VulkanContext::GetCurrentDestinationBlendFunctionName() const
	{
		return GetDestinationBlendFunctionName(m_BlendFunction.Destination);
	}



}