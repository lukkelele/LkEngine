#include "LKpch.h"
#include "LkEngine/Core/Window.h"
#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/Windows_Window.h"
#else
// #include "Platform/Linux/Linux_Window.h"
#endif

#include "LkEngine/Core/Viewport.h"


namespace LkEngine {

	Window* Window::m_Instance = nullptr;
	bool Window::GLFW_Initialized = false;
	
	std::shared_ptr<Window> Window::Create(const char* title, uint32_t width, uint32_t height)
	{
	#ifdef PLATFORM_WINDOWS
		return std::make_shared<Windows_Window>(title, width, height);
	#elif defined(PLATFORM_LINUX)
		std::shared_ptr<Window> window = std::make_shared<Linux_Window>(title, width, height);
		return window;
	#else
		return nullptr;
	#endif
	}

	void Window::WindowResizeCallback(GLFWwindow* glfw_window, int width, int height)
	{
		LOG_DEBUG("Window resized ({0}, {1})", width, height);
		//int pos_x, pos_y;
		//glfwGetWindowPos(glfw_window, &pos_x, &pos_y);
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos = viewport->WorkPos;
		// glViewport(pos_x, pos_y, width, height);
		glViewport(pos.x, pos.y, width, height);
	}

}
