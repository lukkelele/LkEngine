#include "LKpch.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    Application::Application(const ApplicationProperties& props)
        : m_Properties(props)
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
        m_GraphicsContext = m_Window->GetContext();
        m_Input = Input::Create(this);
        m_Renderer = Renderer::Create();
        
        m_Renderer->Init();
        m_Scene = Scene::Create("BaseScene"); // Base Scene
        LOG_INFO("Created scene: {}", m_Scene->GetName());
        m_EditorLayer = EditorLayer::Create(*m_Scene);
        m_LayerStack.PushOverlay(&*m_EditorLayer);

        m_TextureLibrary = TextureLibrary::Create("assets/img");
        m_TextureLibrary->LoadTextures();
    }

    void Application::Run()
    {
		while (!glfwWindowShouldClose(m_Window->GetGlfwWindow()))
		{
            Application* app = this; // Multiple threads
			float ts = m_Timer.GetDeltaTime();
            m_Input->OnUpdate();

            Renderer::BeginFrame();
            {
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(ts);
            }

            if (m_Properties.ImGuiEnabled)
            {
                auto& renderer = m_Renderer;
			    Renderer::Submit([app]() { app->RenderImGui(); });
				Renderer::Submit([=]() { m_GraphicsContext->EndImGuiFrame(); });
            }
            Renderer::EndFrame();

            m_Window->OnUpdate();
		}
    }

    void Application::Exit()
    {
        m_GraphicsContext->Destroy();
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
        e.HandleEvent();
    }

    void Application::RenderImGui()
    {
        m_GraphicsContext->BeginImGuiFrame();
        for (int i = 0; i < m_LayerStack.Size(); i++)
        {
            m_LayerStack[i]->OnImGuiRender();
        }
    }

    bool Application::IsKeyboardEnabled()
    {
        return m_Scene->GetActiveCamera()->IsKeyboardEnabled();
    }

    bool Application::IsMouseEnabled()
    {
        return m_Scene->GetActiveCamera()->IsMouseEnabled();
    }

    void Application::AddScene(Scene& scene)
    {
		Input::SetScene(scene);
        m_Scenes[Scene::GetSceneCount()] = std::shared_ptr<Scene>(&scene);
        LOG_DEBUG("Added scene: {}, scene count: {}", scene.GetName(), Scene::GetSceneCount());
    }


}