#include "LkEngine/Core/Application.h"



namespace LkEngine {

    Application* Application::m_Instance = nullptr;

    Application::Application(ApplicationProperties& props)
        : m_Props(props)
    {
        m_Instance = this;
        m_Window = Window::Create(props.Title.c_str(), props.Width, props.Height);
		m_Renderer = std::make_shared<Renderer>(m_Props.GlslVersion);
        m_Timer.Reset();
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
		while (!glfwWindowShouldClose(*m_Window->GetGlfwWindow()))
		{
			float deltaTime = m_Timer.GetDeltaTime();
			OnUpdate(deltaTime);
		}
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
        GraphicsContext::EndImGuiFrame();

        m_Window->OnUpdate();
    }

    void Application::PushLayer(Layer* layer)
    {
        // LOG_DEBUG("Pushing layer -> {0}", layer->GetName());
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
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