project "ImGui"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17" 
    configurations { "Debug", "Release", "Dist" }
    staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines
    {
        "IMGUI_DEFINE_MATH_OPERATORS"
    }

	files
    {
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
        "imgui_tables.h",
        "imgui_tables.cpp",

		"imgui_impl_glfw.cpp",
		"imgui_impl_glfw.h",
		"imgui_impl_opengl3.cpp",
		"imgui_impl_opengl3.h",

		"ImFileDialog.h",
		"ImFileDialog.cpp",
	}

    includedirs
    {
        "%{ExternalDirectory}/GLFW/include",
        "%{ExternalDirectory}/glad/include",
        "%{ExternalDirectory}/stb_image",
    }

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
