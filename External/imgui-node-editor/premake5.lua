project "imgui-node-editor"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	location "imgui-node-editor"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines 
	{ 
		"IMGUI_DEFINE_MATH_OPERATORS" 
	}

	files
	{
		"%{prj.name}/imgui_node_editor.h",
		"%{prj.name}/imgui_node_editor.cpp",
		"%{prj.name}/imgui_canvas.h",
		"%{prj.name}/imgui_canvas.cpp",
		"%{prj.name}/imgui_extra_math.h",
		"%{prj.name}/imgui_node_editor_internal.h",
		"%{prj.name}/imgui_node_editor_api.cpp",
		"%{prj.name}/imgui_bezier_math.h",
		"%{prj.name}/crude_json.h",
		"%{prj.name}/crude_json.cpp",
	}

    includedirs
    {
		"../imgui"
    }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		pic "On"

	filter "configurations:Debug"
		defines "LK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LK_DIST"
		runtime "Release"
		optimize "on"
