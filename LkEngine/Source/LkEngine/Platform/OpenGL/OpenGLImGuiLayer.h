#pragma once

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

    class LOpenGLImGuiLayer : public LImGuiLayer
    {
    public:
        LOpenGLImGuiLayer() = default;
        ~LOpenGLImGuiLayer() = default;

        virtual void Initialize() override;
        virtual void Shutdown() override;

        /**
         * @brief TODO
         */
        virtual void OnUpdate(const float DeltaTime) {}

        virtual void OnAttach() override {}
        virtual void OnDetach() override {}

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

    private:
        bool bInitialized = false;

        LCLASS(LOpenGLImGuiLayer);
    };

}