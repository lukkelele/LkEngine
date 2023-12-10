#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>

#include "LkEngine/UI/Property.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/UI/DockSpace.h"


namespace LkEngine {

#if 0
    class UILayer : public Layer
    {
    public:
        UILayer() = default;
        ~UILayer() = default;

        static void Init();

    public:
        static float PositionStepSize;
        inline static bool Initialized = false;
        inline static bool ShowImGuiDemoWindow = false;
        inline static ImGuiWindowClass* UILayerWindowClass = nullptr;
        inline static ImGuiWindowClass* RendererWindowClass = nullptr;
        inline static ImGuiWindowClass* ExternalWindowClass = nullptr;
        static std::string SelectedEntityLabel;
        static ImVec2 SceneContentMenuSize, SelectedEntityMenuSize;
    };
#endif

}