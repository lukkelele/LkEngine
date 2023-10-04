#include "Window.h"
#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/Windows_Window.h"
#else
// #include "Platform/Linux/Linux_Window.h"
#endif


namespace LkEngine {

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

}
