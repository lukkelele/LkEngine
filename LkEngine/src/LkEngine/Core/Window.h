#pragma once
#include "LkEngine/Core/Base.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    class Window
    {
    public:
        virtual ~Window() = default;
    
        static std::shared_ptr<Window> Create(const char* title, uint32_t width, uint32_t height);
        virtual void Init(const std::string& glslVersion = "#version 330") = 0;
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
    
    protected:
        uint32_t m_Width, m_Height;
        std::string m_Title;
        std::string m_GlslVersion = "";
        bool m_VSync;
    
        static bool GLFW_Initialized;
    };

}

// OpenGL Functions
// static void OpenGL_ClearError();
// static bool OpenGL_LogCall(const char* func, const char* file, int line);