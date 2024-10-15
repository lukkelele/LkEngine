#include "LKpch.h"
#include "Application.h"


namespace LkEngine {

    LApplication::LApplication(const ApplicationSpecification& InSpecification)
        : Specification(InSpecification)
        , Log(LLog::Instance())
        , MetadataRegistry(LMetadataRegistry::Get())
        , ThreadManager(LThreadManager::Instance())
    {
        m_Instance = this;
        m_Timer.Reset();

        LCrashHandler::AttachInstance(this);

        LK_CORE_TRACE_TAG("Application", "Getting Metadata Registry");
        LMetadataRegistry& MetadataRegistry = LMetadataRegistry::Get();

        LK_CORE_TRACE_TAG("Application", "Creating window");
        Window = MakeUnique<LWindow>(InSpecification);
        MetadataRegistry.RegisterObject("Window", Window);

        ImGuiLayer = LImGuiLayer::Create();
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
        #if 1
                TObjectPtr<LEditorCamera> Camera = Editor->GetEditorCamera();
                LRenderer::BeginScene(Camera->GetViewProjectionMatrix());

                /* Update layers. */
				for (LLayer* layer : LayerStack)
				{
					layer->OnUpdate(Timestep);
				}

            #if 0 /// DISABLED FOR NOW
				if (Scene)
				{
					Scene->OnRenderEditor(*Camera, Timestep);

                    /* Flush 2D renderer. */
					m_Renderer->GetRenderer2D()->EndScene(); 
				}
            #endif
        #endif
            }
            
			if (Specification.ImGuiEnabled)
			{
				//LRenderer::Submit([Application]() 
				LRenderer::Submit([&Application]() 
                { 
                    Application->RenderImGui(); 
                });

				//LRenderer::Submit([=]()
				LRenderer::Submit([&]()
                { 
                    ImGuiLayer->EndFrame(); 
                });
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

	void LApplication::OnEvent(LEvent& Event)
	{
		EventDispatcher Dispatcher(Event);

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