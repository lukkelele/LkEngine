#include "LKpch.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    Application* Application::m_Instance = nullptr;

    Application::Application(ApplicationProperties& props)
        : m_Props(props)
    {
        m_Instance = this;
        Logger::Init("LkEngine.log", "Core", "Client");
        m_Window = Window::Create(props.Title.c_str(), props.Width, props.Height);
        m_Timer.Reset();
    }

    Application::~Application()
    {
    }

    void Application::Init()
    {
        m_Window->Init();
        m_Context = m_Window->GetContext();
        m_Input = create_s_ptr<Input>(this);
        m_Input->Init();

        m_Scene = create_s_ptr<Scene>();
        m_EditorLayer = create_s_ptr<EditorLayer>(m_Scene);
        m_LayerStack.PushOverlay(&*m_EditorLayer);
    }

    void Application::Run()
    {
		while (!glfwWindowShouldClose(*m_Window->GetGlfwWindow()))
		{
			float ts = m_Timer.GetDeltaTime();
            glm::vec2 mousePos = Mouse::GetMousePos();

            auto raycastResults = Physics2D::Raycast(m_Scene, mousePos, mousePos);

            m_Renderer->Clear();

            for (auto it = m_LayerStack.rBegin(); it != m_LayerStack.rEnd(); it++)
            {
                Layer* layer = *it;
                layer->OnUpdate(ts);
            }

            m_Context->BeginImGuiFrame();
            for (auto it = m_LayerStack.rBegin(); it != m_LayerStack.rEnd(); it++)
            {
                Layer* layer = *it;
                layer->OnImGuiRender();
            }
            m_Context->EndImGuiFrame();

            m_Window->OnUpdate();
		}
    }

    void Application::Exit()
    {
        m_Context->Destroy();
        m_Window->Exit();
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
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

    bool Application::IsKeyboardEnabled()
    {
        return m_Scene->GetActiveCamera()->IsKeyboardEnabled();
    }

    bool Application::IsMouseEnabled()
    {
        return m_Scene->GetActiveCamera()->IsMouseEnabled();
    }

}