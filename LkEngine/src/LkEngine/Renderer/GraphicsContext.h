#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <string>
#include <imgui/imgui.h>
//#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine {

    // Forward declaration
    class Window;
    
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        static GraphicsContext* Get() { return m_Context; }

        static s_ptr<GraphicsContext> Create(Window& window, const std::string& glslVersion);

        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void BeginImGuiFrame() = 0;
        virtual void EndImGuiFrame() = 0;
        virtual void InitImGui(const std::string& glslVersion) = 0;
        virtual void SetDarkTheme();
        s_ptr<GLFWwindow*> GetGlfwWindow() { return m_GlfwWindow; }

        // Remove 
        ImVec2 GetMainRenderWindowSize();
        ImVec2 GetMainRenderWindowPos();

    protected:
        static void HandleViewportEvents();
    
    protected:
        inline static GraphicsContext* m_Context = nullptr;
        s_ptr<GLFWwindow*> m_GlfwWindow = nullptr;
        s_ptr<Window> m_Window = nullptr;
        std::string m_GlslVersion = "";

        // Remove
        ImVec2 m_ViewportSize, m_ViewportPos; // GLFW window
        ImVec2 m_MainRenderWindowSize, m_MainRenderWindowPos;
    };

}
