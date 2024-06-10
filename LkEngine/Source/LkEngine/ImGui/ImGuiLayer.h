#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Layer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/ImFileDialog.h>
#include <ImGuizmo/ImGuizmo.h>

#include "ImGuiUtilities.h"


namespace LkEngine {

    class LWindow;

    class ImGuiLayer : public RefCounted, public Layer
    {
    public:
        virtual ~ImGuiLayer() = default;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetDarkTheme();

        static Ref<ImGuiLayer> Create();
    };

	enum class GizmoMode : unsigned int 
	{ 
		Translate = (ImGuizmo::OPERATION::TRANSLATE),
		Rotate    = (ImGuizmo::OPERATION::ROTATE),
		Scale     = (ImGuizmo::OPERATION::SCALE)
	};

}