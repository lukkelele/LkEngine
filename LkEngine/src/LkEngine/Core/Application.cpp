#include "LKpch.h"
#include "LkEngine/Core/Application.h"

#include "LkEngine/UI/OpenGLImGui.h" // TODO: Move me


namespace LkEngine {

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        m_Instance = this;
        Logger::Init("LkEngine.log");
        m_Window = Window::Create(specification.Title.c_str(), specification.Width, specification.Height);
        m_Timer.Reset();

        m_Debugger = new Debugger();

        m_Timestep = 0.0f;
        m_LastTimestep = 0.0f;
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

        m_Editor = std::make_shared<Editor>();

		//UI::InitOpenGLImGui(); // Crashes sometimes, not always for some awesome reason
    }

    void Application::Run()
    {
		while (!glfwWindowShouldClose(m_Window->GetGlfwWindow()))
		{
            Application* app = this; // Multiple threads
			m_Timestep = m_Timer.GetDeltaTime();

            m_Input->OnUpdate();

            Renderer::BeginFrame();
            {
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(m_Timestep);
            }

            if (m_Scene)
            {
                if (m_Editor->IsEnabled())
                    m_Scene->OnRenderEditor(*m_Editor->GetEditorCamera(), m_Timestep);
                else
                    m_Scene->OnRender(m_Scene->GetCamera(), m_Timestep);
            }

            if (m_Specification.ImGuiEnabled)
            {
			    Renderer::Submit([app] { app->RenderImGui(); });
				Renderer::Submit([=] { m_GraphicsContext->EndImGuiFrame(); });
            }

            m_PhysicsSystem->Simulate(m_Timestep);

            Renderer::EndFrame();
            Renderer::Submit([&] { m_Window->SwapBuffers(); });

            m_LastTimestep = m_Timestep;
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
        if (m_Editor->IsEnabled())
            m_Editor->RenderImGui();
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