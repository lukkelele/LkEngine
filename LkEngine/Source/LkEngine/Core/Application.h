#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/CrashHandler.h"
#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"
#include "LkEngine/Core/IO/File.h"
#include "LkEngine/Core/Math/Math.h"
#include "LkEngine/Core/ThreadManager.h"
#include "LkEngine/Core/MetadataRegistry.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

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

#define LK_EDITOR_ONLY 1

namespace LkEngine {

    class LApplication : public LObject
    {
    public:
        LApplication(const ApplicationSpecification& props = ApplicationSpecification());
        ~LApplication();

        void Initialize();
        void Run();
        void Shutdown();

        void OnEvent(LEvent& e);

        void RenderImGui();

        FORCEINLINE void PushLayer(LLayer* Layer)
        {
			LayerStack.PushLayer(Layer);
        }

        FORCEINLINE void PopLayer(LLayer* Layer)
        {
			LayerStack.PopLayer(Layer);
        }

        FORCEINLINE void PushOverlay(LLayer* Layer)
        {
            LayerStack.PushOverlay(Layer);
        }

        FORCEINLINE void PopOverlay(LLayer* Layer)
        {
            LayerStack.PopOverlay(Layer);
        }

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

        const ApplicationSpecification GetSpecification() const { return m_Specification; }

        FTimestep GetTimestep() const { return Timestep; } // REMOVE ME
        uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

        static LApplication* Get() { return m_Instance; }

        FThreadData Test_ThreadData2{};

    private:
        bool bRunning = false;
        Timer m_Timer;
        LLayerStack LayerStack;
        ApplicationSpecification m_Specification;

        FTimestep Timestep{};
        FTimestep LastTimestep{};

        LMetadataRegistry& MetadataRegistry;
        LThreadManager& ThreadManager;

        TUniquePtr<LWindow> Window;
        TUniquePtr<Input> m_Input;

        TObjectPtr<LRenderer> m_Renderer;
        uint32_t m_CurrentFrameIndex = 0;

        TUniquePtr<LEditorLayer> Editor;
        TObjectPtr<LImGuiLayer> ImGuiLayer;

        TUniquePtr<PhysicsSystem> m_PhysicsSystem;

		std::mutex EventQueueMutex;
		std::queue<std::function<void()>> EventQueue;
		std::vector<FEventCallback> EventCallbacks;

        TObjectPtr<LProject> Project;
        //TObjectPtr<LScene> Scene = nullptr;

        inline static LApplication* m_Instance = nullptr;

        LCLASS(LApplication);
    };

}