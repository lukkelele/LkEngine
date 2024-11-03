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

#include "LkEngine/ImGui/ImGuiLayer.h"

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
        LApplication(const ApplicationSpecification& InSpecification = ApplicationSpecification());
        ~LApplication();

        void Initialize();
        void Run();
        void Shutdown();

        bool ReadConfigurationFile();

        void OnEvent(LEvent& e);

        void RenderUI();
        void ProcessEvents();

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

        LString GenerateCrashDump();

        FORCEINLINE LWindow& GetWindow() { return *Window; }
        FORCEINLINE GLFWwindow* GetGlfwWindow() { return Window->GetGlfwWindow(); }

        FORCEINLINE TObjectPtr<LProject> GetProject() { return Project; }

        const ApplicationSpecification& GetSpecification() const 
        { 
            return Specification; 
        }

        FTimestep GetTimestep() const { return Timestep; } /// TODO: MOVE ELSEWHERE
        uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

        FORCEINLINE LPerformanceProfiler* GetPerformanceProfiler()
        {
            return PerformanceProfiler;
        }

        static LApplication* Get() { return Instance; }

    private:
        ApplicationSpecification Specification{};

        bool bRunning = false;

        LTimer Timer;
        FTimestep Timestep{};
        FTimestep LastTimestep{};

        LLog& Log;
        LMetadataRegistry& MetadataRegistry;
        LGarbageCollector& GarbageCollector;
        LLayerStack LayerStack;
        LThreadManager& ThreadManager;

        TUniquePtr<LWindow> Window;

        TObjectPtr<LRenderer> Renderer;
        uint32_t m_CurrentFrameIndex = 0;

        TUniquePtr<LEditorLayer> Editor;
        TObjectPtr<LImGuiLayer> UILayer;

        TUniquePtr<PhysicsSystem> m_PhysicsSystem;

		std::mutex EventQueueMutex;
		std::queue<std::function<void()>> EventQueue;
		std::vector<FEventCallback> EventCallbacks;

        TObjectPtr<LProject> Project{};

        LPerformanceProfiler* PerformanceProfiler{};
        std::unordered_map<const char*, LPerformanceProfiler::FFrameData> ProfilerPreviousFrameData;

        inline static LApplication* Instance = nullptr;

        LCLASS(LApplication);
    };

}