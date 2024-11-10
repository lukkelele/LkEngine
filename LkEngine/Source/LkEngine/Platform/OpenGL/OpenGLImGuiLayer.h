#pragma once

#include "LkEngine/UI/UILayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ImFileDialog/ImFileDialog.h>


namespace LkEngine {

    class LOpenGLImGuiLayer : public LUILayer
    {
    public:
        LOpenGLImGuiLayer();
        ~LOpenGLImGuiLayer() = default;

        virtual void Initialize() override;
        virtual void Shutdown() override;

        /**
         * @brief TODO
         */
        virtual void OnUpdate(const float DeltaTime) {}
        virtual void OnRenderUI() override {};

        virtual void OnAttach() override {}
        virtual void OnDetach() override {}

        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        
        virtual void SetDarkTheme() override;

    private:
        bool bInitialized = false;

        LCLASS(LOpenGLImGuiLayer);
    };

}