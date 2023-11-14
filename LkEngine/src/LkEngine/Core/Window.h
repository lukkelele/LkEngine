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
        virtual s_ptr<GLFWwindow*> GetGlfwWindow() const = 0;
        virtual s_ptr<GraphicsContext> GetContext() const = 0;
        virtual uint16_t GetWidth() const = 0; // { return m_Width; }
        virtual uint16_t GetHeight() const = 0; // { return m_Height; }
        virtual uint16_t GetViewportWidth() const = 0; // { return m_ViewportWidth; }
        virtual uint16_t GetViewportHeight() const = 0; // { return m_ViewportHeight; }
        virtual void SetWidth(uint16_t width) = 0; // { m_Width = width; }
        virtual void SetHeight(uint16_t height) = 0; // { m_Height = height; }
        virtual void SetViewportWidth(uint16_t width) = 0; // { m_ViewportWidth = width; }
        virtual void SetViewportHeight(uint16_t height) = 0; // { m_ViewportHeight = height; }
        std::string GetTitle()  const { return m_Title; }
        std::string GetGlslVersion() const { return m_GlslVersion;  }
        bool IsVSync() const { return m_VSync; }
        virtual glm::vec2 GetPos() const = 0;
        virtual glm::vec2 GetSize() const = 0;
        virtual glm::vec2 GetViewportSize() const = 0;

    protected:
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
    
    protected:
        //uint16_t m_Width = 0, m_Height = 0;
        //uint16_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        glm::vec2 m_Pos = { 0.0f, 0.0f };
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