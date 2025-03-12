#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include <GLFW/glfw3.h>

namespace LkEngine {

    class LSwapChain : public LObject
    {
    public:
        virtual ~LSwapChain() = default;

		virtual void Initialize() = 0;
		virtual void InitializeSurface(GLFWwindow* GlfwWindowHandle) = 0;
		virtual void Destroy() = 0;

		virtual void OnResize(const uint32_t NewWidth, const uint32_t NewHeight) = 0;

		virtual void BeginFrame() = 0;
		virtual void Present() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

        virtual uint32_t GetCurrentBufferIndex() const = 0;

		static TObjectPtr<LSwapChain> Create(uint32_t* InWidth, uint32_t* InHeight, const bool InVSync);

	private:
		LCLASS(LSwapChain)
    };

}