project "ImGuiNodeEditor"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines { 
		"IMGUI_DEFINE_MATH_OPERATORS" 
	}

	files
	{
		"imgui-node-editor/imgui_node_editor.h",
		"imgui-node-editor/imgui_node_editor.cpp",
		"imgui-node-editor/imgui_canvas.h",
		"imgui-node-editor/imgui_canvas.cpp",
		"imgui-node-editor/imgui_extra_math.h",
		"imgui-node-editor/imgui_node_editor_internal.h",
		"imgui-node-editor/imgui_node_editor_api.cpp",
		"imgui-node-editor/imgui_bezier_math.h",

		"imgui-node-editor/crude_json.h",
		"imgui-node-editor/crude_json.cpp",
	}

    includedirs {
		"%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- To allow include 'imgui.h' as well as 'imgui/imgui.h'.
    }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		pic "On"

    filter "configurations:Debug or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AddressSanitize"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
