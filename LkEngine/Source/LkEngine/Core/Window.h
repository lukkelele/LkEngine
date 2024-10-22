#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/String.h"
#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Core/ApplicationConfig.h"

#include "LkEngine/Renderer/RenderContext.h"
#include "LkEngine/Renderer/SwapChain.h"
#include "LkEngine/Renderer/RenderPass.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

	struct FWindowSpecification
	{
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

	/**
	 * FWindowData
     * 
     *  Data container.
	 */
	struct FWindowData
	{
		std::string Title{};
		uint32_t Width = 0;
		uint32_t Height = 0;

		FEventCallback EventCallback;

        LKeyboard::FOnKeyPressed OnKeyPressed;

		LMouse::FOnMouseButtonPressed OnMouseButtonPressed;
		LMouse::FOnMouseButtonReleased OnMouseButtonReleased;
		LMouse::FOnMouseScrolled OnMouseScrolled;
	};

    /**
     * LWindow
     */
    class LWindow : public LObject
    {
		LK_DECLARE_MULTICAST_DELEGATE(FOnWindowSizeUpdated, const uint16_t, const uint16_t);
		LK_DECLARE_MULTICAST_DELEGATE(FOnWindowScalersUpdated, const float, const float);
		LK_DECLARE_MULTICAST_DELEGATE(FOnViewportSizeUpdated, const uint16_t, const uint16_t);
		LK_DECLARE_MULTICAST_DELEGATE(FOnViewportScalersUpdated, const float, const float);
    public:
        LWindow(const FWindowSpecification& WindowSpecification);
        ~LWindow();

        virtual void Initialize() override;

        void SwapBuffers();
        void ProcessEvents();
        void Shutdown();

        FORCEINLINE GLFWwindow* GetGlfwWindow() const { return GlfwWindow; }

        FORCEINLINE LVector2 GetSize() const { return Size; }
        FORCEINLINE uint32_t GetWidth() const { return Size.X; }
        FORCEINLINE uint32_t GetHeight() const { return Size.Y; }
        FORCEINLINE uint32_t GetViewportWidth()  const { return ViewportSize.X; }
        FORCEINLINE uint32_t GetViewportHeight() const { return ViewportSize.Y; }

        FORCEINLINE LVector2 GetPos() const { return m_Pos; }
        FORCEINLINE LVector2 GetViewportSize() const { return ViewportSize; }

        FORCEINLINE std::string GetTitle() const { return m_Title; }
        FORCEINLINE std::string GetShaderVersion() const { return m_GlslVersion; }
        FORCEINLINE bool IsVSyncEnabled() const { return m_VSync; }

        FORCEINLINE TObjectPtr<LRenderContext> GetRenderContext() 
        { 
            return RenderContext; 
        }

        FORCEINLINE void SetDepthEnabled(const bool InEnabled) 
        { 
            if (RenderContext)
            {
                RenderContext->SetDepthEnabled(InEnabled); 
            }
        }

        FORCEINLINE void SetSize(const LVector2& InSize)
        {
            if ((Size.X != InSize.X) || (Size.Y != InSize.X))
            {
                Size.X = static_cast<decltype(Size.X)>(InSize.X);
                Size.Y = static_cast<decltype(Size.Y)>(InSize.Y);
                LK_CORE_ERROR_TAG("Window", "Width={}  Height={}  Scalers={{{}, {}}}", 
                                  Size.X, Size.Y, m_ViewportScalers.X, m_ViewportScalers.Y);

                OnWindowSizeUpdated.Broadcast(static_cast<uint16_t>(Size.X), static_cast<uint16_t>(Size.Y));
            }
        }

        FORCEINLINE void SetViewportWidth(const uint16_t NewWidth) 
        { 
            if (ViewportSize.X != NewWidth)
            {
                ViewportSize.Y = NewWidth; 
                OnViewportSizeUpdated.Broadcast(static_cast<uint16_t>(ViewportSize.X), static_cast<uint16_t>(ViewportSize.Y));
            }
        }

        FORCEINLINE void SetViewportHeight(const uint16_t NewHeight) 
        { 
            if (ViewportSize.Y != NewHeight)
            {
                ViewportSize.Y = static_cast<decltype(ViewportSize.Y)>(NewHeight); 
                OnViewportSizeUpdated.Broadcast(static_cast<uint16_t>(ViewportSize.X), static_cast<uint16_t>(ViewportSize.Y));
            }
        }

        void SetVSync(const bool InEnabled);

        /// REMOVE
        TObjectPtr<LSwapChain> GetSwapChain();
        TObjectPtr<LRenderPass> GetRenderPass();

		FORCEINLINE void SetWidth(const uint16_t NewWidth) 
        { 
            if (Size.X != NewWidth)
            {
                Size.X = static_cast<decltype(Size.X)>(NewWidth); 
                OnWindowSizeUpdated.Broadcast(static_cast<uint16_t>(Size.X), static_cast<uint16_t>(Size.Y));
            }
        }

		FORCEINLINE void SetHeight(const uint16_t NewHeight) 
        { 
            if (Size.Y != NewHeight)
            {
                Size.Y = static_cast<decltype(Size.Y)>(NewHeight); 
                OnWindowSizeUpdated.Broadcast(static_cast<uint16_t>(Size.X), static_cast<uint16_t>(Size.Y));
            }
        }

        /// 
        /// UPDATE ALL THIS 
        /// 

		FORCEINLINE float GetScalerX() const { return m_ViewportScalers.X; }
		FORCEINLINE float GetScalerY() const { return m_ViewportScalers.Y; }
		FORCEINLINE LVector2 GetScalers() const { return m_ViewportScalers; }

		FORCEINLINE void SetScalerX(const const float InX)
		{
			m_ViewportScalers.X = InX;
		}

		FORCEINLINE void SetScalerY(const float InY)
		{
			m_ViewportScalers.Y = InY;
		}

		FORCEINLINE void SetScalers(const float InX, const float InY)
		{
			m_ViewportScalers.X = InX;
			m_ViewportScalers.Y = InY;
		}

		FORCEINLINE void SetScalers(const glm::vec2& InScalers)
		{
			m_ViewportScalers = InScalers;
		}

        /* TODO : Event category. */
        FORCEINLINE void SetEventCallback(const FEventCallback& Callback)
        {
            Data.EventCallback = Callback;
        }

        FORCEINLINE FWindowData& GetWindowData() { return Data; }
        FORCEINLINE const FWindowData& GetWindowData() const { return Data; }

        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

        FORCEINLINE static LWindow& Get() { return *Instance; }

    public:
        FOnWindowSizeUpdated OnWindowSizeUpdated{};
        FOnViewportSizeUpdated OnViewportSizeUpdated{};

        static constexpr uint16_t DEFAULT_WIDTH  = 1650;
        static constexpr uint16_t DEFAULT_HEIGHT = 1080;
    private:
        FWindowSpecification Specification{};
        std::string m_Title = "";

	    bool bGlfwInitialized = false;
        GLFWwindow* GlfwWindow = nullptr;

        LVector2 Size{};
        LVector2 ViewportSize{};

        LVector2 m_Pos = { 0.0f, 0.0f };
        LVector2 m_ViewportScalers = { 1.0f, 1.0f };

        std::string m_GlslVersion = "";
        bool m_VSync = false;

        FWindowData Data{};

        TObjectPtr<LPipeline> m_Pipeline = nullptr;
        TObjectPtr<LSwapChain> m_SwapChain = nullptr;
        TObjectPtr<LRenderContext> RenderContext = nullptr;

        inline static LWindow* Instance = nullptr;

        friend class LEditorLayer;
    private:
        LCLASS(LWindow);
    };


}
