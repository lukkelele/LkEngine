project "imgui_test_engine"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{ 
		"imgui_te_ui.h",
		"imgui_te_ui.cpp",
		"imgui_capture_tool.h",
		"imgui_capture_tool.cpp",
		"imgui_te_context.h",
		"imgui_te_context.cpp",
		"imgui_te_coroutine.h",
		"imgui_te_coroutine.cpp",
		"imgui_te_engine.h",
		"imgui_te_engine.cpp",
		"imgui_te_exporters.h",
		"imgui_te_exporters.cpp",
		"imgui_te_imconfig.h",
		"imgui_te_internal.h",
		"imgui_te_perftool.h",
		"imgui_te_perftool.cpp",
		"imgui_te_ui.h",
		"imgui_te_ui.cpp",
		"imgui_te_utils.h",
		"imgui_te_utils.cpp",
		"*.h", 
		"*.cpp",
		"thirdparty/**.h",
		"thirdparty/**.cpp",
	}

	includedirs
	{
		"lib/imgui",
		"../imgui",
		"${wks.location}/lib/imgui",
	}

	defines
	{
		"IMGUI_DEFINE_MATH_OPERATORS",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
