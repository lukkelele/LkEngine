#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    // Forward declaration
    class Viewport;

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
        uint32_t GetWidth()  const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        std::string GetTitle()  const { return m_Title; }
        std::string GetGlslVersion() const { return m_GlslVersion;  }
        bool IsVSync() const { return m_VSync; }
        s_ptr<Viewport> GetViewport() { return m_Viewport; }

    protected:
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        //virtual void UpdateViewportSize(GLFWwindow* window, int width, int height) = 0;
    
    protected:
        uint32_t m_Width, m_Height;
        std::string m_Title = "";
        std::string m_GlslVersion = "";
        bool m_VSync;
        s_ptr<Viewport> m_Viewport = nullptr;

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