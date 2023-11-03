include "lib/dependencies.lua"

workspace "LkEngine"
    architecture "x86_64"
    startproject "LkApplication"

    configurations {
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

include "LkApplication/premake5.lua"
include "lib/glfw/premake5.lua"
include "lib/glad/premake5.lua"
include "lib/imgui/premake5.lua"
include "lib/ImGuizmo/premake5.lua"
include "lib/box2d/premake5.lua"


project "LkEngine"
    location "LkEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "LKpch.h"
    pchsource "LkEngine/src/LKpch.cpp"

    defines {
        "_GLM_WIN32",
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
        "IMGUI_DEFINE_MATH_OPERATORS",

        "LK_RENDERER_API_OPENGL",
    }

    files { 
        "%{wks.location}/LkEngine/**.h",
        "%{wks.location}/LkEngine/**.cpp",
        "%{wks.location}/lib/stb_image/**.h",
        "%{wks.location}/lib/stb_image/**.cpp",
   	}

    libdirs {
        "%{wks.location}/lib/GLFW/lib",
    }

    includedirs {
        "%{prj.name}",
        "%{wks.location}/LkEngine/src",
        "%{wks.location}/LkEngine/src/LkEngine",
        "%{wks.location}/lib",
        "%{wks.location}/lib/glm",
        "%{wks.location}/lib/glad",
        "%{wks.location}/lib/glad/include",
        "%{wks.location}/lib/stb_image",
        "%{wks.location}/lib/GLFW/include",
        "%{wks.location}/lib/spdlog/include",
        "%{wks.location}/lib/stb_image",
        --"%{wks.location}/lib/imgui",
        "%{Dependencies.ImGui.IncludeDir}",
        "%{wks.location}/lib/imgui/backends",
        "%{wks.location}/lib/imgui/examples",
        "%{wks.location}/lib/ImGuizmo",
        "%{wks.location}/lib/entt/src",
        "%{wks.location}/lib/box2d/include",
    }

    links {
        "GLFW",
        "glad",
        "opengl32",
        "ImGui",
        "ImGuizmo",
        "box2d",
    }

	filter "system:windows"
		defines { 
            "LKPLATFORM_WINDOWS",
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