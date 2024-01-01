#include "LKpch.h"
#include "LkEngine/Core/Window.h"

#ifdef LK_PLATFORM_WINDOWS
	#include "Platform/Windows/Windows_Window.h"
#else
	#include "Platform/Linux/Linux_Window.h"
#endif


namespace LkEngine {

	std::shared_ptr<Window> Window::Create(const ApplicationSpecification& specification)
	{
	#ifdef LK_PLATFORM_WINDOWS
		return std::make_shared<Windows_Window>(specification);
	#elif defined(LK_PLATFORM_LINUX)
		return std::make_shared<Linux_Window>(specification);
	#else
		throw std::runtime_error("No Windows/Linux platform detected");
	#endif
	}

	std::shared_ptr<Window> Window::Create(const char* title, uint32_t width, uint32_t height)
	{
	#ifdef LK_PLATFORM_WINDOWS
		return std::make_shared<Windows_Window>(title, width, height);
	#elif defined(LK_PLATFORM_LINUX)
		std::shared_ptr<Window> window = std::make_shared<Linux_Window>(title, width, height);
		return window;
	#else
		throw std::runtime_error("No Windows/Linux platform detected");
	#endif
	}

}
