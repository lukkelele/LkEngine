#pragma once

#include "LkEngine/ImGui/ImGuiLayer.h"

#include "LkEngine/Renderer/RenderCommandBuffer.h"

#include <imgui/imgui_impl_vulkan.h>


namespace LkEngine {

    class VulkanImGuiLayer : public ImGuiLayer
    {
    public:
        VulkanImGuiLayer();
        ~VulkanImGuiLayer();

        void Init(Ref<Window>& window);
        void Shutdown();

        void OnAttach();
        void OnDetach();

        void BeginFrame();
        void EndFrame();

    private:
        bool m_Initialized = false;

		float m_Time = 0.0f;
		Ref<RenderCommandBuffer> m_RenderCommandBuffer;
    };

}