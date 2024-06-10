#include "LKpch.h"
#include "Application.h"


namespace LkEngine {

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
        , m_Timestep(0.0f)
        , m_LastTimestep(0.0f)
    {
        m_Instance = this;
        m_Timer.Reset();

        Log::Init("LkEngine.log");

        m_Window = std::make_unique<Window>(specification);
        m_SelectionContext = std::make_unique<SelectionContext>();

        m_ImGuiLayer = ImGuiLayer::Create();
        m_Debugger = std::make_unique<Debugger>();
    }

    Application::~Application()
    {
        LK_CORE_WARN_TAG("Application", "Terminating application");
        Shutdown();
    }

    void Application::Init()
    {
        m_Window->Init();
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        m_RenderContext = m_Window->GetRenderContext();
        Input::Init();

        m_PhysicsSystem = std::make_unique<PhysicsSystem>();
        m_PhysicsSystem->Init();

        m_Editor = std::make_unique<EditorLayer>();
		m_Editor->SetEventCallback([this](Event& e) { m_Editor->OnEvent(e); });

        m_Renderer = Ref<Renderer>::Create();
        m_Renderer->Init();

        m_ImGuiLayer->Init();
        m_ImGuiLayer->SetDarkTheme();
        m_Debugger->Init();

        m_Editor->Init();

        PushOverlay(m_Editor.get());
    }

    void Application::Run()
    {
		while (!glfwWindowShouldClose(m_Window->GetGlfwWindow()))
		{
            Application* app = this;
			m_Timestep = m_Timer.GetDeltaTime();

            Renderer::BeginFrame();

            Input::Update();

            if (m_Editor->IsEnabled())
            {
                Renderer::BeginScene(m_Editor->GetEditorCamera()->GetViewProjectionMatrix());
            }

            // Update layers
            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate(m_Timestep);
            }
            
            // TODO: Use 'MainCameraEntity' here
            if (m_Scene)
            {
                if (m_Editor->IsEnabled())
                {
                    m_Scene->OnRenderEditor(*m_Editor->GetEditorCamera(), m_Timestep);
                }
                else
                {
                    m_Scene->OnRender(m_Scene->GetMainCamera(), m_Timestep);
                }
                m_Renderer->GetRenderer2D()->EndScene(); // Flush 2D renderer
            }

			if (m_Specification.ImGuiEnabled)
			{
				Renderer::Submit([app]() { app->RenderImGui(); });
				Renderer::Submit([=]() { m_ImGuiLayer->EndFrame(); });
			}
			Renderer::EndFrame();

			// On Render thread
			Renderer::Submit([&]()
			{
				m_Window->SwapBuffers();
			});

            m_PhysicsSystem->Simulate(m_Timestep);

		    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Renderer::GetFramesInFlight();
            m_LastTimestep = m_Timestep;

            ProcessEvents();
		}
    }

    void Application::Shutdown()
    {
        LK_CORE_WARN_TAG("Application", "Renderer->Shutdown()");
        m_Renderer->Shutdown();

        LK_CORE_WARN_TAG("Application", "Window->Shutdown()");
        m_Window->Shutdown();
        LK_CORE_WARN_TAG("Application", "Window shutdown");
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

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(event);
			if (event.Handled)
				break;
		}

		if (event.Handled)
			return;

		for (auto& eventCallback : m_EventCallbacks)
		{
			eventCallback(event);
			if (event.Handled)
				break;
		}
	}

	void Application::ProcessEvents()
	{
        if (m_EventQueue.size() > 0)
            LK_CORE_INFO_TAG("Application", "Events in queue {}", m_EventQueue.size());

		Input::TransitionPressedKeys();
		Input::TransitionPressedButtons();

		m_Window->ProcessEvents();

		std::scoped_lock<std::mutex> lock(m_EventQueueMutex);
		while (m_EventQueue.size() > 0)
		{
			auto& func = m_EventQueue.front();
			func();
			m_EventQueue.pop();
		}
	}

    void Application::RenderImGui()
    {
        m_ImGuiLayer->BeginFrame();

        //if (m_Editor->IsEnabled()) m_Editor->OnImGuiRender();

        for (int i = 0; i < m_LayerStack.Size(); i++)
        {
            m_LayerStack[i]->OnImGuiRender();
        }
    }

    void Application::AddScene(Scene& scene)
    {
		Input::SetScene(Ref<Scene>(&scene));
    }
    
    void Application::SetScene(Ref<Scene> scene)
    {
        m_Scene = scene;
		Input::SetScene(scene);
    }


}