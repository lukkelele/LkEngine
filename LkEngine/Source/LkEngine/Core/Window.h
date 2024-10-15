#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"

#include "LkEngine/Core/String.h"
#include "LkEngine/Core/ApplicationConfig.h"

#include "LkEngine/Renderer/RenderContext.h"
#include "LkEngine/Renderer/SwapChain.h"
#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

	struct FWindowSpecification
	{
		//std::string Title = "LkEngine v2.0.1";
        /// @TODO: Get version
		LString Title = "LkEngine v2.0.1";
		uint32_t Width = 1600;
		uint32_t Height = 1080;
		bool Decorated = true;
		bool Fullscreen = false;
		bool VSync = true;
        bool StartMaximized = false;
        std::filesystem::path IconPath{};

        FWindowSpecification() = default;
        FWindowSpecification(const ApplicationSpecification& ApplicationSpec)
            : Title(ApplicationSpec.Title)
            , Width(ApplicationSpec.Width)
            , Height(ApplicationSpec.Height)
            , Fullscreen(ApplicationSpec.Fullscreen)
            , Decorated(true)
            , VSync(ApplicationSpec.VSync)
            , StartMaximized(ApplicationSpec.StartMaximized) 
        {
        }
	};

    class LWindow : public LObject
    {
    public:
        LWindow(const FWindowSpecification& WindowSpecification);
        ~LWindow();

        void Init();
        void SwapBuffers();
        void ProcessEvents();
        void Shutdown();

        FORCEINLINE GLFWwindow* GetGlfwWindow() const { return m_GlfwWindow; }
        FORCEINLINE uint32_t GetWidth() const { return m_Width; }
        FORCEINLINE uint32_t GetHeight() const { return m_Height; }
        FORCEINLINE uint32_t GetViewportWidth()  const { return m_ViewportWidth; }
        FORCEINLINE uint32_t GetViewportHeight() const { return m_ViewportHeight; }
        FORCEINLINE glm::vec2 GetPos() const { return m_Pos; }
        FORCEINLINE glm::vec2 GetSize() const { return glm::vec2(m_Width, m_Height); }
        FORCEINLINE glm::vec2 GetViewportSize() const { return { m_ViewportWidth, m_ViewportHeight }; }
        FORCEINLINE std::string GetTitle() const { return m_Title; }
        FORCEINLINE std::string GetShaderVersion() const { return m_GlslVersion; }
        FORCEINLINE bool IsVSyncEnabled() const { return m_VSync; }
        FORCEINLINE void SetViewportWidth(uint32_t width) { m_ViewportWidth = width; }
        FORCEINLINE void SetViewportHeight(uint32_t height) { m_ViewportHeight = height; }
        FORCEINLINE void SetDepthEnabled(bool enabled) { RenderContext->SetDepthEnabled(enabled); }
        FORCEINLINE TObjectPtr<LRenderContext> GetRenderContext() { return RenderContext; }

        void SetSize(const glm::vec2& size);
        void SetVSync(bool enabled);

        /// REMOVE
        TObjectPtr<LSwapChain> GetSwapChain();
        TObjectPtr<LRenderPass> GetRenderPass();

		FORCEINLINE void SetWidth(const uint32_t width) { m_Width = width; }
		FORCEINLINE void SetHeight(const uint32_t height) { m_Height = height; }

        /// UPDATE
        float GetScalerX() const;
        float GetScalerY() const;
        glm::vec2 GetScalers() const;
        void SetScalerX(float x);
        void SetScalerY(float y);
        void SetScalers(float x, float y);
        void SetScalers(const glm::vec2& scalers);

        /* TODO : Event category. */
        FORCEINLINE void SetEventCallback(const FEventCallback& Callback)
        {
            m_Data.EventCallback = Callback;
        }

        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

        FORCEINLINE static LWindow& Get()
        {
            return *Instance;
        }

    private:
        std::string m_Title = "";
        uint32_t m_Width = 0, m_Height = 0;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        glm::vec2 m_Pos = { 0.0f, 0.0f };
        glm::vec2 m_ViewportScalers = { 1.0f, 1.0f };

        std::string m_GlslVersion = "";
        bool m_VSync;

        FWindowSpecification Specification;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            FEventCallback EventCallback;
        };

        WindowData m_Data{};

        GLFWwindow* m_GlfwWindow = nullptr;

        TObjectPtr<LPipeline> m_Pipeline = nullptr;
        TObjectPtr<LSwapChain> m_SwapChain = nullptr;
        TObjectPtr<LRenderContext> RenderContext = nullptr;

	    inline static bool bGlfwInitialized = false;

        inline static LWindow* Instance = nullptr;

        friend class LEditorLayer;

    private:
        LCLASS(LWindow);
    };


}
