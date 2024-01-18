project "imgui-node-editor"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

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
		"${wks.location}/external/imgui",
		"../imgui"
    }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

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
