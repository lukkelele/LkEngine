#include "LKpch.h"
#include "OpenGLSwapChain.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLSwapChain::OpenGLSwapChain(uint32_t* InWidth, uint32_t* InHeight, const bool InVSync)
	{
		LK_UNUSED(InWidth && InHeight && InVSync);
		LOBJECT_REGISTER();
	}

	void OpenGLSwapChain::Initialize()
	{
	}

	void OpenGLSwapChain::InitializeSurface(GLFWwindow* GlfwWindowHandle)
	{
		LK_UNUSED(GlfwWindowHandle);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void OpenGLSwapChain::Create(uint32_t* InWidth, uint32_t* InHeight, const bool InVSync)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void OpenGLSwapChain::Destroy()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void OpenGLSwapChain::OnResize(const uint32_t NewWidth, const uint32_t NewHeight)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void OpenGLSwapChain::BeginFrame()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void OpenGLSwapChain::Present()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	uint32_t OpenGLSwapChain::GetCurrentBufferIndex() const
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		return 1;
	}
	
}