#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Base.h"
#include "ApplicationConfig.h"

#include "LkEngine/Renderer/RenderContext.h"
#include "LkEngine/Renderer/SwapChain.h"
#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

	struct WindowSpecification
	{
		std::string Title = "LkEngine";
		uint32_t Width = 1600;
		uint32_t Height = 1080;
		bool Decorated = true;
		bool Fullscreen = false;
		bool VSync = true;
        bool StartMaximized = false;
		std::filesystem::path IconPath;

        WindowSpecification() = default;
        WindowSpecification(const ApplicationSpecification& appSpecification)
            : Title(appSpecification.Title)
            , Width(appSpecification.Width)
            , Height(appSpecification.Height)
            , Fullscreen(appSpecification.Fullscreen)
            , Decorated(true)
            , VSync(appSpecification.VSync)
            , StartMaximized(appSpecification.StartMaximized) {}
	};

    class Window 
    {
    public:
        Window(const WindowSpecification& specification);
        ~Window();

        void Init();
        void SwapBuffers();
        void ProcessEvents();
        void Shutdown();

        GLFWwindow* GetGlfwWindow() const { return m_GlfwWindow; }
        inline uint32_t GetWidth() const { return m_Width; }
        inline uint32_t GetHeight() const { return m_Height; }
        inline uint32_t GetViewportWidth()  const { return m_ViewportWidth; }
        inline uint32_t GetViewportHeight() const { return m_ViewportHeight; }
	    inline glm::vec2 GetPos() const { return m_Pos; }
	    inline glm::vec2 GetSize() const { return glm::vec2(m_Width, m_Height); }
		inline glm::vec2 GetViewportSize() const { return { m_ViewportWidth, m_ViewportHeight }; }
        inline std::string GetTitle() const { return m_Title; }
        inline std::string GetShaderVersion() const { return m_GlslVersion;  }

        void SetSize(const glm::vec2& size);
        void SetVSync(bool enabled);
        inline bool IsVSyncEnabled() const { return m_VSync; }

        Ref<SwapChain> GetSwapChain();
        Ref<RenderPass> GetRenderPass();
        inline Ref<RenderContext> GetRenderContext() { return m_RenderContext; }

        void SetWidth(uint32_t width); // { m_Width = width; }
        void SetHeight(uint32_t height); // { m_Height = height; }
        inline void SetViewportWidth(uint32_t width) { m_ViewportWidth = width; }
        inline void SetViewportHeight(uint32_t height) { m_ViewportHeight = height; }
        inline void SetDepthEnabled(bool enabled) { m_RenderContext->SetDepthEnabled(enabled); }

        float GetScalerX() const;
        float GetScalerY() const;
        glm::vec2 GetScalers() const;
        void SetScalerX(float x);
        void SetScalerY(float y);
        void SetScalers(float x, float y);
        void SetScalers(const glm::vec2& scalers);

        // Callbacks
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

        static Window& Get() { return *m_Instance; }

    private:
        std::string m_Title = "";
        uint32_t m_Width = 0, m_Height = 0;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        glm::vec2 m_Pos = { 0.0f, 0.0f };
        glm::vec2 m_ViewportScalers = { 1.0f, 1.0f };

        std::string m_GlslVersion = "";
        bool m_VSync;

        WindowSpecification m_Specification;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            EventCallbackFn EventCallback;
        } m_Data;

        GLFWwindow* m_GlfwWindow = nullptr;

        Ref<Pipeline> m_Pipeline = nullptr;
        Ref<SwapChain> m_SwapChain = nullptr;
        Ref<RenderContext> m_RenderContext = nullptr;

        inline static Window* m_Instance = nullptr;

        friend class EditorLayer;
    };


}
