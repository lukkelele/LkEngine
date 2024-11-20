project "imgui-node-editor"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	location "%{prj.name}"

	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines 
	{ 
		"IMGUI_DEFINE_MATH_OPERATORS" 
	}

	files
	{
		"%{prj.location}/imgui_node_editor.h",
		"%{prj.location}/imgui_node_editor.cpp",
		"%{prj.location}/imgui_canvas.h",
		"%{prj.location}/imgui_canvas.cpp",
		"%{prj.location}/imgui_extra_math.h",
		"%{prj.location}/imgui_node_editor_internal.h",
		"%{prj.location}/imgui_node_editor_api.cpp",
		"%{prj.location}/imgui_bezier_math.h",

		"%{prj.location}/crude_json.h",
		"%{prj.location}/crude_json.cpp",
	}

    includedirs
    {
		"imgui"
    }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		pic "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
