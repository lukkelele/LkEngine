#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    class Window
    {
    public:
        virtual ~Window() = default;
    
        static Window* Get() { return m_Instance; }
        static std::shared_ptr<Window> Create(const char* title, uint32_t width, uint32_t height);

        virtual void Init(const std::string& glslVersion = "#version 450") = 0;
        virtual void OnUpdate() = 0;
        virtual void Exit() = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual std::shared_ptr<GLFWwindow*> GetGlfwWindow() const = 0;
        virtual std::shared_ptr<GraphicsContext> GetContext() const = 0;
        uint16_t GetWidth()  const { return m_Width; }
        uint16_t GetHeight() const { return m_Height; }
        uint16_t GetViewportWidth()  const { return m_ViewportWidth; }
        uint16_t GetViewportHeight() const { return m_ViewportHeight; }
        void SetWidth(uint16_t width) { m_Width = width; }
        void SetHeight(uint16_t height) { m_Height = height; }
        void SetViewportWidth(uint16_t width) { m_ViewportWidth = width; }
        void SetViewportHeight(uint16_t height) { m_ViewportHeight = height; }
        std::string GetTitle()  const { return m_Title; }
        std::string GetGlslVersion() const { return m_GlslVersion;  }
        bool IsVSync() const { return m_VSync; }
        std::pair<float, float> GetPos();
        std::pair<float, float> GetSize();
        std::pair<float, float> GetViewportSize();

    protected:
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
    
    protected:
        uint16_t m_Width = 0, m_Height = 0;
        uint16_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        std::string m_Title = "";
        std::string m_GlslVersion = "";
        bool m_VSync;

        static bool GLFW_Initialized;
        static Window* m_Instance;
    };



    // Move this
    constexpr int OpenGL_Major_Version = 4;
    constexpr int OpenGL_Minor_Version = 5;
    constexpr const char* OpenGL_GLSL_33 = "#version 330";
    constexpr const char* OpenGL_GLSL_45 = "#version 450";
    constexpr const char* OpenGL_GLSL = OpenGL_GLSL_33;

}

// OpenGL Functions
// static void OpenGL_ClearError();
// static bool OpenGL_LogCall(const char* func, const char* file, int line);