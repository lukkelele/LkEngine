#include "LKpch.h"
#include "LkEngine/Renderer/GraphicsContext.h"
#include "LkEngine/Platform/Windows/Windows_Window.h"
#include "LkEngine/UI/UILayer.h"
#include <ImGuizmo/ImGuizmo.h>

#ifdef LK_RENDERER_API_OPENGL
	#include "LkEngine/Renderer/OpenGL/OpenGLContext.h"
#elif defined(LK_RENDERER_API_VULKAN)
#endif

namespace LkEngine {

	s_ptr<GraphicsContext> GraphicsContext::Create(Window& window, const std::string& glslVersion)
	{
	#ifdef LK_RENDERER_API_OPENGL
		return std::make_shared<OpenGLContext>(window, glslVersion);
	#elif defined(LK_RENDERER_API_VULKAN)
		throw std::runtime_error("No graphics context could be created as Vulkan is not supported yet!");
	#else
		throw std::runtime_error("No graphics context could be created as LK_RENDERER_API is undefined");
	#endif
	}

	void GraphicsContext::HandleViewportEvents()
	{
		auto window = Window::Get();
		int viewport_width, viewport_height;
		glfwGetWindowSize(window->GetGlfwWindow(), &viewport_width, &viewport_height);
	}

#if 0
	ImVec2 GraphicsContext::GetMainRenderWindowSize()
	{
		return ImVec2(m_Window->GetWidth(), m_Window->GetHeight());
	}

	ImVec2 GraphicsContext::GetMainRenderWindowPos()
	{
		glm::vec2 pos = m_Window->GetPos();
		return ImVec2(pos.x, pos.y);
	}
#endif
	
	void GraphicsContext::SetDarkTheme()
	{
		ImGui::StyleColorsDark();
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		// Check Mark
		colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
		// Slider
		colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
	}


}
