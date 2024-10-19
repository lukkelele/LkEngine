#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include <glfw/glfw3.h>


namespace LkEngine {

    class LSwapChain : public LObject
    {
    public:
        virtual ~LSwapChain() = default;

		virtual void Init() = 0;
		virtual void InitSurface(GLFWwindow* windowHandle) = 0;

		virtual void Destroy() = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		virtual void BeginFrame() = 0;
		virtual void Present() = 0;

        virtual uint32_t GetCurrentBufferIndex() const = 0;

		static TObjectPtr<LSwapChain> Create(uint32_t* width, uint32_t* height, bool vsync);

	private:
		LCLASS(LSwapChain)
    };

}