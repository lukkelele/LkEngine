#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui_impl_opengl3.h>


namespace LkEngine {

    // Forward declaration
    class Window;
    
    class GraphicsContext
    {
    public:
        GraphicsContext(void* window_handle);
        ~GraphicsContext() = default;

        static GraphicsContext* Get() { return m_Context; }
        void BeginImGuiFrame();
        void EndImGuiFrame();

        void Init();
        void Destroy();
        s_ptr<GLFWwindow*> GetGlfwWindow();
        void InitImGui(const std::string& glslVersion);
        void SetDarkTheme();
        ImVec2 GetMainRenderWindowSize();
        ImVec2 GetMainRenderWindowPos();

    private:
        static void HandleViewportEvents();
    
    private:
        static GraphicsContext* m_Context;
        s_ptr<GLFWwindow*> m_GlfwWindow = nullptr;
        s_ptr<Window> m_Window = nullptr;
        ImVec2 m_MainRenderWindowSize;
        ImVec2 m_MainRenderWindowPos;
    };

}
