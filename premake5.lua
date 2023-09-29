workspace "LkEngine"
    architecture "x86_64"

    configurations
    {
        "Debug",
		"Release",
        "Dist"
    }

    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
        defines { "LK_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "LK_RELEASE" }
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines { "LK_DIST" }
        runtime "Release"
        optimize "On"


outputdir = "%{cfg.buildcfg}-%{cfg.system}"

include "lib/GLFW/glfw.lua"
include "lib/glad/glad.lua"
include "lib/imgui/imgui.lua"

project "LkEngine"
    location "src"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    defines
    {
		"_GLM_WIN32",
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
        "IMGUI_DEFINE_MATH_OPERATORS",
    }

    files
    { 
        "%{wks.location}/LkEngine/**.h",
        "%{wks.location}/LkEngine/**.cpp",
        "%{wks.location}/lib/stb_image/**.h",
        "%{wks.location}/lib/stb_image/**.cpp",
   	}

    libdirs
    {
        "%{wks.location}/lib/GLFW/lib",
    }

    includedirs
    {
        "src",
        "%{prj.name}",
		"%{wks.location}/LkEngine/src",
		"%{wks.location}/LkEngine/src/LkEngine",
		"%{wks.location}/lib",
		"%{wks.location}/lib/glm",
		"%{wks.location}/lib/imgui",
		"%{wks.location}/lib/imgui/backends",
		"%{wks.location}/lib/imgui/examples",
		"%{wks.location}/lib/glad",
		"%{wks.location}/lib/glad/include",
		"%{wks.location}/lib/stb_image",
		"%{wks.location}/lib/GLFW/include",
		"%{wks.location}/lib/spdlog/include",
		"%{wks.location}/lib/stb_image",
    }

    links
    {
        "GLFW",
        "glad",
        "opengl32",
        "ImGui",
    }

	filter "system:windows"
		defines 
        { 
			"PLATFORM_WINDOWS",
			"_IMGUI_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}

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