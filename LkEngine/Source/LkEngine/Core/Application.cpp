#include "LKpch.h"
#include "Application.h"


namespace LkEngine {

    TObjectPtr<LScene> Test_ScenePtr{};
    TObjectPtr<LEntity> Test_EntityPtr{};

    LApplication::LApplication(const ApplicationSpecification& Specification)
        : m_Specification(Specification)
        , MetadataRegistry(LMetadataRegistry::Get())
        , ThreadManager(LThreadManager::Instance())
    {
        m_Instance = this;
        m_Timer.Reset();

        LLog::Initialize("LkEngine.log");
        LK_CORE_INFO_TAG("Application", "Starting LkEngine");

        LK_CORE_WARN_TAG("Application", "Getting Metadata Registry");
        LMetadataRegistry& MetadataRegistry = LMetadataRegistry::Get();

        for (const auto& [ StaticClassName, StaticClassMetadata ] : MetadataRegistry.GetStaticClassRegistry())
        {
            LK_CORE_INFO(" Registered Static Class: \"{}\"", StaticClassName);
        }

        LK_CORE_DEBUG_TAG("Application", "Creating window");
        Window = MakeUnique<LWindow>(Specification);
        MetadataRegistry.RegisterObject("Window", Window);

        ImGuiLayer = LImGuiLayer::Create();

        LCrashHandler::AttachInstance(this);

#if 0 // TEST THREAD MANAGER
        /** Setup testing for thread submission. */
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
        FThreadStartArgs Test_Thread2_StartArgs{};
        Test_Thread2_StartArgs.bRunAfterCreation = true;
        ThreadManager.CreateThread(Test_Thread2_StartArgs, ThreadFunction_TestCommandQueue, std::ref(Test_ThreadData2));
#endif
    }

    LApplication::~LApplication()
    {
        if (bRunning)
        {
            LK_CORE_WARN_TAG("Application", "Terminating application");
            Shutdown();
        }
        else
        {
            LK_CORE_WARN_TAG("Application", "Already invoked application shutdown");
        }
    }

    void LApplication::Initialize()
    {
        Window->Init();
		Window->SetEventCallback([this](LEvent& Event) { OnEvent(Event); });

        Input::Init();

        m_PhysicsSystem = MakeUnique<PhysicsSystem>();
        m_PhysicsSystem->Init();

        /* Setup EditorLayer. */
        Editor = MakeUnique<LEditorLayer>();
		Editor->SetEventCallback([this](LEvent& Event) 
        { 
            Editor->OnEvent(Event); 
        });

        /* Initialize the renderer. */
        m_Renderer = TObjectPtr<LRenderer>::Create();
        m_Renderer->Initialize();

        ImGuiLayer->Initialize();
        ImGuiLayer->SetDarkTheme();

        Editor->Initialize();

        PushOverlay(Editor.get());
    }

    void LApplication::Run()
    {
        bRunning = true;
		while (!glfwWindowShouldClose(Window->GetGlfwWindow()))
		{
            LApplication* Application = this;
			Timestep = m_Timer.GetDeltaTime();

            Input::Update();
            LRenderer::BeginFrame();

            /**
             * LkEditor
             */
            if (Editor->IsEnabled())
            {
        #if 0
                TObjectPtr<LEditorCamera> Camera = Editor->GetEditorCamera();
                LRenderer::BeginScene(Camera->GetViewProjectionMatrix());

                /* Update layers. */
				for (LLayer* layer : LayerStack)
				{
					layer->OnUpdate(Timestep);
				}

				if (Scene)
				{
					Scene->OnRenderEditor(*Camera, Timestep);

                    /* Flush 2D renderer. */
					m_Renderer->GetRenderer2D()->EndScene(); 
				}
        #endif
            }
            
			if (m_Specification.ImGuiEnabled)
			{
				LRenderer::Submit([Application]() { Application->RenderImGui(); });
				LRenderer::Submit([=]() { ImGuiLayer->EndFrame(); });
			}

			LRenderer::EndFrame();

			/* Submit buffer swap on render thread. */
			LRenderer::Submit([&]()
			{
				Window->SwapBuffers();
			});

            m_PhysicsSystem->Simulate(Timestep);

		    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % LRenderer::GetFramesInFlight();
            LastTimestep = Timestep;

            ProcessEvents();
		}
    }

    void LApplication::Shutdown()
    {
        if (bRunning)
        {
			LK_CORE_WARN_TAG("Application", "Renderer->Shutdown()");
			m_Renderer->Shutdown();

			LK_CORE_WARN_TAG("Application", "Window->Shutdown()");
			Window->Shutdown();
        }
    }

#if 0
    void LApplication::PushLayer(LLayer* layer)
    {
        LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void LApplication::PushOverlay(LLayer* layer)
    {
        LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void LApplication::PopLayer(LLayer* layer)
    {
        LayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void LApplication::PopOverlay(LLayer* layer)
    {
        LayerStack.PopOverlay(layer);
        layer->OnDetach();
    }
#endif

	void LApplication::OnEvent(LEvent& Event)
	{
		EventDispatcher Dispatcher(Event);

		//for (auto LayerIter = LayerStack.end(); LayerIter != LayerStack.begin(); )
		for (LLayer* Layer : LayerStack)
		{
			Layer->OnEvent(Event);
            if (Event.Handled)
            {
				break;
            }
		}

        if (Event.Handled)
        {
			return;
        }

		for (FEventCallback& Callback : EventCallbacks)
		{
			Callback(Event);
            if (Event.Handled)
            {
				break;
            }
		}
	}

	void LApplication::ProcessEvents()
	{
		Input::TransitionPressedKeys();
		Input::TransitionPressedButtons();

		Window->ProcessEvents();

		std::scoped_lock<std::mutex> lock(EventQueueMutex);
		while (EventQueue.size() > 0)
		{
            /* Invoke event from the queue and remove it after. */
            EventQueue.front()();
			EventQueue.pop();
		}
	}

    /* FIXME */
    LString LApplication::GenerateCrashDump()
    {
        return "<< LkEngine CRASHDUMP >>";
    }

    void LApplication::RenderImGui()
    {
        ImGuiLayer->BeginFrame();

        for (int i = 0; i < LayerStack.Size(); i++)
        {
            LayerStack[i]->OnImGuiRender();
        }
    }

}