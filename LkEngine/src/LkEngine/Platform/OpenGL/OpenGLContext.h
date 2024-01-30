#pragma once

#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    class Window;
    class OpenGLSwapChain;

    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(Window* window);
        ~OpenGLContext();

        void Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc) override;
        void Destroy() override;
        void SetViewport(const glm::vec2& pos, const glm::vec2& size) override;
        void UpdateResolution(uint16_t width, uint16_t height) override;
        void SetDepthEnabled(bool enabled) override;
        GLFWwindow* GetGlfwWindow() override { return m_GlfwWindow; }
        void SetBlendingEnabled(bool enabled) override;
        void SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc) override;
        void SetSourceBlendFunction(const SourceBlendFunction& srcFunc) override;
        void SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc) override;

        void SetName(std::string_view name) override { m_Name = std::string(name); }
        const std::string GetName() const { return m_Name; }
        
        std::string GetCurrentSourceBlendFunctionName() const override;
        std::string GetCurrentDestinationBlendFunctionName() const override;

        bool GetBlendingEnabled() const override { return m_BlendingEnabled; } 

    private:
        std::string m_GlslVersion = "";
        bool m_DepthEnabled = true;
        bool m_BlendingEnabled = true;
        BlendFunction m_BlendFunction;

        std::string m_Name;

        GLFWwindow* m_GlfwWindow = nullptr;
        s_ptr<Window> m_Window = nullptr;

        Ref<OpenGLSwapChain> m_SwapChain = nullptr;

        friend class Editor;
    };

}