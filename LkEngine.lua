workspace "LkEngine"
    architecture "x86_64"
    startproject "LkApplication"

    configurations { "Debug", "Release", "Dist" }

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


ExternalDirectory = "%{wks.location}/External"
AssetsDirectory = "%{wks.location}/Assets"
BuildOutputDirectory = "%{cfg.buildcfg}-%{cfg.system}"
TargetDirectory = "%{wks.location}/Binaries/" .. BuildOutputDirectory .. "/%{prj.name}"
IntermediateDirectory = "%{wks.location}/Intermediate/" .. BuildOutputDirectory .. "/%{prj.name}"
include "External/Dependencies.lua"


project "LkEngine"
    location "LkEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    pchheader "LKpch.h"
    pchsource "LkEngine/Source/LKpch.cpp"

    defines 
    {
        "LK_RENDERER_API_OPENGL",
        "LK_PHYSICS_API_BULLET3",
        "LK_PHYSICS_API_BOX2D",

        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS",
		"YAML_CPP_STATIC_DEFINE",
        "IMGUI_DEFINE_MATH_OPERATORS",
    }

    files
    { 
        "%{wks.location}/LkEngine/**.h",
        "%{wks.location}/LkEngine/**.cpp",

        "%{wks.location}/LkEngine/Source/**.h",
        "%{wks.location}/LkEngine/Source/**.cpp",

        "%{ExternalDirectory}/stb_image/**.h",
        "%{ExternalDirectory}/stb_image/**.cpp",
   	}

    libdirs 
    {
        "%{Dependencies.Glfw.LibDir}",
        "%{Dependencies.YamlCPP.LibDir}",
    }

    includedirs 
    {
        "%{wks.location}/LkEngine/Source",
        "%{wks.location}/LkEngine/Source/LkEngine",

        "%{ExternalDirectory}",

        "%{Dependencies.Glfw.IncludeDir}",
        "%{Dependencies.Glad.IncludeDir}",
        "%{Dependencies.ImGui.IncludeDir}",
        "%{Dependencies.ImGuizmo.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Entt.IncludeDir}",
        "%{Dependencies.StbImage.IncludeDir}",
        "%{Dependencies.Glm.IncludeDir}",
        "%{Dependencies.Spdlog.IncludeDir}",
        "%{Dependencies.Box2D.IncludeDir}",
        "%{Dependencies.YamlCPP.IncludeDir}",
        "%{Dependencies.Choc.IncludeDir}",
        "%{Dependencies.ImGuiNodeEditor.IncludeDir}",
    }

    links {
        "GLFW",
        "ImGui",
        "ImGuizmo",
        "box2d",
        "imgui-node-editor",
        "yaml-cpp",
        "glad", 
        "opengl32",
    }

	filter "system:windows"
		defines 
        { 
            "LK_PLATFORM_WINDOWS",
            "_IMGUI_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
            "_GLM_WIN32",
            --"VK_PROTOTYPES",
            --"VK_USE_PLATFORM_WIN32_KHR",
		}
        includedirs 
        { 
            --"%{Dependencies.Vulkan.Windows.IncludeDir}" 
        }
        libdirs 
        { 
            --"%{Dependencies.Vulkan.Windows.LibDir}",
            "%{Dependencies.Assimp.Windows.LibDir}",
        }
        links {
            --"%{Dependencies.Vulkan.Windows.LibName}",
            "%{Dependencies.Assimp.Windows.LibName}",
        }
        postbuildcommands {
            "{COPY} %{Dependencies.Assimp.Windows.LibDir}/%{Dependencies.Assimp.Windows.LibName}.dll %{cfg.targetdir}"
        }

	filter "configurations:Debug"
		defines "LK_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "LK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "LK_DIST"
		runtime "Release"
		optimize "On"

