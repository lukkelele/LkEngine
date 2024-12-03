/******************************************************************
 * LApplication
 *
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/Globals.h"

#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/CrashHandler.h"
#include "LkEngine/Core/Time/Timer.h"
#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"
#include "LkEngine/Core/IO/File.h"
#include "LkEngine/Core/Math/Math.h"
#include "LkEngine/Core/ThreadManager.h"
#include "LkEngine/Core/MetadataRegistry.h"
#include "LkEngine/Core/Memory/GarbageCollector.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Input/Input.h"

#include "LkEngine/UI/UILayer.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Input/Keyboard.h"

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Physics/PhysicsSystem.h"

#define LK_USE_EDITOR 1

namespace LkEngine {

	class LApplication : public LObject
	{
	public:
		LApplication(const FApplicationSpecification& InSpecification = FApplicationSpecification());
		~LApplication();

		virtual void Initialize();
		virtual void Run();
		virtual void Shutdown();

		bool ReadConfigurationFile(FApplicationSpecification& InSpecification);
		void SetupDirectories();

		void OnEvent(LEvent& e);

		void RenderUI();
		void ProcessEvents();

		FORCEINLINE void PushLayer(TObjectPtr<LLayer> InLayer)
		{
			LayerStack.PushLayer(InLayer);
		}

		/// TODO: Add event category to bundle the callback with.
		FORCEINLINE void AddEventCallback(const FEventCallback& EventCallback)
		{
			EventCallbacks.push_back(EventCallback);
		}

		template<typename EventFunction>
		void QueueEvent(EventFunction&& Event)
		{
			EventQueue.push(Event);
		}

		template<typename TEvent, bool bDispatchImmediately = false, typename... TEventArgs>
		void DispatchEvent(TEventArgs&&... Args)
		{
			std::shared_ptr<TEvent> Event = MakeShared<TEvent>(std::forward<TEventArgs>(Args)...);
			if constexpr (bDispatchImmediately)
			{
				OnEvent(*Event);
			}
			else
			{
				std::scoped_lock<std::mutex> ScopedLock(EventQueueMutex);
				EventQueue.push([Event]()
				{
					LApplication::Get()->OnEvent(*Event);
				});
			}
		}

		std::string GenerateCrashDump();

		FORCEINLINE LWindow& GetWindow() { return *Window; }
		FORCEINLINE GLFWwindow* GetGlfwWindow() { return Window->GetGlfwWindow(); }

		FORCEINLINE const FApplicationSpecification& GetSpecification() const { return Specification; }

		FTimestep GetTimestep() const { return Timestep; } /// TODO: MOVE ELSEWHERE
		uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

		FORCEINLINE LPerformanceProfiler* GetPerformanceProfiler()
		{
			return PerformanceProfiler;
		}

		static LApplication* Get() { return Instance; }

	private:
		FApplicationSpecification Specification{};

		bool bRunning = false;

		LTimer Timer;
		FTimestep Timestep{};
		FTimestep LastTimestep{};

		LMetadataRegistry& MetadataRegistry;
		LGarbageCollector& GarbageCollector;
		LThreadManager& ThreadManager;

		TUniquePtr<LWindow> Window;
		LLayerStack LayerStack;

		TObjectPtr<LRenderer> Renderer;
		uint32_t m_CurrentFrameIndex = 0;

		TObjectPtr<LUILayer> UILayer;
		TUniquePtr<LEditorLayer> Editor;

		TUniquePtr<PhysicsSystem> m_PhysicsSystem;

		std::mutex EventQueueMutex;
		std::queue<std::function<void()>> EventQueue;
		std::vector<FEventCallback> EventCallbacks;

		LPerformanceProfiler* PerformanceProfiler{};
		std::unordered_map<const char*, LPerformanceProfiler::FFrameData> ProfilerPreviousFrameData;

		inline static LApplication* Instance = nullptr;

		LCLASS(LApplication);
	};

}
