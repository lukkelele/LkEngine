#include "LkEngine/Core/Application.h"



namespace LkEngine {

    Application* Application::m_Instance = nullptr;

    // Application::Application(const std::string& title, uint32_t width, uint32_t height)
    Application::Application(ApplicationProperties& props)
        : m_Props(props)
    {
        m_Instance = this;
        m_Window = Window::Create(props.Title.c_str(), props.Width, props.Height);
		m_Renderer = std::make_shared<Renderer>(m_Props.GlslVersion);
    }

    Application::~Application()
    {
    }

    void Application::Init()
    {
		m_Renderer->Init();
        m_Window->Init();
        m_Context = m_Window->GetContext();
    }

    void Application::Run()
    {
    }

    void Application::Exit()
    {
        m_Context->Destroy();
        m_Window->Exit();
    }

    void Application::OnUpdate(float ts)
    {
        m_Renderer->Clear();

        for (auto it = m_LayerStack.rBegin(); it != m_LayerStack.rEnd(); it++)
        {
            Layer* layer = *it;
            layer->OnUpdate(ts);
        }

        GraphicsContext::BeginImGuiFrame();
        for (auto it = m_LayerStack.rBegin(); it != m_LayerStack.rEnd(); it++)
        {
            Layer* layer = *it;
            layer->OnImGuiRender();
        }
        ImGui::ShowMetricsWindow();
        ImGui::ShowStackToolWindow();
        ImGui::Begin("Style");
        ImGui::ShowStyleSelector("StyleSelector");
        ImGui::End();
        GraphicsContext::EndImGuiFrame();

        m_Window->OnUpdate();
    }

    void Application::PushOverlay(Layer* layer)
    {
        // LOG_DEBUG("Pushing overlay -> {0}", layer->GetName());
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer)
    {
        m_LayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void Application::PopOverlay(Layer* layer)
    {
        m_LayerStack.PopOverlay(layer);
        layer->OnDetach();
    }

    void Application::OnEvent(Event& e)
    {
    }

}