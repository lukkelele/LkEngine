project "ImGui"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17" 
	location "imgui"

    staticruntime "On"
    configurations { "Debug", "Release", "Dist" }

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines
    {
        "IMGUI_DEFINE_MATH_OPERATORS",
		"_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING"
    }

	files
    {
        "%{prj.location}/imconfig.h",
		"%{prj.location}/imgui.h",
		"%{prj.location}/imgui.cpp",
		"%{prj.location}/imgui_draw.cpp",
		"%{prj.location}/imgui_internal.h",
		"%{prj.location}/imgui_widgets.cpp",
		"%{prj.location}/imstb_rectpack.h",
		"%{prj.location}/imstb_textedit.h",
		"%{prj.location}/imstb_truetype.h",
		"%{prj.location}/imgui_demo.cpp",
        "%{prj.location}/imgui_tables.h",
        "%{prj.location}/imgui_tables.cpp",

		"%{prj.location}/backends/imgui_impl_glfw.cpp",
		"%{prj.location}/backends/imgui_impl_glfw.h",
		"%{prj.location}/backends/imgui_impl_opengl3.cpp",
		"%{prj.location}/backends/imgui_impl_opengl3.h",
	}

    includedirs
    {
        "%{prj.location}",
        "%{prj.location}/backends",
        "%{ExternalDirectory}/GLFW/include",
        "%{ExternalDirectory}/glad/include",
        "%{ExternalDirectory}/stb_image",
        "%{Dependencies.ImGuizmo.IncludeDir}",
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
