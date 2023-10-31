#pragma once

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

    class OpenGLRenderer : public RendererAPI
    {
    public:
        void Init();
        void Shutdown();

        void BeginFrame();
        void EndFrame();

    };

}