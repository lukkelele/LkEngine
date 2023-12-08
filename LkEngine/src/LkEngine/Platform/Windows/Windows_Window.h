#pragma once

#include "LkEngine/Core/Window.h"


namespace LkEngine {

    // Forward declaration
    class Renderer;
    
    class Windows_Window : public Window
    {
    public:
        Windows_Window(const char* title, uint32_t width, uint32_t height);
        ~Windows_Window();
    
        void Init(const std::string& glslVersion);
        void OnUpdate();
        void Exit();
        bool IsVSync() const;
        void SetVSync(bool enabled);
        GLFWwindow* GetGlfwWindow() const { return m_GlfwWindow; }
        s_ptr<GraphicsContext> GetContext() const { return m_Context; }
        uint16_t GetWidth() const { return m_Width; }
        uint16_t GetHeight() const { return m_Height; }
        uint16_t GetViewportWidth()  const { return m_ViewportWidth; }
        uint16_t GetViewportHeight() const { return m_ViewportHeight; }
        glm::vec2 GetViewportSize() const;
        glm::vec2 GetSize() const;
        glm::vec2 GetPos() const;
        void SetWidth(uint16_t width) { m_Width = width; }
        void SetHeight(uint16_t height) { m_Height = height; }
        void SetViewportWidth(uint16_t width) { m_ViewportWidth = width; }
        void SetViewportHeight(uint16_t height) { m_ViewportHeight = height; }
        void SetDepthEnabled(bool enabled) { m_Context->SetDepthEnabled(enabled); }

    private:
        uint16_t m_Width = 0, m_Height = 0;
        uint16_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        GLFWwindow* m_GlfwWindow = nullptr;
        s_ptr<GraphicsContext> m_Context = nullptr;
    };

}