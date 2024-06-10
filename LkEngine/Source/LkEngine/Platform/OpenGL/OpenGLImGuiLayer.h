#pragma once

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

    class OpenGLImGuiLayer : public ImGuiLayer
    {
    public:
        OpenGLImGuiLayer();
        ~OpenGLImGuiLayer();

        void Init();
        void Shutdown();

        void OnAttach() {}
        void OnDetach() {}

        void BeginFrame();
        void EndFrame();

    private:
        bool m_Initialized = false;
    };

}