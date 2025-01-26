#include "LKpch.h"
#include "Application.h"

#include "LkEngine/Core/ApplicationSerializer.h"

#include "LkEngine/Editor/EditorLayer.h"

#include <nfd.hpp>


namespace LkEngine {

	void Core::Setup(const int Argc, char* Argv[])
	{
		Global::SetRuntimeArguments(Argc, Argv);

		LLog& Logger = LLog::Get();
		Logger.Initialize();

		LK_CORE_INFO("Starting LkEngine ({})", LVersion::ToString(LK_ENGINE_VERSION));
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
			Shutdown();
		}
	}

	void LApplication::Initialize()
	{
		if (!ReadConfigurationFile(Specification))
		{
			LK_CORE_ERROR("Failed to read configuration file");
		}

		Window = TObjectPtr<LWindow>::Create(Specification);

		SetupDirectories();

		LK_CORE_VERIFY(NFD::Init());

		Window->Initialize();
		/* TODO: The event system is to be updated. */
		Window->SetEventCallback([this](LEvent& Event)
		{
			OnEvent(Event);
		});

		LInput::Initialize();
		LSelectionContext::Get(); /* TODO: REMOVE */

		/* Initialize the renderer. */
		Renderer = TObjectPtr<LRenderer>::Create();
		Renderer->Initialize();

		/* UI layer. */
		UILayer = LUILayer::Create();
		UILayer->Initialize();
		UILayer->SetDarkTheme();

	#if LK_ENGINE_EDITOR
		TObjectPtr<LEditorLayer> Editor = TObjectPtr<LEditorLayer>::Create();
		LayerStack.PushOverlay(Editor);
	#endif

		LK_CORE_DEBUG_TAG("Application", "Creating performance profiler");
		PerformanceProfiler = new LPerformanceProfiler();
	}

	void LApplication::Run()
	{
		bRunning = true;

		LApplication* Application = this;
		GLFWwindow* GlfwWindow = Window->GetGlfwWindow();
		LK_CORE_DEBUG_TAG("Application", "Entering main loop");

		Timer.Reset();
		while (!glfwWindowShouldClose(GlfwWindow))
		{
			const float DeltaTime = Timer.GetDeltaTime();

			LInput::Update();
			ProcessEvents();

			LRenderer::BeginFrame();

			for (TObjectPtr<LLayer>& Layer : LayerStack)
			{
				Layer->OnUpdate(DeltaTime);
			}

			/* UI. */
			{
				LRenderer::Submit([Application]() { Application->RenderUI(); });

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

			CurrentFrameIndex = (CurrentFrameIndex + 1) % LRenderer::GetFramesInFlight();

			LastDeltaTime = DeltaTime;

			LInput::ClearReleased();
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

				LK_CORE_DEBUG_TAG("Application", "Saving project: {}", Project->GetName());
				Project->Save();
			}

			LayerStack.Destroy();

			LK_CORE_DEBUG_TAG("Application", "Destroying renderer");
			LRenderer::Destroy();
			UILayer->Destroy();

			LAssetManager::Destroy();

			Window->Destroy();
			Window.Release();

			bRunning = false;
		}

		LK_CORE_INFO("Exiting LkEngine");
	}

	bool LApplication::ReadConfigurationFile(FApplicationSpecification& InSpecification)
	{
		LApplicationSerializer Serializer(this);
		return Serializer.Deserialize(LFileSystem::GetEngineConfig(), InSpecification);
	}

	void LApplication::SetupDirectories()
	{
		// TODO: Check the pathing here before creating any directories.
		// Should be placed in the assets directory.

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

	void LApplication::OnEvent(LEvent& Event)
	{
		LEventDispatcher Dispatcher(Event);

		for (TObjectPtr<LLayer>& Layer : LayerStack)
		{
			Layer->OnEvent(Event);
			if (Event.bHandled)
			{
				break;
			}
		}

		if (Event.bHandled)
		{
			return;
		}

		for (FEventCallback& EventCallback : EventCallbacks)
		{
			EventCallback(Event);
			if (Event.bHandled)
			{
				break;
			}
		}

		LK_CORE_ERROR("Event '{}' was not handled", Event.GetName());
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

	const char* LApplication::GetPlatformName()
	{
	#if defined(LK_PLATFORM_WINDOWS)
		return "Windows";
	#elif defined(LK_PLATFORM_LINUX)
		return "Linux";
	#else
		LK_CORE_VERIFY(false);
		return nullptr;
	#endif
	}

	const char* LApplication::GetConfigurationName()
	{
	#if defined(LK_DEBUG)
		return "Debug";
	#elif defined(LK_RELEASE)
		return "Release";
	#elif defined(LK_DIST)
		return "Dist";
	#else
		LK_CORE_VERIFY(false);
		return nullptr;
	#endif
	}

}

