#include "LKpch.h"
#include "VulkanImGuiLayer.h"
#include "VulkanContext.h"
#include "VulkanSwapChain.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include <imgui/imgui_internal.h>


namespace LkEngine {

	static std::vector<VkCommandBuffer> ImGuiCommandBuffers;

    VulkanImGuiLayer::VulkanImGuiLayer()
    {
    }

    VulkanImGuiLayer::~VulkanImGuiLayer()
    {
    }

    void VulkanImGuiLayer::Init(Ref<Window>& window)
    {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       

		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);

		// Build the font atlas
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		io.ConfigDockingAlwaysTabBar = false;

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

		VulkanImGuiLayer* instance = this;
		Renderer::Submit([instance]()
		{
			Application& app = *Application::Get();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow()->GetGlfwWindow());

			auto vulkanContext = VulkanContext::Get();
			auto device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

			VkDescriptorPool descriptorPool;

			// Create Descriptor Pool
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
			};
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 100 * IM_ARRAYSIZE(pool_sizes);
			pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool));

			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForVulkan(window, true);
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = VulkanContext::GetInstance();
			init_info.PhysicalDevice = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetVulkanPhysicalDevice();
			init_info.Device = device;
			init_info.QueueFamily = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetQueueFamilyIndices().Graphics;
			init_info.Queue = VulkanContext::GetCurrentDevice()->GetGraphicsQueue();
			init_info.PipelineCache = nullptr;
			init_info.DescriptorPool = descriptorPool;
			init_info.Allocator = nullptr;
			init_info.MinImageCount = 2;
			VulkanSwapChain& swapChain = *Ref<VulkanSwapChain>(Application::Get()->GetWindow()->GetSwapChain());
			//VulkanSwapChain& swapChain = Application::Get()->GetWindow()->GetSwapChain();
			init_info.ImageCount = swapChain.GetImageCount();
			init_info.CheckVkResultFn = VKUtils::VulkanCheckResult;
			ImGui_ImplVulkan_Init(&init_info, swapChain.GetRenderPass());

			// Upload Fonts
			{
				// Use any command queue

				VkCommandBuffer commandBuffer = vulkanContext->GetCurrentDevice()->GetCommandBuffer(true);
				ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
				vulkanContext->GetCurrentDevice()->FlushCommandBuffer(commandBuffer);

				VK_CHECK_RESULT(vkDeviceWaitIdle(device));
				ImGui_ImplVulkan_DestroyFontUploadObjects();
			}

			//uint32_t framesInFlight = Renderer::GetConfig().FramesInFlight;
			uint32_t framesInFlight = Renderer::GetFramesInFlight();
			ImGuiCommandBuffers.resize(framesInFlight);
			for (uint32_t i = 0; i < framesInFlight; i++)
				ImGuiCommandBuffers[i] = VulkanContext::GetCurrentDevice()->CreateSecondaryCommandBuffer("ImGuiSecondaryCoommandBuffer");
		});

#if 0
		Application& app = *Application::Get();
		//GLFWwindow* glfwWindow = app.GetGlfwWindow();
		GLFWwindow* glfwWindow = window->GetGlfwWindow();

		auto vulkanContext = VulkanContext::Get();
		auto device = VulkanContext::GetCurrentDevice()->GetVulkanDevice();

		VkDescriptorPool descriptorPool;
		// Create Descriptor Pool
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 100 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool));

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = VulkanContext::GetInstance();
		init_info.PhysicalDevice = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		init_info.Device = device;
		init_info.QueueFamily = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetQueueFamilyIndices().Graphics;
		init_info.Queue = VulkanContext::GetCurrentDevice()->GetGraphicsQueue();
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = descriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;

		//VulkanSwapChain& swapChain = *Ref<VulkanSwapChain>(Application::Get()->GetWindow()->GetSwapChain());
		VulkanSwapChain& swapChain = *Ref<VulkanSwapChain>(window->GetSwapChain());
		LK_CORE_ASSERT(swapChain.GetRenderPass() != nullptr, "VulkanImGuiLayer::Init, VulkanSwapChain render pass is nullptr!")
		//VulkanSwapChain& swapChain = *Ref<SwapChain>().As<VulkanSwapChain>(Application::Get()->GetWindow()->GetSwapChain());
		//VulkanSwapChain& swapChain = *Ref<SwapChain>().As<VulkanSwapChain>();
		init_info.ImageCount = swapChain.GetImageCount();
		init_info.CheckVkResultFn = VKUtils::VulkanCheckResult;
		ImGui_ImplVulkan_Init(&init_info, swapChain.GetRenderPass());
		LK_INFO("ImGui Version: {}", ImGui::GetVersion());
#endif

		m_Initialized = true;
    }

    void VulkanImGuiLayer::Shutdown()
    {
		ImGui_ImplGlfw_Shutdown();
        ImGui_ImplVulkan_Shutdown();
		ImGui::DestroyContext();

    }

	void VulkanImGuiLayer::OnAttach()
	{
	}

	void VulkanImGuiLayer::OnDetach()
	{
	}


    void VulkanImGuiLayer::BeginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		ImGuizmo::BeginFrame();
    }

    void VulkanImGuiLayer::EndFrame()
    {
        //ImGui::Render();
        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), );
		ImGui::Render();

		//VulkanSwapChain& swapChain = *Ref<VulkanSwapChain&>(Application::Get()->GetWindow()->GetSwapChain());
		auto window = Window::Get();
		VulkanSwapChain& swapChain = *Ref<VulkanSwapChain>(window->GetSwapChain());

		VkClearValue clearValues[2];
		clearValues[0].color = { {0.1f, 0.1f,0.1f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		uint32_t width = swapChain.GetWidth();
		uint32_t height = swapChain.GetHeight();

		uint32_t commandBufferIndex = swapChain.GetCurrentBufferIndex();

		VkCommandBufferBeginInfo drawCmdBufInfo = {};
		drawCmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		drawCmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		drawCmdBufInfo.pNext = nullptr;

		VkCommandBuffer drawCommandBuffer = swapChain.GetCurrentDrawCommandBuffer();
		VK_CHECK_RESULT(vkBeginCommandBuffer(drawCommandBuffer, &drawCmdBufInfo));

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = swapChain.GetRenderPass();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		renderPassBeginInfo.clearValueCount = 2; // Color + depth
		renderPassBeginInfo.pClearValues = clearValues;
		renderPassBeginInfo.framebuffer = swapChain.GetCurrentFramebuffer();

		vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = swapChain.GetRenderPass();
		inheritanceInfo.framebuffer = swapChain.GetCurrentFramebuffer();

		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		cmdBufInfo.pInheritanceInfo = &inheritanceInfo;

		VK_CHECK_RESULT(vkBeginCommandBuffer(ImGuiCommandBuffers[commandBufferIndex], &cmdBufInfo));

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = (float)height;
		viewport.height = -(float)height;
		viewport.width = (float)width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(ImGuiCommandBuffers[commandBufferIndex], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(ImGuiCommandBuffers[commandBufferIndex], 0, 1, &scissor);

		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, ImGuiCommandBuffers[commandBufferIndex]);

		VK_CHECK_RESULT(vkEndCommandBuffer(ImGuiCommandBuffers[commandBufferIndex]));

		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.push_back(ImGuiCommandBuffers[commandBufferIndex]);

		vkCmdExecuteCommands(drawCommandBuffer, uint32_t(commandBuffers.size()), commandBuffers.data());

		vkCmdEndRenderPass(drawCommandBuffer);

		VK_CHECK_RESULT(vkEndCommandBuffer(drawCommandBuffer));

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
    
}
