#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Layer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/ImFileDialog.h>
#include <ImGuizmo/ImGuizmo.h>

#include "ImGuiUtilities.h"


namespace LkEngine {

    class LWindow;

    class LImGuiLayer : public LLayer
    {
    public:
        virtual ~LImGuiLayer() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetDarkTheme();

        static TObjectPtr<LImGuiLayer> Create();

    private:
        LCLASS(LImGuiLayer);
    };

	enum class GizmoMode : unsigned int 
	{ 
		Translate = (ImGuizmo::OPERATION::TRANSLATE),
		Rotate    = (ImGuizmo::OPERATION::ROTATE),
		Scale     = (ImGuizmo::OPERATION::SCALE)
	};

}