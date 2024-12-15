#pragma once

#include "LkEngine/Core/CoreMacros.h"

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

	/** 
     * EGizmo 
     */
	enum class EGizmo
	{ 
		Translate = ImGuizmo::TRANSLATE,
		Rotate	  = ImGuizmo::ROTATE,
		Scale	  = ImGuizmo::SCALE
	};

	extern bool GDebug_DisplayWindowSizeOnHover;

}
