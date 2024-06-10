#include "LKpch.h"
#include "Application.h"


namespace LkEngine {

    TObjectPtr<Scene> Test_ScenePtr{};
    TObjectPtr<LEntity> Test_EntityPtr{};

    LApplication::LApplication(const ApplicationSpecification& Specification)
        : m_Specification(Specification)
        , ThreadManager(LThreadManager::Instance())
    {
        m_Instance = this;
        m_Timer.Reset();

        Log::Init("LkEngine.log");

        Window = std::make_unique<LWindow>(Specification);
        m_SelectionContext = std::make_unique<SelectionContext>();

        m_ImGuiLayer = ImGuiLayer::Create();
        m_Debugger = std::make_unique<Debugger>();

        CrashHandler = LCrashHandler::Create(this);

        LVector TestVector_A;
        LVector TestVector_B = LVector(0.0f, 50.0f, 95.0f);
        LK_CORE_FATAL_TAG("Application", "Testing LVector::ToString");
        LK_CORE_FATAL_TAG("Application", "Vector_A -> {}", TestVector_A.ToString().CStr());
        LK_CORE_FATAL_TAG("Application", "Vector_B -> {}", *TestVector_B.ToString());
        //exit(EXIT_SUCCESS);

        Test_EntityPtr = TObjectPtr<LEntity>::Create();

        FThreadStartArgs Test_Thread1_StartArgs{};
        Test_Thread1_StartArgs.bRunAfterCreation = true;

        auto Test_ThreadFunc1 = [](int a, int b)
		{
            int LoopCount = 0;
            while (true)
            {
				LK_CORE_INFO_TAG("Test_Thread1", "Count={}", LoopCount++);
                std::this_thread::sleep_for(4s);
            }
		};
        LK_CORE_WARN("Creating Test_Thread1");
        ThreadManager.CreateThread(Test_Thread1_StartArgs, Test_ThreadFunc1, 1, 3);

        /** Setup testing for thread submission. */
        FThreadStartArgs Test_Thread2_StartArgs{};
        Test_Thread2_StartArgs.bRunAfterCreation = true;
        ThreadManager.CreateThread(Test_Thread2_StartArgs, ThreadFunction_TestCommandQueue, std::ref(Test_ThreadData2));
    }

    LApplication::~LApplication()
    {
        LK_CORE_WARN_TAG("Application", "Terminating application");
        Shutdown();
    }

    void LApplication::Init()
    {
        Window->Init();
		Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        m_RenderContext = Window->GetRenderContext();
        Input::Init();

        m_PhysicsSystem = MakeUnique<PhysicsSystem>();
        m_PhysicsSystem->Init();

        m_Editor = MakeUnique<EditorLayer>();
		m_Editor->SetEventCallback([this](Event& e) { m_Editor->OnEvent(e); });

        m_Renderer = Ref<Renderer>::Create();
        m_Renderer->Init();

        m_ImGuiLayer->Init();
        m_ImGuiLayer->SetDarkTheme();
        m_Debugger->Init();

        m_Editor->Init();

        PushOverlay(m_Editor.get());
    }

    void LApplication::Run()
    {
		while (!glfwWindowShouldClose(Window->GetGlfwWindow()))
		{
            LApplication* Application = this;
			Timestep = m_Timer.GetDeltaTime();

            Input::Update();
            Renderer::BeginFrame();

            /**
             * LkEditor
             */
            if (m_Editor->IsEnabled())
            {
                Ref<LEditorCamera> Camera = m_Editor->GetEditorCamera();
                Renderer::BeginScene(Camera->GetViewProjectionMatrix());

                /* Update layers. */
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(Timestep);
				}

				if (m_Scene)
				{
					m_Scene->OnRenderEditor(*Camera, Timestep);

                    /* Flush 2D renderer. */
					m_Renderer->GetRenderer2D()->EndScene(); 
				}
            }
            
			if (m_Specification.ImGuiEnabled)
			{
				Renderer::Submit([Application]() { Application->RenderImGui(); });
				Renderer::Submit([=]() { m_ImGuiLayer->EndFrame(); });
			}

			Renderer::EndFrame();

			/* Submit buffer swap on render thread. */
			Renderer::Submit([&]()
			{
				Window->SwapBuffers();
			});

            m_PhysicsSystem->Simulate(Timestep);

		    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Renderer::GetFramesInFlight();
            LastTimestep = Timestep;

            ProcessEvents();
		}
    }

    void LApplication::Shutdown()
    {
        LK_CORE_WARN_TAG("Application", "Renderer->Shutdown()");
        m_Renderer->Shutdown();

        LK_CORE_WARN_TAG("Application", "Window->Shutdown()");
        Window->Shutdown();
        LK_CORE_WARN_TAG("Application", "Window shutdown");
    }

    void LApplication::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void LApplication::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void LApplication::PopLayer(Layer* layer)
    {
        m_LayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void LApplication::PopOverlay(Layer* layer)
    {
        m_LayerStack.PopOverlay(layer);
        layer->OnDetach();
    }

	void LApplication::OnEvent(Event& event)
	{
		EventDispatcher Dispatcher(event);

		for (auto LayerIter = m_LayerStack.end(); LayerIter != m_LayerStack.begin(); )
		{
			(*(--LayerIter))->OnEvent(event);
            if (event.Handled)
            {
				break;
            }
		}

        if (event.Handled)
        {
			return;
        }

		for (auto& EventCallback : m_EventCallbacks)
		{
			EventCallback(event);
            if (event.Handled)
            {
				break;
            }
		}
	}

	void LApplication::ProcessEvents()
	{
        if (m_EventQueue.size() > 0)
        {
            LK_CORE_INFO_TAG("Application", "Events in queue {}", m_EventQueue.size());
        }

		Input::TransitionPressedKeys();
		Input::TransitionPressedButtons();

		Window->ProcessEvents();

		std::scoped_lock<std::mutex> lock(m_EventQueueMutex);
		while (m_EventQueue.size() > 0)
		{
			auto& func = m_EventQueue.front();
			func();
			m_EventQueue.pop();
		}
	}

    LString LApplication::GenerateCrashDump()
    {
        /// @FIXME
        return "<< LkEngine CRASHDUMP >>";
    }

    void LApplication::RenderImGui()
    {
        m_ImGuiLayer->BeginFrame();

        for (int i = 0; i < m_LayerStack.Size(); i++)
        {
            m_LayerStack[i]->OnImGuiRender();
        }
    }

    // @FIXME
    void LApplication::AddScene(Scene& scene)
    {
		Input::SetScene(Ref<Scene>(&scene));
    }
    
    // @FIXME
    void LApplication::SetScene(Ref<Scene> scene)
    {
        m_Scene = scene;
		Input::SetScene(scene);
    }


}