#include "LKpch.h"
#include "Application.h"

#include "LkEngine/Core/ApplicationSerializer.h"

#include <nfd.hpp>


namespace LkEngine {

	void Core::Setup(const int Argc, char* Argv[])
	{
		Global::SetRuntimeArguments(Argc, Argv);

		LLog& Logger = LLog::Get();
		Logger.Initialize();

		LK_CORE_INFO("LkEngine Version: {}", LVersion::ToString(LK_ENGINE_VERSION));
	}

	LApplication::LApplication(const FApplicationSpecification& InSpecification)
		: Specification(InSpecification)
		, MetadataRegistry(LMetadataRegistry::Get())
		, GarbageCollector(LGarbageCollector::Get())
		, ThreadManager(LThreadManager::Get())
	{
		LOBJECT_REGISTER(LApplication);
		Instance = this;

		LCrashHandler::AttachInstance(this);
		GarbageCollector.Initialize();

		LLog::RegisterLoggers();
	}

	LApplication::~LApplication()
	{
		if (bRunning)
		{
			LK_CORE_INFO("Shutting down application");
			Shutdown();
		}
	}

	void LApplication::Initialize()
	{
		if (!ReadConfigurationFile(Specification))
		{
			LK_CORE_ERROR("Failed to read configuration file");
		}

		Window = std::make_unique<LWindow>(Specification);

		SetupDirectories();

		LK_VERIFY(NFD::Init());

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
		UILayer = LUILayer::Create();
		UILayer->Initialize();
		UILayer->SetDarkTheme();

	#if LK_ENGINE_EDITOR
		/* Create and initialize EditorLayer. */
		Editor = std::make_unique<LEditorLayer>();
		Editor->Initialize();
		LayerStack.PushOverlay(Editor.get());
	#endif

		PerformanceProfiler = new LPerformanceProfiler();
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
	#if 0
			LRenderer::Submit([&]() { Window->GetSwapChain().BeginFrame(); });
	#endif

			LRenderer::BeginFrame();

	#if LK_ENGINE_EDITOR
			/** LkEditor */
			if (Editor->IsEnabled())
			{
				TObjectPtr<LEditorCamera> EditorCamera = Editor->GetEditorCamera();
				LRenderer::BeginScene(EditorCamera->GetViewProjectionMatrix());

				Editor->RenderViewport();

				/* Update all layers. */
				for (TObjectPtr<LLayer>& Layer : LayerStack)
				{
					Layer->OnUpdate(Timestep);
				}

				LRenderer::EndScene();
			}
	#endif

			/* UI */
			if (Specification.ImGuiEnabled)
			{
				LRenderer::Submit([Application]()
				{
					Application->RenderUI();
				});

				LRenderer::Submit([&]()
				{
					UILayer->EndFrame();
				});
			}

			LRenderer::EndFrame();

			/* Swap buffers. */
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
			Serializer.Serialize(LFileSystem::GetEngineConfig());

			if (TObjectPtr<LProject> Project = LProject::Current())
			{
				TObjectPtr<LScene> ActiveScene = LScene::GetActiveScene();
				if (ActiveScene)
				{
				}

				Project->Save();
			}

			LAssetManager::Destroy();
			LRenderer::Destroy();
			UILayer->Destroy();

			LK_CORE_TRACE_TAG("Application", "Window->Shutdown()");
			Window->Shutdown();

			bRunning = false;
		}
	}

	bool LApplication::ReadConfigurationFile(FApplicationSpecification& InSpecification)
	{
		LApplicationSerializer Serializer(this);
		return Serializer.Deserialize(LFileSystem::GetEngineConfig(), InSpecification);
	}

	void LApplication::SetupDirectories()
	{
		/* Create 'Scenes' directory if it does not exist. */
		if (!std::filesystem::exists("Scenes"))
		{
			LK_INFO("Creating 'Scenes' directory");
			std::filesystem::create_directories("Scenes");
		}
	}

	std::string LApplication::GenerateCrashDump()
	{
		return "NULL";
	}

	void LApplication::RenderUI()
	{
		/* Bind to default framebuffer before any UI rendering takes place. 
		 * TODO: This should not be done here but rather as an initial render submission.
		 */
		LFramebuffer::TargetSwapChain();

		UILayer->BeginFrame();

		for (TObjectPtr<LLayer>& Layer : LayerStack)
		{
			Layer->OnRenderUI();
		}

		UILayer->EndFrame();
	}

	/* FIXME: The event system needs to be re-evaluated to efficiently
	 *        ripple events down to existing layers and overlays. */
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

		std::scoped_lock<std::mutex> ScopedLock(EventQueueMutex);
		while (EventQueue.size() > 0)
		{
			/* Invoke event from the queue and remove it after. */
			EventQueue.front()();
			EventQueue.pop();
		}
	}

}

