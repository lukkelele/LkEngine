#pragma once

#include "LkEngine/Core/PlatformDetection.h"
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"
#include "LkEngine/Core/IO/File.h"

#include "LkEngine/Input/Input.h"

#include "LkEngine/ImGui/ImGuiLayer.h"

#include "LkEngine/Debug/Debugger.h"

#include "LkEngine/Asset/AssetRegistry.h"

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

#include "LkEngine/Editor/Editor.h"

#include "LkEngine/Physics/PhysicsSystem.h"


namespace LkEngine {

    class Application
    {
    public:
        Application(const ApplicationSpecification& props = ApplicationSpecification());
        ~Application();

        static Application* Get() { return m_Instance; }

        void Init();
        void Run();
        void Exit();
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        void PopOverlay(Layer* layer);
        
        void RenderImGui();
        void AddScene(Scene& scene);
        void SetScene(Ref<Scene> scene);

        Window& GetWindow() { return *m_Window; }
        GLFWwindow* GetGlfwWindow() { return m_Window->GetGlfwWindow(); }
        Ref<Renderer> GetRenderer() const { return m_Renderer; }
        Ref<GraphicsContext> GetGraphicsContext() { return m_GraphicsContext; }
        const ApplicationSpecification GetSpecification() const { return m_Specification;  }

        Timestep GetTimestep() const { return m_Timestep; }

		uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

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
				m_EventQueue.push([event](){ Application::Get()->OnEvent(*event); });
			}
		}

    private:
        ApplicationSpecification m_Specification;
        Timer m_Timer;
        Layer* m_MainLayer = nullptr; 
        LayerStack m_LayerStack;
        AssetRegistry m_AssetRegistry;

        uint32_t m_CurrentFrameIndex = 0;
        SelectionContext* m_SelectionContext = nullptr;

        Ref<Renderer> m_Renderer = nullptr;
        s_ptr<Input> m_Input = nullptr;
        std::unique_ptr<Window> m_Window = nullptr;

        Editor* m_Editor = nullptr;

        PhysicsSystem* m_PhysicsSystem = nullptr;
        Ref<GraphicsContext> m_GraphicsContext = nullptr;
        Ref<ImGuiLayer> m_ImGuiLayer = nullptr;

        Timestep m_LastTimestep, m_Timestep;

		std::mutex m_EventQueueMutex;
		std::queue<std::function<void()>> m_EventQueue;
		std::vector<EventCallbackFn> m_EventCallbacks;

        Debugger* m_Debugger = nullptr;

        Ref<Scene> m_Scene = nullptr; // Active scene
        Ref<Scene> m_Scenes[8];

        inline static Application* m_Instance = nullptr;
    };

}