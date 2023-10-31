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

        void Init();
        void Destroy();
        void BeginImGuiFrame();
        void EndImGuiFrame();
        void InitImGui(const std::string& glslVersion);
        //void SetDarkTheme();

    };

}