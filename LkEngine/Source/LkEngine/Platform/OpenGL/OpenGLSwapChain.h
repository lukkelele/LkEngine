#pragma once

#include "LkEngine/Renderer/SwapChain.h"


struct GLFWwindow;

namespace LkEngine {

	class OpenGLSwapChain : public SwapChain
	{
	public:
		OpenGLSwapChain() = default;
		OpenGLSwapChain(uint32_t* width, uint32_t* height, bool vsync);

		void Init();
		void InitSurface(GLFWwindow* windowHandle);
		void Create(uint32_t* width, uint32_t* height, bool vsync);
		void Destroy();

		void OnResize(uint32_t width, uint32_t height);

		void BeginFrame();
		void Present();

		uint32_t GetImageCount() const { return m_ImageCount; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		uint32_t GetCurrentBufferIndex() const;

	private:
		uint32_t m_Width, m_Height;
		uint32_t m_ImageCount = 0;
	};

}
