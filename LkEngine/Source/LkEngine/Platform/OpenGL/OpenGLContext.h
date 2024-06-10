#pragma once

#include "LkEngine/Renderer/RenderContext.h"


namespace LkEngine {

    class LWindow;
    class OpenGLSwapChain;

    class OpenGLContext : public RenderContext
    {
    public:
        OpenGLContext(LWindow* InWindowRef);
        ~OpenGLContext();

        void Init(const ESourceBlendFunction& InSourceBlendFunction, 
                  const EDestinationBlendFunction& InDestinationBlendFunction) override;
        void Destroy() override;

        GLFWwindow* GetGlfwWindow() override { return m_GlfwWindow; }

        void SetViewport(const glm::vec2& pos, const glm::vec2& size) override;
        void UpdateResolution(uint16_t width, uint16_t height) override;

        void SetDepthEnabled(bool enabled) override;
        void SetDepthFunction(const EDepthFunction& depthFunc) override;
        void SetBlendingEnabled(bool enabled) override;
        void SetBlendFunction(const ESourceBlendFunction& InSourceBlendFunction, 
                              const EDestinationBlendFunction& InDestinationBlendFunction) override;
        void SetSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction) override;
        void SetDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction) override;

        FORCEINLINE void SetName(std::string_view InName) override 
        { 
            //m_Name = std::string(name); 
            //m_Name = name.data();
            m_Name = InName;
        }

        const std::string GetName() const { return m_Name; }
        
        std::string GetCurrentSourceBlendFunctionName() const override;
        std::string GetCurrentDestinationBlendFunctionName() const override;

        bool GetBlendingEnabled() const override { return bBlendingEnabled; } 

    private:
        std::string m_GlslVersion = "";

        bool bDepthEnabled = true;
        bool bBlendingEnabled = true;

        FBlendFunction BlendFunction{};
        EDepthFunction DepthFunction;

        std::string m_Name;

        LWindow* m_Window;
        GLFWwindow* m_GlfwWindow;

        Ref<OpenGLSwapChain> m_SwapChain;

        friend class Editor;
    };

}