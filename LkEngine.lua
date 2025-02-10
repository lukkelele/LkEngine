-- Version --
LK_ENGINE_MAJOR = 0
LK_ENGINE_MINOR = 2
LK_ENGINE_PATCH = 1

function EncodeVersion(Major, Minor, Patch)
    return (Major << 24) | (Minor << 16) | Patch
end

LK_ENGINE_VERSION = EncodeVersion(LK_ENGINE_MAJOR, LK_ENGINE_MINOR, LK_ENGINE_PATCH)

workspace "LkEngine"
    architecture "x86_64"
    startproject "LkRuntime"

    configurations { 
        "Debug", 
        "Debug-AddressSanitize", 
        "Release", 
        "Dist",
        "AutomationTest",
    }

    flags { 
        "MultiProcessorCompile" 
    }

    defines {
        "LK_ENGINE_MAJOR=" .. LK_ENGINE_MAJOR,
        "LK_ENGINE_MINOR=" .. LK_ENGINE_MINOR,
        "LK_ENGINE_PATCH=" .. LK_ENGINE_PATCH,
        "LK_ENGINE_VERSION=" .. LK_ENGINE_VERSION,

        "LK_ENGINE_STATIC_LIB",
        "_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING",
		"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
		"SPDLOG_USE_STD_FORMAT",
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS",
		"NOMINMAX",
		"TRACY_ENABLE",
		"TRACY_ON_DEMAND",
		"TRACY_CALLSTACK=10",
        "GLM_ENABLE_EXPERIMENTAL",
    }

    filter "action:vs*"
		linkoptions { 
            "/ignore:4006", -- Ignore: 'Already defined' warning for object files.
            "/ignore:4099", -- Ignore: 'PDB not found, linking as if no debug info'.
			"/NODEFAULTLIB:MSVCRT",
        }

        disablewarnings { 
            "4068",  -- 'Unknown #pragma mark'
            "4267",  -- 'Size conversion, possible loss of data'
            "4244",  -- 'Size conversion, possible loss of data'
            "28020", -- Warning for infinite loops: 'The expression is not true at this call'
            "33011", -- 'Unchecked upper bound'
        } 

    filter "system:windows"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
        }

		buildoptions { 
            "/Zc:preprocessor",    -- Enable new standard-conforming preprocessor for MSVC.
            "/Zc:__cplusplus",
            "/EHsc",               -- Explictly only allow exceptions at throw statements or in a function.
        }

    filter "configurations:Debug"
        defines { "LK_ENGINE_DEBUG" }
        runtime "Debug"
        symbols "On"

	filter "configurations:Release"
        runtime "Release"
		optimize "On"
		symbols "Default"
		defines { "LK_ENGINE_RELEASE" }

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		defines { "LK_ENGINE_DIST" }

    filter "configurations:AutomationTest"
        runtime "Debug"
        symbols "On"
        defines { 
            "LK_ENGINE_DEBUG", 
            "LK_ENGINE_AUTOMATION_TEST",
        }


BuildOutputDirectory = "%{cfg.buildcfg}-%{cfg.system}"
TargetDirectory = "%{wks.location}/Binaries/" .. BuildOutputDirectory .. "/%{prj.name}"
IntermediateDirectory = "%{wks.location}/Intermediate/" .. BuildOutputDirectory .. "/%{prj.name}"

ExternalDirectory = "%{wks.location}/External"
LibrariesDirectory = "%{ExternalDirectory}/Libraries"
AssetsDirectory = "%{wks.location}/LkRuntime/Assets"

include "External/Dependencies.lua"

include "LkRuntime/LkRuntime.lua"
--include "LkEditor/LkEditor.lua"

-- Test
include "LkEngine/Test/LkEngineTest.lua"
-- ~Test

--|---------------------------------------------
--| LkEngine
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

    files { 
        "%{wks.location}/LkEngine/Source/**.h",
        "%{wks.location}/LkEngine/Source/**.cpp",
        "%{wks.location}/LkEngine/Source/**.tpp",

        "%{Dependency.StbImage.IncludeDir}/**.h",
        "%{Dependency.StbImage.IncludeDir}/**.cpp",
        "%{Dependency.Spdlog.IncludeDir}/spdlog/spdlog.h",
   	}

    defines {
        "LK_CHAR_UTF8", --"LK_CHAR_UNICODE",
        "LK_ENGINE_OPENGL",
        "LK_OPENGL4",   -- OpenGL 4

        "LK_ENGINE_EDITOR=1",

        "LK_PHYSICS_API_BULLET3",
        "LK_PHYSICS_API_BOX2D",

        "GLFW_INCLUDE_NONE",
        "IMGUI_DEFINE_MATH_OPERATORS",
    }

    libdirs {
        "%{ExternalDirectory}/Libraries",
    }

    includedirs {
        "%{wks.location}/LkEngine/Source",
        "%{wks.location}/LkEngine/Source/LkEngine",

        "%{Dependency.Glfw.IncludeDir}",
        "%{Dependency.Glad.IncludeDir}",
        "%{Dependency.StbImage.IncludeDir}",
        "%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- Allow 'imgui.h' as well as 'imgui/imgui.h' as an include.
        "%{Dependency.ImGuizmo.IncludeDir}",
        "%{Dependency.ImGuiNodeEditor.IncludeDir}",
        "%{Dependency.Assimp.IncludeDir}",
        "%{Dependency.Entt.IncludeDir}",
        "%{Dependency.Glm.IncludeDir}",
        "%{Dependency.Spdlog.IncludeDir}",
        "%{Dependency.Box2D.IncludeDir}",
        "%{Dependency.Bullet3.IncludeDir}",
        "%{Dependency.YamlCpp.IncludeDir}",
        "%{Dependency.Tracy.IncludeDir}",
        "%{Dependency.NfdExtended.IncludeDir}",
    }

    links {
        "Glfw", 
        "Glad", 
        "ImGui",
        "ImGuizmo",
        "ImGuiNodeEditor",
        "NfdExtended",
        "YamlCpp",
        "Box2D",
        "Bullet3",
    }

	filter "system:windows"
		defines { 
            "LK_PLATFORM_WINDOWS",
            "_GLM_WIN32",
            "_IMGUI_WIN32",
		}

        links {
            "opengl32",
        }

        filter "configurations:Debug or configurations:AutomationTest"
			buildoptions {
				"/wd4312", -- Disable: C4312 (type conversion from greater size)
				"/wd4244", -- Disable: C4244 (conversion, possible loss of data)
			}

			links {
				"%{Dependency.Assimp.Windows.DebugLibName}",
				"zlibstaticd", -- Built by Assimp.
				"DbgHelp"
			}

		filter "configurations:Release or configurations:Dist"
			links {
				"%{Dependency.Assimp.Windows.LibName}",
				"zlibstatic", -- Built by Assimp.
			}
        
	filter "configurations:Debug or configurations:Debug-AddressSanitize or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"
        links { "Tracy" }

	filter "configurations:AutomationTest"
        includedirs {
            "%{wks.location}/LkEngine/Test",
        }

    filter "kind:StaticLib"
        defines { "LK_ENGINE_STATIC_LIB" }

    filter "kind:SharedLib"
        defines { "LK_ENGINE_CORE" }
