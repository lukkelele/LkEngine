#pragma once

#include "LkEngine/Renderer/RenderContext.h"


namespace LkEngine {

    class LWindow;
    class OpenGLSwapChain;

    class LOpenGLContext : public LRenderContext
    {
    public:
        LOpenGLContext(LWindow* InWindow);
        ~LOpenGLContext() = default;

        virtual void Init(const ESourceBlendFunction& InSourceBlendFunction, 
                          const EDestinationBlendFunction& InDestinationBlendFunction) override;
        virtual void Destroy() override;

        FORCEINLINE virtual GLFWwindow* GetGlfwWindow() override 
        { 
            return m_GlfwWindow; 
        }

        virtual void SetViewport(const glm::vec2& pos, const glm::vec2& size) override;
        virtual void UpdateResolution(const uint16_t width, const uint16_t height) override;

        virtual void SetDepthEnabled(const bool InEnabled) override;
        virtual void SetDepthFunction(const EDepthFunction& depthFunc) override;

        virtual void SetBlendingEnabled(bool enabled) override;
        virtual void SetBlendFunction(const ESourceBlendFunction& InSourceBlendFunction, 
                                      const EDestinationBlendFunction& InDestinationBlendFunction) override;
        virtual void SetSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction) override;
        virtual void SetDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction) override;

        FORCEINLINE virtual void SetName(std::string_view InName) override 
        { 
            m_Name = InName; 
        }

        FORCEINLINE const std::string GetName() const { return m_Name; }
        
        virtual std::string GetCurrentSourceBlendFunctionName() const override;
        virtual std::string GetCurrentDestinationBlendFunctionName() const override;

        FORCEINLINE virtual bool GetBlendingEnabled() const override 
        { 
            return bBlendingEnabled; 
        } 

    private:
        std::string m_GlslVersion = "";

        bool bDepthEnabled = true;
        bool bBlendingEnabled = true;

        FBlendFunction BlendFunction{};
        EDepthFunction DepthFunction;

        std::string m_Name;

        LWindow* Window;
        GLFWwindow* m_GlfwWindow;

        TObjectPtr<OpenGLSwapChain> m_SwapChain;

        friend class Editor;

        LCLASS(LOpenGLContext);
    };

}