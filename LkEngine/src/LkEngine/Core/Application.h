#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/LayerStack.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/IO/File.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Physics/2D/Physics2D.h"
#include "LkEngine/UI/UILayer.h"

/*
    TODO:
    - UV Mapping, rotate textures
*/

namespace LkEngine {

    struct ApplicationProperties
    {
		std::string Title;
        uint32_t Width, Height;
		std::string Directory;
		std::string GlslVersion;
		ApplicationProperties(const std::string& title = "LkEngine",
						      uint16_t width = 1920,
						      uint16_t height = 1080,
                              const std::string& directory = "",
                              const std::string& glslVersion = "#version 450")
			: Title(title), Width(width), Height(height), Directory(directory), GlslVersion(glslVersion) {}
    };

    class Application
    {
    public:
        Application(const ApplicationProperties& props = ApplicationProperties());
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
        
        uint16_t GetWidth() const { return m_Window->GetWidth(); }
        uint16_t GetHeight() const { return m_Window->GetHeight(); }
        s_ptr<Window> GetWindow() const { return m_Window; }
        s_ptr<Renderer> GetRenderer() const { return m_Renderer; }
        GLFWwindow* GetGlfwWindow() { return m_Window->GetGlfwWindow(); }
        s_ptr<GraphicsContext> GetGraphicsContext() { return m_Context; }
        s_ptr<TextureLibrary> GetTextureLibrary() { return m_TextureLibrary; }

        bool IsKeyboardEnabled();
        bool IsMouseEnabled();

    private:
        ApplicationProperties m_Props;
        LayerStack m_LayerStack;
        Timer m_Timer;
        s_ptr<Window> m_Window = nullptr;
        s_ptr<Renderer> m_Renderer = nullptr;
        s_ptr<GraphicsContext> m_Context = nullptr;
        s_ptr<TextureLibrary> m_TextureLibrary = nullptr;
        s_ptr<Scene> m_Scene = nullptr;
        s_ptr<EditorLayer> m_EditorLayer = nullptr;
        s_ptr<Input> m_Input = nullptr;
        inline static Application* m_Instance = nullptr;
    };

}