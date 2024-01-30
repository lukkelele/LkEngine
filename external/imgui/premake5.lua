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
        "imgui_tables.h",
        "imgui_tables.cpp",

		"imgui_impl_opengl3.cpp",
		"imgui_impl_opengl3.h",

		"imgui_impl_vulkan.h",
		"imgui_impl_vulkan.cpp",
	}

    includedirs {
        "%{wks.location}/external/GLFW/include",
        "%{wks.location}/external/glad/include",
        "%{Dependencies.Vulkan.Windows.IncludeDir}",
    }

    libdirs {
        "%{Dependencies.Vulkan.Windows.LibDir}",
    }

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
        includedirs {
            "%{Dependencies.Vulkan.Windows.IncludeDir}"
        }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
