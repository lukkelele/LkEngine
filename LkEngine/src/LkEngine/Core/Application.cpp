#include "LKpch.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    Application::Application(const ApplicationProperties& props)
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
        m_Window->Init(LK_SHADER_VERSION);
        m_Context = m_Window->GetContext();
        m_Input = Input::Create(this);
        m_Renderer = Renderer::Create();
        
        m_Renderer->Init();
        m_Scene = Scene::Create();
        m_EditorLayer = EditorLayer::Create(*m_Scene);
        m_LayerStack.PushOverlay(&*m_EditorLayer);

        m_TextureLibrary = TextureLibrary::Create("assets/img");
        m_TextureLibrary->LoadTextures();
    }

    void Application::Run()
    {
		while (!glfwWindowShouldClose(m_Window->GetGlfwWindow()))
		{
			float ts = m_Timer.GetDeltaTime();
            glm::vec2 mousePos = Mouse::GetMousePos();

            auto raycastResults = Physics2D::Raycast(*m_Scene, mousePos, mousePos);

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
        LOG_DEBUG("Pushing layer \"{}\"", layer->GetName());
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
        e.HandleEvent();
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