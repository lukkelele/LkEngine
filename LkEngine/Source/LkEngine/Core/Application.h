#pragma once

#include "LkEngine/Core/PlatformDetection.h"
#include "LkEngine/Core/Base.h"
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

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Input/Input.h"

#include "LkEngine/ImGui/ImGuiLayer.h"

#include "LkEngine/Debug/Debugger.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Input/Keyboard.h"

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Physics/PhysicsSystem.h"


namespace LkEngine {

    class LApplication : public LObject
    {
    public:
        LApplication(const ApplicationSpecification& props = ApplicationSpecification());
        ~LApplication();

        void Init();
        void Run();
        void Shutdown();
        void OnEvent(Event& e);

        void RenderImGui();
        void AddScene(Scene& scene);
        void SetScene(Ref<Scene> scene);

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        void PopOverlay(Layer* layer);

        void ProcessEvents();
        void AddEventCallback(const EventCallbackFn& eventCallback) { m_EventCallbacks.push_back(eventCallback); }

        template<typename Func>
        void QueueEvent(Func&& func)
        {
            m_EventQueue.push(func);
        }

        template<typename TEvent, bool DispatchImmediately = false, typename... TEventArgs>
        void DispatchEvent(TEventArgs&&... args)
        {
            std::shared_ptr<TEvent> event = std::make_shared<TEvent>(std::forward<TEventArgs>(args)...);
            if constexpr (DispatchImmediately)
            {
                OnEvent(*event);
            }
            else
            {
                std::scoped_lock<std::mutex> lock(m_EventQueueMutex);
                m_EventQueue.push([event]() { LApplication::Get()->OnEvent(*event); });
            }
        }

        LString GenerateCrashDump();

        LWindow& GetWindow() { return *Window; }
        GLFWwindow* GetGlfwWindow() { return Window->GetGlfwWindow(); }
        Ref<Renderer> GetRenderer() const { return m_Renderer; }
        Ref<Project> GetProject() { return m_Project; }
        Ref<RenderContext> GetRenderContext() { return m_RenderContext; }
        const ApplicationSpecification GetSpecification() const { return m_Specification; }

        FTimestep GetTimestep() const { return Timestep; } // REMOVE ME

        uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

        static LApplication* Get() { return m_Instance; }

        FThreadData Test_ThreadData2{};

    private:
        Timer m_Timer;
        LayerStack m_LayerStack;
        ApplicationSpecification m_Specification;

        FTimestep Timestep{};
        FTimestep LastTimestep{};

        TUniquePtr<LCrashHandler> CrashHandler;
        LThreadManager& ThreadManager;

        TUniquePtr<LWindow> Window;
        TUniquePtr<Input> m_Input;

        Ref<Renderer> m_Renderer;
        uint32_t m_CurrentFrameIndex = 0;

        Ref<RenderContext> m_RenderContext;
        Ref<ImGuiLayer> m_ImGuiLayer;

        TUniquePtr<EditorLayer> m_Editor;
        TUniquePtr<Debugger> m_Debugger;
        TUniquePtr<PhysicsSystem> m_PhysicsSystem;
        TUniquePtr<SelectionContext> m_SelectionContext;

		std::mutex m_EventQueueMutex;
		std::queue<std::function<void()>> m_EventQueue;
		std::vector<EventCallbackFn> m_EventCallbacks;

        Ref<Project> m_Project;

        Ref<Scene> m_Scene = nullptr; // Active scene // RENAME

        inline static LApplication* m_Instance = nullptr;
    };

}