project "ImGui"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17" 

    staticruntime "On"
    configurations { "Debug", "Release", "Dist" }

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines {
        "IMGUI_DEFINE_MATH_OPERATORS",
		"_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING"
    }

	files {
        "imgui/imconfig.h",
		"imgui/imgui.h",
		"imgui/imgui.cpp",
		"imgui/imgui_draw.cpp",
		"imgui/imgui_internal.h",
		"imgui/imgui_widgets.cpp",
		"imgui/imstb_rectpack.h",
		"imgui/imstb_textedit.h",
		"imgui/imstb_truetype.h",
		"imgui/imgui_demo.cpp",
        "imgui/imgui_tables.h",
        "imgui/imgui_tables.cpp",

		"imgui/backends/imgui_impl_glfw.cpp",
		"imgui/backends/imgui_impl_glfw.h",
		"imgui/backends/imgui_impl_opengl3.cpp",
		"imgui/backends/imgui_impl_opengl3.h",
	}

    includedirs {
        "imgui",
        "imgui/backends",
        "%{Dependency.Glfw.IncludeDir}",
        "%{Dependency.Glad.IncludeDir}",
        "%{Dependency.StbImage.IncludeDir}",
        "%{Dependency.ImGuizmo.IncludeDir}",
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
