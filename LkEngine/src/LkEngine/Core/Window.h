#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    class Window
    {
    public:
        virtual ~Window() = default;
    
        static Window* Get() { return m_Instance; }
        static s_ptr<Window> Create(const ApplicationSpecification& specification);
        static s_ptr<Window> Create(const char* title, uint32_t width, uint32_t height);

        virtual void Init(const std::string& shaderVersion) = 0;
        virtual void SwapBuffers() = 0;
        virtual void Exit() = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual GLFWwindow* GetGlfwWindow() const = 0;
        virtual s_ptr<GraphicsContext> GetContext() const = 0;
        inline virtual uint16_t GetWidth() const = 0; 
        inline virtual uint16_t GetHeight() const = 0; 
        inline virtual uint16_t GetViewportWidth() const = 0; 
        inline virtual uint16_t GetViewportHeight() const = 0;
        virtual void SetWidth(uint16_t width) = 0;
        virtual void SetHeight(uint16_t height) = 0; 
        virtual void SetSize(const glm::vec2& size) = 0;
        virtual void SetViewportWidth(uint16_t width) = 0; 
        virtual void SetViewportHeight(uint16_t height) = 0; 
        virtual void SetDepthEnabled(bool enabled) = 0;
        virtual std::string GetTitle() const = 0;
        virtual std::string GetGlslVersion() const = 0;
        virtual bool IsVSyncEnabled() const = 0;
        virtual glm::vec2 GetPos() const = 0;
        virtual glm::vec2 GetSize() const = 0;
        virtual glm::vec2 GetViewportSize() const = 0;
        virtual float GetScalerX() const = 0;
        virtual float GetScalerY() const = 0;
        virtual glm::vec2 GetScalers() const = 0;
        virtual void SetScalerX(float x) = 0;
        virtual void SetScalerY(float y) = 0;
        virtual void SetScalers(float x, float y) = 0;
        virtual void SetScalers(const glm::vec2& scalers) = 0;

    protected:
        inline static Window* m_Instance = nullptr;
    };

}
