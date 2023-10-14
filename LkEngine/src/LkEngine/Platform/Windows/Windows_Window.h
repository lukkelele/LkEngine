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
    
        void Init(const std::string& glslVersion = OpenGL_GLSL);
        void OnUpdate();
        void Exit();
        bool IsVSync() const;
        void SetVSync(bool enabled);

        std::shared_ptr<GLFWwindow*> GetGlfwWindow() const { return m_GlfwWindow; }
        std::shared_ptr<GraphicsContext> GetContext() const { return m_Context; }

    private:
        std::shared_ptr<GLFWwindow*> m_GlfwWindow = nullptr;
        std::shared_ptr<GraphicsContext> m_Context = nullptr;
    };

}