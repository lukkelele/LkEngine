#include "LKpch.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        m_Instance = this;
        Logger::Init("LkEngine.log");
        m_Window = Window::Create(specification.Title.c_str(), specification.Width, specification.Height);
        m_Timer.Reset();

        m_Debugger = new Debugger();
    }

    Application::~Application()
    {
        LOG_WARN("Terminating application");
    }

    void Application::Init()
    {
        m_AssetRegistry.Clear();
        m_Window->Init(LK_SHADER_VERSION);
        m_GraphicsContext = m_Window->GetContext();

        m_PhysicsSystem = new PhysicsSystem();
        m_PhysicsSystem->Init();

        m_Input = std::make_shared<Input>(this);
        Keyboard::Init();
        Mouse::Init();

        m_Renderer = std::make_shared<Renderer>();
        m_Renderer->Init();
        m_Debugger->Init();

        m_EditorLayer = std::make_shared<EditorLayer>();
        m_LayerStack.PushOverlay(&*m_EditorLayer);
    }

    void Application::Run()
    {
		while (!glfwWindowShouldClose(m_Window->GetGlfwWindow()))
		{
            Application* app = this; // Multiple threads
			Timestep ts = m_Timer.GetDeltaTime();

            m_Input->OnUpdate();

            Renderer::BeginFrame();
            {
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(ts);
            }

            if (m_Scene)
            {
                if (m_EditorLayer->IsEnabled())
                    m_Scene->OnRenderEditor(*m_EditorLayer->GetEditorCamera(), ts);
                else
                    m_Scene->OnRender(m_Scene->GetCamera(), ts);
            }

            if (m_Specification.ImGuiEnabled)
            {
			    Renderer::Submit([app] { app->RenderImGui(); });
				Renderer::Submit([=] { m_GraphicsContext->EndImGuiFrame(); });
            }

            m_PhysicsSystem->Simulate(ts);

            Renderer::EndFrame();
            Renderer::Submit([&] { m_Window->SwapBuffers(); });
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
            m_LayerStack[i]->OnImGuiRender();
    }

    void Application::AddScene(Scene& scene)
    {
		Input::SetScene(scene);
        m_Scenes[Scene::GetSceneCount()] = std::shared_ptr<Scene>(&scene);
        LOG_DEBUG("Added scene: {}, scene count: {}", scene.GetName(), Scene::GetSceneCount());
    }


}