#pragma once

#include "LkEngine/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"


namespace LkEngine {

    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(Window* window);
        VulkanContext() = default;
        VulkanContext(const GraphicsContext& context) {}
        //VulkanContext(s_ptr<GraphicsContext> context) {}
        ~VulkanContext();

        void Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc);
        void Destroy();
        void SetViewport(const glm::vec2& pos, const glm::vec2& size);
        void UpdateResolution(uint16_t width, uint16_t height);
        void SetDepthEnabled(bool enabled);
        GLFWwindow* GetGlfwWindow() { return m_GlfwWindow; }
        void SetBlendingEnabled(bool enabled);
        void SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc);
        void SetSourceBlendFunction(const SourceBlendFunction& srcFunc);
        void SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc);
        bool GetBlending() const { return m_BlendingEnabled; }
        bool& GetBlending() { return m_BlendingEnabled; }

        void SetName(std::string_view name) override { m_Name = std::string(name); }
        const std::string GetName() const { return m_Name; }

        std::string GetCurrentSourceBlendFunctionName() const;
        std::string GetCurrentDestinationBlendFunctionName() const;

        Ref<VulkanDevice>& GetDevice() { return m_Device; }
        Ref<VulkanPhysicalDevice>& GetPhysicalDevice() { return m_PhysicalDevice; }

		static VkInstance GetInstance() { return m_VulkanInstance; }

        static Ref<VulkanContext> Get(); 
		static Ref<VulkanDevice> GetCurrentDevice() { return Get()->GetDevice(); }

    private:
		inline static VkInstance m_VulkanInstance;

		Ref<VulkanDevice> m_Device;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;

        std::string m_Name;

		VkPipelineCache m_PipelineCache = nullptr;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		VulkanSwapChain m_SwapChain;

        bool m_DepthEnabled = true;
        bool m_BlendingEnabled = true;
        BlendFunction m_BlendFunction;

        std::shared_ptr<Window> m_Window = nullptr;
        GLFWwindow* m_GlfwWindow = nullptr;
    };

}