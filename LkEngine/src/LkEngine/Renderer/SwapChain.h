#pragma once

#include "LkEngine/Core/Base.h"

#include <glfw/glfw3.h>


namespace LkEngine {

    class SwapChain : public RefCounted
    {
    public:
        virtual ~SwapChain() = default;

		//virtual void Create(uint32_t* width, uint32_t* height, bool vsync) = 0;

		virtual void Init() = 0;
		virtual void InitSurface(GLFWwindow* windowHandle) = 0;

		virtual void Destroy() = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		virtual void BeginFrame() = 0;
		virtual void Present() = 0;

        virtual uint32_t GetCurrentBufferIndex() const = 0;

		static Ref<SwapChain> Create(uint32_t* width, uint32_t* height, bool vsync);
    };

}