#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <string>


namespace LkEngine {

    // Forward declaration
    class Window;
    
    class GraphicsContext
    {
    public:
        GraphicsContext(void* _windowHandle);
        ~GraphicsContext() = default;

        static GraphicsContext* Get() { return m_Context; }
        static void BeginImGuiFrame();
        static void EndImGuiFrame();

        void Init();
        void Destroy();
        std::shared_ptr<GLFWwindow*> GetGlfwWindow();
        void InitImGui(const std::string& glslVersion);
        void SetDarkTheme();
    
    public:
        std::shared_ptr<GLFWwindow*> GlfwWindow = nullptr;
    private:
        static GraphicsContext* m_Context;
        std::shared_ptr<Window> m_Window = nullptr;
    };

}
