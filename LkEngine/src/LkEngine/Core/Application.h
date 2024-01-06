#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/IO/File.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Debug/Debugger.h"
#include "LkEngine/Asset/AssetRegistry.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"
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
        
        GLFWwindow* GetGlfwWindow() { return m_Window->GetGlfwWindow(); }
        s_ptr<Window> GetWindow() const { return m_Window; }
        s_ptr<Renderer> GetRenderer() const { return m_Renderer; }
        s_ptr<GraphicsContext> GetGraphicsContext() { return m_GraphicsContext; }
        void RenderImGui();
        void AddScene(Scene& scene);
        s_ptr<Scene> GetScene(uint8_t idx) { return m_Scenes[idx]; }

    private:
        ApplicationSpecification m_Specification;
        Timer m_Timer;
        LayerStack m_LayerStack;
        AssetRegistry m_AssetRegistry;

        s_ptr<Window> m_Window = nullptr;
        s_ptr<Renderer> m_Renderer = nullptr;
        s_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        s_ptr<Input> m_Input = nullptr;
        s_ptr<EditorLayer> m_EditorLayer = nullptr;
        PhysicsSystem* m_PhysicsSystem = nullptr;

        s_ptr<Scene> m_Scene = nullptr; // Active scene
        s_ptr<Scene> m_Scenes[8];

        Debugger* m_Debugger = nullptr;

        inline static Application* m_Instance = nullptr;
    };

}