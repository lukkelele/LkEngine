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
        static s_ptr<Window> Create(const char* title, uint32_t width, uint32_t height);

        virtual void Init(const std::string& glslVersion = "#version 450") = 0;
        virtual void OnUpdate() = 0;
        virtual void Exit() = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual GLFWwindow* GetGlfwWindow() const = 0;
        virtual s_ptr<GraphicsContext> GetContext() const = 0;
        virtual uint16_t GetWidth() const = 0; 
        virtual uint16_t GetHeight() const = 0; 
        virtual uint16_t GetViewportWidth() const = 0; 
        virtual uint16_t GetViewportHeight() const = 0;
        virtual void SetWidth(uint16_t width) = 0;
        virtual void SetHeight(uint16_t height) = 0; 
        virtual void SetViewportWidth(uint16_t width) = 0; 
        virtual void SetViewportHeight(uint16_t height) = 0; 
        virtual void SetDepthEnabled(bool enabled) = 0;
        std::string GetTitle()  const { return m_Title; }
        std::string GetGlslVersion() const { return m_GlslVersion;  }
        bool IsVSync() const { return m_VSync; }
        virtual glm::vec2 GetPos() const = 0;
        virtual glm::vec2 GetSize() const = 0;
        virtual glm::vec2 GetViewportSize() const = 0;

    protected:
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
    
    protected:
        glm::vec2 m_Pos = { 0.0f, 0.0f };
        std::string m_Title = "";
        std::string m_GlslVersion = "";
        bool m_VSync;

        inline static bool GLFW_Initialized = false;
        inline static Window* m_Instance = nullptr;
    };

}

// OpenGL Functions
// static void OpenGL_ClearError();
// static bool OpenGL_LogCall(const char* func, const char* file, int line);