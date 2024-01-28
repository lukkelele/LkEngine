#include "LKpch.h"
#include "OpenGLSwapChain.h"

#include "LkOpenGL.h"

//#include <glfw/glfw3.h>


namespace LkEngine {

	OpenGLSwapChain::OpenGLSwapChain(uint32_t* width, uint32_t* height, bool vsync)
	{
	}

	void OpenGLSwapChain::Init()
	{
	}

	void OpenGLSwapChain::InitSurface(GLFWwindow* windowHandle)
	{
	}

	void OpenGLSwapChain::Create(uint32_t* width, uint32_t* height, bool vsync)
	{
	}

	void OpenGLSwapChain::Destroy()
	{
	}

	void OpenGLSwapChain::OnResize(uint32_t width, uint32_t height)
	{
	}

	void OpenGLSwapChain::BeginFrame()
	{
	}

	void OpenGLSwapChain::Present()
	{
	}

	uint32_t OpenGLSwapChain::GetCurrentBufferIndex() const
	{
		return 1;
	}
	
}