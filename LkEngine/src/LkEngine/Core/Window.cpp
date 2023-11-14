#include "LKpch.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Scene/Scene.h" // temporary
#include "LkEngine/UI/DockSpace.h" // temporary
#include "LkEngine/Editor/EditorLayer.h"

#ifdef LK_PLATFORM_WINDOWS
#include "Platform/Windows/Windows_Window.h"
#else
// #include "Platform/Linux/Linux_Window.h"
#endif


namespace LkEngine {

	Window* Window::m_Instance = nullptr;
	bool Window::GLFW_Initialized = false;
	
	std::shared_ptr<Window> Window::Create(const char* title, uint32_t width, uint32_t height)
	{
	#ifdef LK_PLATFORM_WINDOWS
		return std::make_shared<Windows_Window>(title, width, height);
	#elif defined(LK_PLATFORM_LINUX)
		std::shared_ptr<Window> window = std::make_shared<Linux_Window>(title, width, height);
		return window;
	#else
		return nullptr;
	#endif
	}

	void Window::WindowResizeCallback(GLFWwindow* glfw_window, int width, int height)
	{
		int size_x, size_y;
		glfwGetWindowSize(glfw_window, &size_x, &size_y);

		auto window = Window::Get();
		//window->m_ViewportWidth = size_x;
		//window->m_ViewportHeight = size_y;
		window->SetViewportWidth(size_x);
		window->SetViewportHeight(size_y);

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos = viewport->WorkPos;

		if (EditorLayer::IsEnabled())
		{
			//window->m_Width = EditorLayer::EditorWindowSize.x;
			//window->m_Height = EditorLayer::EditorWindowSize.y;
			//width = EditorLayer::EditorWindowSize.x;
			//height = EditorLayer::EditorWindowSize.y;
			//pos = ImVec2(EditorLayer::EditorWindowPos.x, EditorLayer::EditorWindowPos.y);
		}
		else
		{
			window->SetWidth(width);
			window->SetHeight(height);
		}

		glViewport(pos.x, pos.y, width, height);
		LOG_DEBUG("Window Resize: ({}, {})", window->GetWidth(), window->GetHeight());

		auto& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);
	}


}
