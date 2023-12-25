#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/ApplicationConfig.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/IO/File.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Renderer/RenderCommandQueue.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Physics2D/Physics2D.h"
#include "LkEngine/UI/UILayer.h"

//
// TODO:
// - UV Mapping, rotate textures
// - Entity selection, make it so dragging entities over eachother wont set new selection
//

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
        s_ptr<TextureLibrary> GetTextureLibrary() { return m_TextureLibrary; }
        void RenderImGui();
        void AddScene(Scene& scene);
        s_ptr<Scene> GetScene(uint8_t idx) { return m_Scenes[idx]; }

    private:
        ApplicationSpecification m_Specification;
        LayerStack m_LayerStack;
        Timer m_Timer;
        s_ptr<Window> m_Window = nullptr;
        s_ptr<Renderer> m_Renderer = nullptr;
        s_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        s_ptr<TextureLibrary> m_TextureLibrary = nullptr;
        s_ptr<Scene> m_ActiveScene = nullptr;
        s_ptr<EditorLayer> m_EditorLayer = nullptr;
        s_ptr<Input> m_Input = nullptr;
        s_ptr<Scene> m_Scenes[8];
        inline static Application* m_Instance = nullptr;
    };

}