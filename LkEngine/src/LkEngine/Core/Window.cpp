#include "LKpch.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Scene/Scene.h" // temporary
#include "LkEngine/UI/DockSpace.h" // temporary

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/Windows_Window.h"
#else
// #include "Platform/Linux/Linux_Window.h"
#endif


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
		//LOG_DEBUG("Window resized ({0}, {1})", width, height);
		int size_x, size_y;
		glfwGetWindowSize(glfw_window, &size_x, &size_y);

		auto window = Window::Get();
		window->m_ViewportWidth = size_x;
		window->m_ViewportHeight = size_y;
		auto& cam = *Scene::ActiveScene->GetActiveCamera();
		//cam.SetProjection(glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height)));
		//cam.SetProjection(glm::ortho(0.0f, static_cast<float>(DockSpace::CenterWindowSize.x), 0.0f, static_cast<float>(DockSpace::CenterWindowSize.y)));

		//mouse_pos.y *= LkEngine::DockSpace::CenterWindowSize.y;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos = viewport->WorkPos;

		glViewport(pos.x, pos.y, width, height);
		window->m_Width = width;
		window->m_Height = height;

		auto& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);
	}

}
