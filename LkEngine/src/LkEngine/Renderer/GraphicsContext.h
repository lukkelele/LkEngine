#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine {

    // Forward declaration
    class Window;
    
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        static GraphicsContext* Get() { return m_Instance; }

        static s_ptr<GraphicsContext> Create(Window& window, const std::string& glslVersion);

        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void BeginImGuiFrame() = 0;
        virtual void EndImGuiFrame() = 0;
        virtual void InitImGui(const std::string& glslVersion) = 0;
        virtual void SetDepthEnabled(bool enabled) = 0;
        virtual GLFWwindow* GetGlfwWindow() = 0;
        virtual void SetDarkTheme();

    protected:
        static void HandleViewportEvents();
    
    protected:
        inline static GraphicsContext* m_Instance = nullptr;

    };

}
