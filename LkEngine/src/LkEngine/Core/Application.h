#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Timer.h"


namespace LkEngine {

    struct ApplicationProperties
    {
		std::string Title;
		uint32_t Width, Height;
		std::string Directory;
		std::string GlslVersion;
		ApplicationProperties(const std::string& title = "LkEngine",
						      uint16_t width = 1920,
						      uint16_t height = 1080,
                              const std::string& directory = "",
                              const std::string& glslVersion = "#version 450")
			: Title(title), Width(width), Height(height), Directory(directory), GlslVersion(glslVersion) {}
    };

    class Application
    {
    public:
        Application(ApplicationProperties& props = ApplicationProperties());
        ~Application();

        static Application* Get() { return m_Instance; }

        void Init();
        void Run();
        void Exit();
        void OnUpdate(float ts = 1.0f);
        void OnEvent(Event& e);
        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        void PopOverlay(Layer* layer);
        
        std::shared_ptr<Renderer> GetRenderer() const { return m_Renderer; }
        std::shared_ptr<GLFWwindow*> GetGlfwWindow() { return m_Window->GetGlfwWindow(); }
        std::shared_ptr<GraphicsContext> GetGraphicsContext() { return m_Context; }

    private:
        ApplicationProperties m_Props;
        LayerStack m_LayerStack;
        Timer m_Timer;
        std::shared_ptr<Window> m_Window = nullptr;
        std::shared_ptr<Renderer> m_Renderer = nullptr;
        std::shared_ptr<GraphicsContext> m_Context = nullptr;
        static Application* m_Instance;
    };

}