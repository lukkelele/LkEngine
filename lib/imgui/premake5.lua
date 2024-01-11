project "ImGui"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    staticruntime "On"

    configurations { "Debug", "Release", "Dist" }

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    defines { "IMGUI_DEFINE_MATH_OPERATORS" }

	files {
        "imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",
		"imgui_impl_glfw.cpp",
		"imgui_impl_glfw.h",
		"imgui_impl_opengl3.cpp",
		"imgui_impl_opengl3.h",
        "imgui_tables.h",
        "imgui_tables.cpp",
	}

    includedirs
    {
        "%{wks.location}/lib/GLFW/include",
        "%{wks.location}/lib/glad/include",
    }

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
