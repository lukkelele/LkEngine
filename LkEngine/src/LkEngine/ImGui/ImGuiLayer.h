#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Layer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <ImGuizmo/ImGuizmo.h>

#include "ImGuiUtilities.h"


namespace LkEngine {

    // Forward declaration
    class Window;

    class ImGuiLayer : public RefCounted, public Layer
    {
    public:
        virtual ~ImGuiLayer() = default;

        //static s_ptr<ImGuiLayer> Create();
        static Ref<ImGuiLayer> Create();

        virtual void Init(Ref<Window>& window) = 0;
        virtual void Shutdown() = 0;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetDarkTheme();
    };

}