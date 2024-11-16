workspace "LkEngine"
    architecture "x86_64"
    startproject "LkApplication"

    configurations 
    { 
        "Debug", 
        "Debug-AddressSanitize", 
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


BuildOutputDirectory = "%{cfg.buildcfg}-%{cfg.system}"
TargetDirectory = "%{wks.location}/Binaries/" .. BuildOutputDirectory .. "/%{prj.name}"
IntermediateDirectory = "%{wks.location}/Intermediate/" .. BuildOutputDirectory .. "/%{prj.name}"

ExternalDirectory = "%{wks.location}/External"
LibrariesDirectory = "%{ExternalDirectory}/Libraries"
AssetsDirectory = "%{wks.location}/Assets"

include "External/Dependencies.lua"

--|---------------------------------------------
--| LkEngine
--|
--| Engine Project.
--|---------------------------------------------
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
        "LK_OPENGL4", -- OpenGL 4
        "LK_CHAR_UTF8",
        --"LK_CHAR_UNICODE",

        "_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING",
		"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
		"SPDLOG_USE_STD_FORMAT",
        "GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
        "GLM_ENABLE_EXPERIMENTAL",
        "IMGUI_DEFINE_MATH_OPERATORS",
		"NOMINMAX",
		"TRACY_ENABLE",
		"TRACY_ON_DEMAND",
		"TRACY_CALLSTACK=10",

        "LK_PHYSICS_API_BULLET3",
        "LK_PHYSICS_API_BOX2D",
    }

    files
    { 
        "%{wks.location}/LkEngine/**.h",
        "%{wks.location}/LkEngine/**.cpp",

        "%{wks.location}/LkEngine/Source/**.h",
        "%{wks.location}/LkEngine/Source/**.cpp",
        "%{wks.location}/LkEngine/Source/**.tpp",

		"%{ExternalDirectory}/stb_image/**.h",
		"%{ExternalDirectory}/stb_image/**.cpp",

        "%{ExternalDirectory}/spdlog/include/spdlog/spdlog.h",
   	}

    libdirs 
    {
        "%{ExternalDirectory}/Libraries",
        "%{Dependencies.Glfw.LibDir}",
        "%{Dependencies.YamlCPP.LibDir}",
        "%{Dependencies.Box2D.LibDir}",
    }

    includedirs 
    {
        "%{wks.location}/LkEngine/Source",
        "%{wks.location}/LkEngine/Source/LkEngine",

        "%{ExternalDirectory}",
        "%{ExternalDirectory}/spdlog/include",

        "%{Dependencies.Glfw.IncludeDir}",
        "%{Dependencies.Glad.IncludeDir}",
        "%{Dependencies.ImGui.IncludeDir}",
        "%{Dependencies.ImGuizmo.IncludeDir}",
        "%{Dependencies.ImGuiNodeEditor.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Entt.IncludeDir}",
        "%{Dependencies.StbImage.IncludeDir}",
        "%{Dependencies.Glm.IncludeDir}",
        "%{Dependencies.Spdlog.IncludeDir}",
        "%{Dependencies.Box2D.IncludeDir}",
        "%{Dependencies.YamlCPP.IncludeDir}",
        "%{Dependencies.Tracy.IncludeDir}",
    }

    links 
    {
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
            "_CRT_SECURE_NO_WARNINGS",
            "_GLM_WIN32",
            "_IMGUI_WIN32",
		}

        libdirs 
        { 
            "%{Dependencies.Assimp.Windows.LibDir}",
        }

        postbuildcommands 
        {
        }
 
		buildoptions 
        { 
            "/Zc:preprocessor",    -- Enable new standard-conforming preprocessor for MSVC.
            "/Zc:__cplusplus",
            "/EHsc",               -- Only allow exceptions at throw statements or in a function.
            "/wd4312",             -- Disable warning: C4312 (type conversion from greater size)
            "/wd4244",             -- Disable warning: C4244 (conversion, possible loss of data)
        }

        linkoptions 
        { 
            "/IGNORE:4006", -- Ignore warning 'already defined' for object files.
            "/IGNORE:4099", -- Ignore warning 'PDB not found, linking as if no debug info'.
        }

	filter { "system:windows", "configurations:Debug" }	
        links 
        {
            "%{Dependencies.Assimp.Windows.DebugLibName}.lib",
            "zlibstaticd", -- Built by Assimp.
            "DbgHelp"
        }

	filter { "system:windows", "configurations:Release or configurations:Dist" }	
        links 
        {
            "%{Dependencies.Assimp.Windows.LibName}.lib",
            "zlibstatic", -- Built by Assimp.
        }
        
	filter { "system:windows", "configurations:Debug-AddressSanitize" }	
		runtime "Debug"
		sanitize { "Address" }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }
		defines "LK_DEBUG"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines 
        {
            "LK_DEBUG",
        }

        links
        {
            "Tracy",
        }

	filter "configurations:Release"
        runtime "Release"
		optimize "On"
		symbols "Default"
		defines "LK_RELEASE"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		defines "LK_DIST"

