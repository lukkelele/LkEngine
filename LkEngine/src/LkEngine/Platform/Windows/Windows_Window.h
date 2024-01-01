#pragma once

#include "LkEngine/Core/Window.h"


namespace LkEngine {

    // Forward declaration
    class Renderer;
    
    class Windows_Window : public Window
    {
    public:
        Windows_Window(const char* title, uint32_t width, uint32_t height);
        Windows_Window(const ApplicationSpecification& specification);
        ~Windows_Window();
    
        void Init(const std::string& glslVersion);
        void SwapBuffers();
        void Exit();
        uint16_t GetWidth() const { return m_Width; }
        uint16_t GetHeight() const { return m_Height; }
        uint16_t GetViewportWidth()  const { return m_ViewportWidth; }
        uint16_t GetViewportHeight() const { return m_ViewportHeight; }
        glm::vec2 GetViewportSize() const;
        glm::vec2 GetSize() const;
        glm::vec2 GetPos() const;
        void SetSize(const glm::vec2& size);
        void SetVSync(bool enabled);
        std::string GetTitle() const { return m_Title; }
        bool IsVSyncEnabled() const { return m_VSync; }
        std::string GetGlslVersion() const { return m_GlslVersion;  }
        GLFWwindow* GetGlfwWindow() const { return m_GlfwWindow; }
        s_ptr<GraphicsContext> GetContext() const { return m_GraphicsContext; }
        void SetWidth(uint16_t width); // { m_Width = width; }
        void SetHeight(uint16_t height); // { m_Height = height; }
        void SetViewportWidth(uint16_t width) { m_ViewportWidth = width; }
        void SetViewportHeight(uint16_t height) { m_ViewportHeight = height; }
        void SetDepthEnabled(bool enabled) { m_GraphicsContext->SetDepthEnabled(enabled); }
        float GetScalerX() const;
        float GetScalerY() const;
        glm::vec2 GetScalers() const;
        void SetScalerX(float x);
        void SetScalerY(float y);
        void SetScalers(float x, float y);

        // Callbacks
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    private:
        std::string m_Title = "";
        uint16_t m_Width = 0, m_Height = 0;
        uint16_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        glm::vec2 m_Pos = { 0.0f, 0.0f };
        float m_ScalerX = 1.0f, m_ScalerY = 1.0f;

        std::string m_GlslVersion = "";
        bool m_VSync;

        GLFWwindow* m_GlfwWindow = nullptr;
        s_ptr<GraphicsContext> m_GraphicsContext = nullptr;

    };

}