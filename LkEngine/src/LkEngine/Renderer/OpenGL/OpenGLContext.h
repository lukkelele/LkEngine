#pragma once

#include "LkEngine/Renderer/GraphicsContext.h"


namespace LkEngine {

    // Forward declaration
    class Window;

    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(Window& window, const std::string& glslVersion);
        ~OpenGLContext();

        void Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc);
        void Destroy();
        void BeginImGuiFrame();
        void EndImGuiFrame();
        void InitImGui(const std::string& glslVersion);
        void UpdateResolution(uint16_t width, uint16_t height);
        void SetDepthEnabled(bool enabled);
        GLFWwindow* GetGlfwWindow() { return m_GlfwWindow; }
        void SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc);
        void SetSourceBlendFunction(const SourceBlendFunction& srcFunc);
        void SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc);
        std::string GetSourceBlendFunctionName();
        std::string GetDestinationBlendFunctionName();
        std::string GetSourceBlendFunctionName(const SourceBlendFunction& srcFunc);
        std::string GetDestinationBlendFunctionName(const DestinationBlendFunction& dstFunc);

    private:
        std::string m_GlslVersion = "";
        bool m_DepthEnabled = true;
        BlendFunction m_BlendFunction;
        GLFWwindow* m_GlfwWindow = nullptr;
        s_ptr<Window> m_Window = nullptr;
    };

}