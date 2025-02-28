#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Input/Keyboard.h"
#include "LkEngine/Core/Input/Mouse.h"
#include "LkEngine/Core/ApplicationConfig.h"

#include "LkEngine/Renderer/RenderContext.h"
#include "LkEngine/Renderer/SwapChain.h"
#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

    class LViewport;

    /* @TODO: Don't use the application config here. */
	struct FWindowSpecification
	{
		std::string Title = "LkEngine";
		uint32_t Width = 1600;
		uint32_t Height = 1080;
		bool bDecorated = true;
		bool bFullscreen = false;
		bool bVSync = true;
        bool bStartMaximized = false;
        std::filesystem::path IconPath{};

        FWindowSpecification() = default;
        FWindowSpecification(const FApplicationSpecification& ApplicationSpec)
            : Title(ApplicationSpec.Title)
            , Width(ApplicationSpec.Width)
            , Height(ApplicationSpec.Height)
            , bFullscreen(ApplicationSpec.Fullscreen)
            , bDecorated(true)
            , bVSync(ApplicationSpec.VSync)
            , bStartMaximized(ApplicationSpec.StartMaximized) 
        {
        }
	};

	/** Window maximized callback. */
	LK_DECLARE_MULTICAST_DELEGATE(FOnWindowMaximized, const bool);

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

		bool bVSync = true;
		bool bMaximized = false;
		bool bFullscreen = false;

		FOnWindowMaximized OnWindowMaximized;
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
		LWindow() = delete;
		~LWindow();

        virtual void Initialize() override;
        virtual void Destroy() override;

        void SwapBuffers();
        void ProcessEvents();

        FORCEINLINE GLFWwindow* GetGlfwWindow() const { return GlfwWindow; }

		void Maximize();
		bool IsMaximized() const;
		void Minimize();
		bool IsMinimized() const;
		bool IsFullscreen() const;
		void SetFullscreen(const bool Fullscreen);
		void SetTitle(const std::string& NewTitle);

        FORCEINLINE std::string GetTitle() const { return Title; }
        FORCEINLINE LVector2 GetSize() const { return Size; }
        FORCEINLINE uint16_t GetWidth() const { return static_cast<uint16_t>(Size.X); }
        FORCEINLINE uint16_t GetHeight() const { return static_cast<uint16_t>(Size.Y); }
        FORCEINLINE uint16_t GetViewportWidth()  const { return static_cast<uint16_t>(ViewportSize.X); }
        FORCEINLINE uint16_t GetViewportHeight() const { return static_cast<uint16_t>(ViewportSize.Y); }

        FORCEINLINE LVector2 GetPosition() const { return Pos; }
        FORCEINLINE LVector2 GetViewportSize() const { return ViewportSize; }

        FORCEINLINE bool IsVSyncEnabled() const { return Data.bVSync; }

        FORCEINLINE TObjectPtr<LRenderContext> GetRenderContext() 
        { 
            LK_CORE_ASSERT(RenderContext);
            return RenderContext; 
        }

        void SetDepthEnabled(const bool InEnabled) 
        { 
            if (RenderContext)
            {
                RenderContext->SetDepthEnabled(InEnabled); 
            }
        }

        void SetBlendingEnabled(const bool InEnabled) 
        { 
            if (RenderContext)
            {
                RenderContext->SetBlendingEnabled(InEnabled); 
            }
        }

        FORCEINLINE void SetSize(const LVector2& InSize)
        {
            if ((Size.X != InSize.X) || (Size.Y != InSize.X))
            {
                Size.X = static_cast<uint16_t>(InSize.X);
                Size.Y = static_cast<uint16_t>(InSize.Y);

                OnWindowSizeUpdated.Broadcast((uint16_t)Size.X, (uint16_t)Size.Y);
            }
        }

        FORCEINLINE void SetViewportWidth(const uint16_t NewWidth) 
        { 
            if (ViewportSize.X != NewWidth)
            {
                ViewportSize.X = static_cast<float>(NewWidth);
                OnViewportSizeUpdated.Broadcast(static_cast<uint16_t>(ViewportSize.X), static_cast<uint16_t>(ViewportSize.Y));
            }
        }

        FORCEINLINE void SetViewportHeight(const uint16_t NewHeight) 
        { 
            if (ViewportSize.Y != NewHeight)
            {
                ViewportSize.Y = static_cast<float>(NewHeight); 
                OnViewportSizeUpdated.Broadcast(static_cast<uint16_t>(ViewportSize.X), static_cast<uint16_t>(ViewportSize.Y));
            }
        }

        void SetVSync(const bool InEnabled);

		FORCEINLINE LSwapChain& GetSwapChain() 
        { 
            LK_CORE_ASSERT(m_SwapChain);
            return *m_SwapChain; 
        }

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
		FORCEINLINE float GetScalerX() const { return ViewportScalers.X; }
		FORCEINLINE float GetScalerY() const { return ViewportScalers.Y; }
		FORCEINLINE LVector2 GetScalers() const { return ViewportScalers; }

		FORCEINLINE void SetScalerX(const float InX) { ViewportScalers.X = InX; }
		FORCEINLINE void SetScalerY(const float InY) { ViewportScalers.Y = InY; }
		FORCEINLINE void SetScalers(const float InX, const float InY)
		{
			ViewportScalers.X = InX;
			ViewportScalers.Y = InY;
		}

        FORCEINLINE FWindowData& GetData() { return Data; }
        FORCEINLINE const FWindowData& GetData() const { return Data; }

        std::string GetShaderVersion() const { return GlslVersion; } /* REMOVE */
        FORCEINLINE const FWindowSpecification& GetSpecification() const { return Specification; }

        FORCEINLINE static LWindow& Get() { return *Instance; }

    public:
        FOnWindowSizeUpdated OnWindowSizeUpdated{};
        FOnViewportSizeUpdated OnViewportSizeUpdated{};

        static constexpr uint16_t DEFAULT_WIDTH  = 1600;
        static constexpr uint16_t DEFAULT_HEIGHT = 1080;
    private:
        FWindowData Data{};
        FWindowSpecification Specification{}; /* TODO: Replace entirely with FWindowData? */
		std::string Title{};

	    bool bGlfwInitialized = false;
		GLFWwindow* GlfwWindow{};

        LVector2 Size{};
        LVector2 ViewportSize{};

        LVector2 Pos = { 0.0f, 0.0f };
        LVector2 ViewportScalers = { 1.0f, 1.0f };

		std::string GlslVersion{}; /// REMOVE

        TObjectPtr<LPipeline> m_Pipeline = nullptr;
        TObjectPtr<LSwapChain> m_SwapChain = nullptr;
        TObjectPtr<LRenderContext> RenderContext = nullptr;

        inline static LWindow* Instance = nullptr;

        friend class LEditorLayer;
    private:
        LCLASS(LWindow);
    };


}
