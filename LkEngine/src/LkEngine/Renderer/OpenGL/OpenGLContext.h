#pragma once

#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    // Forward declaration
    class Window;


    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(Window& window, const std::string& glslVersion);
        ~OpenGLContext();

        void Init();
        void Destroy();
        void BeginImGuiFrame();
        void EndImGuiFrame();
        void InitImGui(const std::string& glslVersion);
        void SetDepthEnabled(bool enabled);
        GLFWwindow* GetGlfwWindow() { return m_GlfwWindow; }

    private:
        std::string m_GlslVersion = "";
        bool m_depthEnabled = true;
        GLFWwindow* m_GlfwWindow = nullptr;
        s_ptr<Window> m_Window = nullptr;
    };

}