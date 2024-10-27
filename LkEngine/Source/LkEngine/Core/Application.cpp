#include "LKpch.h"
#include "Application.h"

#include "LkEngine/Core/ApplicationSerializer.h"


namespace LkEngine {

    LApplication::LApplication(const ApplicationSpecification& InSpecification)
        : Specification(InSpecification)
        , Log(LLog::Instance())
        , MetadataRegistry(LMetadataRegistry::Get())
        , GarbageCollector(LGarbageCollector::Get())
        , ThreadManager(LThreadManager::Instance())
    {
        Instance = this;
        LCrashHandler::AttachInstance(this);        

        Global::SetRuntimeArguments(Specification.Argc, Specification.Argv);
        GarbageCollector.Initialize();

        LK_CORE_TRACE_TAG("Application", "Creating window");
        Window = MakeUnique<LWindow>(InSpecification);
        MetadataRegistry.RegisterObject("Window", Window);

        /* Read configuration file. */
        ReadConfigurationFile();
    }

    LApplication::~LApplication()
    {
        if (bRunning)
        {
            LK_CORE_INFO_TAG("Application", "Shutting down");
            Shutdown();
        }
    }

    void LApplication::Initialize()
    {
        Window->Initialize();
		Window->SetEventCallback([this](LEvent& Event) 
        { 
            OnEvent(Event); 
        });

        LInput::Initialize();
        
        /* Initialize the renderer. */
        Renderer = TObjectPtr<LRenderer>::Create();
        Renderer->Initialize();

        /* UI layer. */
        UILayer = LImGuiLayer::Create();
        UILayer->Initialize();
        UILayer->SetDarkTheme();

    #if LK_USE_EDITOR
        /* Create and initialize EditorLayer. */
        Editor = MakeUnique<LEditorLayer>();
        Editor->Initialize();
        LayerStack.PushOverlay(Editor.get());
    #endif
    }

    void LApplication::Run()
    {
        bRunning = true;

        LApplication* Application = this;
        GLFWwindow* GlfwWindow = Window->GetGlfwWindow();

        Timer.Reset();
		while (!glfwWindowShouldClose(GlfwWindow))
		{
			Timestep = Timer.GetDeltaTime();

            LInput::Update();
            LRenderer::BeginFrame();

            /** LkEditor */
            if (Editor->IsEnabled())
            {
                TObjectPtr<LEditorCamera> EditorCamera = Editor->GetEditorCamera();
                LRenderer::BeginScene(EditorCamera->GetViewProjectionMatrix());

                /* Update all layers. */
				for (TObjectPtr<LLayer>& Layer : LayerStack)
				{
					Layer->OnUpdate(Timestep);
				}

                LRenderer::EndScene();
            }
            
            /* UI */
			if (Specification.ImGuiEnabled)
			{
				LRenderer::Submit([Application]() 
                { 
                    Application->RenderUI(); 
                });

				LRenderer::Submit([&]() { UILayer->EndFrame(); });
			}

			LRenderer::EndFrame();

			/* Submit the buffer swap. */
			LRenderer::Submit([&]()
			{
				Window->SwapBuffers();
			});

		    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % LRenderer::GetFramesInFlight();
            LastTimestep = Timestep;

            ProcessEvents();
		}
    }

    void LApplication::Shutdown()
    {
        if (bRunning)
        {
            /* Serialize application configuration. */
			LApplicationSerializer Serializer(this);
			Serializer.Serialize(Global::GetEngineConfig());

			Renderer->Shutdown();

			LK_CORE_WARN_TAG("Application", "Window->Shutdown()");
			Window->Shutdown();

            bRunning = false;
        }
    }

    bool LApplication::ReadConfigurationFile()
    {
        LApplicationSerializer Serializer(this);
        Serializer.Deserialize(Global::GetEngineConfig());

        return true;
    }

    /* FIXME */
    LString LApplication::GenerateCrashDump()
    {
        return "[TODO] CRASHDUMP";
    }

    void LApplication::RenderUI()
    {
        /* Bind to default framebuffer before any UI rendering takes place. */
		LFramebuffer::TargetSwapChain();

        UILayer->BeginFrame();

        for (TObjectPtr<LLayer>& Layer : LayerStack)
        {
            Layer->OnImGuiRender();
        }
    }

    /// FIXME: 
	void LApplication::OnEvent(LEvent& Event)
	{
		EventDispatcher Dispatcher(Event);

		for (TObjectPtr<LLayer>& Layer : LayerStack)
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
		LInput::TransitionPressedKeys();
		LInput::TransitionPressedButtons();

		Window->ProcessEvents();

		std::scoped_lock<std::mutex> lock(EventQueueMutex);
		while (EventQueue.size() > 0)
		{
            /* Invoke event from the queue and remove it after. */
            EventQueue.front()();
			EventQueue.pop();
		}
	}

}