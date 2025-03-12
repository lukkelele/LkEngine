/******************************************************************
 * LApplication
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Version.h"

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/CrashHandler.h"
#include "LkEngine/Core/Time/Timer.h"
#include "LkEngine/Core/IO/File.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Math/Math.h"
/** @fixme: FIXME */
///#include "LkEngine/Core/ThreadManager.h"
#include "LkEngine/Core/MetadataRegistry.h"
#include "LkEngine/Core/Memory/GarbageCollector.h"
#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"
#include "LkEngine/Core/Event/EventQueue.h"
#include "LkEngine/Core/Input/Input.h"
#include "LkEngine/Core/Input/Mouse.h"
#include "LkEngine/Core/Input/Keyboard.h"

#include "LkEngine/Renderer/UI/UILayer.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/SceneRenderer.h"

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Physics/PhysicsSystem.h"


namespace LkEngine {

	LK_DECLARE_MULTICAST_DELEGATE(FOnEngineShutdown);

	namespace Core 
	{
		extern FOnEngineShutdown OnEngineShutdown;

		/**
		 * Setup necessary engine components (logging, global config).
		 */
		void Setup(const int Argc, char* Argv[]);
	}

	class LApplication : public LObject
	{
	public:
		LApplication(const FApplicationSpecification& InSpecification = FApplicationSpecification());
		~LApplication();

		virtual void Initialize();
		virtual void Run();
		virtual void Shutdown();

		FORCEINLINE static LApplication& Get() { return *Instance; }

		bool ReadConfigurationFile(FApplicationSpecification& InSpecification);

		void RenderUI();
		void ProcessEvents();

		static const char* GetPlatformName();
		static const char* GetConfigurationName();

		FORCEINLINE void PushLayer(TObjectPtr<LLayer> InLayer)
		{
			LayerStack.PushLayer(InLayer);
		}

		void OnEvent(LEvent& Event);

		template<typename TEvent, bool DispatchImmediately = false, typename... TEventArgs>
		void DispatchEvent(TEventArgs&&... Args)
		{
			std::shared_ptr<TEvent> Event = std::make_shared<TEvent>(std::forward<TEventArgs>(Args)...);
			if constexpr (DispatchImmediately)
			{
				OnEvent(*Event);
			}
			else
			{
				/* Queue the event. */
				//std::scoped_lock<std::mutex> ScopedLock(EventQueueMutex);
				//EventQueue.push([Event]() { LApplication::Get()->OnEvent(*Event); });
				CoreEventQueue.Add([Event]() { LApplication::Get().OnEvent(*Event); });
			}
		}

		FORCEINLINE LWindow& GetWindow() { return *Window; }
		uint32_t GetCurrentFrameIndex() const { return CurrentFrameIndex; }

		FORCEINLINE const FApplicationSpecification& GetSpecification() const { return Specification; }

		LPerformanceProfiler* GetPerformanceProfiler() { return PerformanceProfiler; }

		FORCEINLINE float GetDeltaTime() const { return LastDeltaTime; }
		std::string GenerateCrashDump();

	private:
		bool bRunning = false;
		FApplicationSpecification Specification{};

		LTimer Timer;
		float LastDeltaTime = 0.0f;

		LMetadataRegistry& MetadataRegistry;
		LGarbageCollector& GarbageCollector;
		/** @fixme: FIXME */
		///LThreadManager& ThreadManager;
		LEventQueue CoreEventQueue;

		TObjectPtr<LWindow> Window{};
		LLayerStack LayerStack;

		/** @fixme: FIXME */
		//TObjectPtr<LRenderer> Renderer;
		uint32_t CurrentFrameIndex = 0;

		TObjectPtr<LUILayer> UILayer{};

		LPerformanceProfiler* PerformanceProfiler{};
		std::unordered_map<const char*, LPerformanceProfiler::FFrameData> ProfilerPreviousFrameData;

		inline static LApplication* Instance = nullptr;

		LCLASS(LApplication);
	};

}
