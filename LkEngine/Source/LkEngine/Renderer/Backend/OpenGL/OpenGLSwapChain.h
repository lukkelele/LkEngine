#pragma once

#include "LkEngine/Renderer/SwapChain.h"

namespace LkEngine {

	class OpenGLSwapChain : public LSwapChain
	{
	public:
		OpenGLSwapChain() = default;
		OpenGLSwapChain(uint32_t* InWidth, uint32_t* InHeight, const bool InVSync);

		virtual void Initialize() override;
		virtual void InitializeSurface(GLFWwindow* GlfwWindowHandle) override;
		virtual void Destroy() override;

		virtual void OnResize(const uint32_t NewWidth, const uint32_t NewHeight) override;

		virtual void BeginFrame() override;
		virtual void Present() override;

		FORCEINLINE uint32_t GetImageCount() const { return m_ImageCount; }
		FORCEINLINE virtual uint32_t GetWidth() const override { return m_Width; }
		FORCEINLINE virtual uint32_t GetHeight() const override { return m_Height; }
		FORCEINLINE virtual uint32_t GetCurrentBufferIndex() const override;

		void Create(uint32_t* InWidth, uint32_t* InHeight, const bool InVSync);

	private:
		uint32_t m_Width{};
		uint32_t m_Height{};
		uint32_t m_ImageCount = 0;

		LCLASS(OpenGLSwapChain)
	};

}
