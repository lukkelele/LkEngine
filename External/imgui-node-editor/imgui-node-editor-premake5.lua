project "imgui-node-editor"
	kind "StaticLib"
	language "C++"
   	cppdialect "C++17"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines { "IMGUI_DEFINE_MATH_OPERATORS" }

	files
	{
		"imgui_node_editor.h",
		"imgui_node_editor.cpp",
		"imgui_canvas.h",
		"imgui_canvas.cpp",
		"imgui_extra_math.h",
		"imgui_node_editor_internal.h",
		"imgui_node_editor_api.cpp",
		"imgui_bezier_math.h",

		"crude_json.h",
		"crude_json.cpp",
	}

    	includedirs
    	{
		"%{ExternalDirectory}/imgui",
    	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
