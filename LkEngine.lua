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
        "_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING",
		"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
		"SPDLOG_USE_STD_FORMAT",
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS",
		"NOMINMAX",
		"TRACY_ENABLE",
		"TRACY_ON_DEMAND",
		"TRACY_CALLSTACK=10",
    }

    filter "action:vs*"
		linkoptions { 
            "/ignore:4006", -- Ignore: 'Already defined' warning for object files.
            "/ignore:4099", -- Ignore: 'PDB not found, linking as if no debug info'.
			"/NODEFAULTLIB:MSVCRT",
        }

        disablewarnings { 
            "4068" -- Disable: 'Unknown #pragma mark'
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


BuildOutputDirectory = "%{cfg.buildcfg}-%{cfg.system}"
TargetDirectory = "%{wks.location}/Binaries/" .. BuildOutputDirectory .. "/%{prj.name}"
IntermediateDirectory = "%{wks.location}/Intermediate/" .. BuildOutputDirectory .. "/%{prj.name}"

ExternalDirectory = "%{wks.location}/External"
LibrariesDirectory = "%{ExternalDirectory}/Libraries"
AssetsDirectory = "%{wks.location}/LkRuntime/Assets"

include "External/Dependencies.lua"

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

        "%{Dependencies.StbImage.IncludeDir}/*.h",
        "%{Dependencies.StbImage.IncludeDir}/*.cpp",
        "%{Dependencies.Spdlog.IncludeDir}/spdlog/spdlog.h",
   	}

    defines {
        "LK_CHAR_UTF8", --"LK_CHAR_UNICODE",
        "LK_ENGINE_OPENGL",
        "LK_OPENGL4",   -- OpenGL 4
        "LK_PHYSICS_API_BULLET3",
        "LK_PHYSICS_API_BOX2D",

        "GLFW_INCLUDE_NONE",
        "GLM_ENABLE_EXPERIMENTAL",
        "IMGUI_DEFINE_MATH_OPERATORS",
    }

    libdirs {
        "%{ExternalDirectory}/Libraries",
    }

    includedirs {
        "%{wks.location}/LkEngine/Source",
        "%{wks.location}/LkEngine/Source/LkEngine",

        "%{ExternalDirectory}",
        "%{Dependencies.Glfw.IncludeDir}",
        "%{Dependencies.Glad.IncludeDir}",
        "%{Dependencies.StbImage.IncludeDir}",
        "%{Dependencies.ImGui.IncludeDir}",
        "%{Dependencies.ImGuizmo.IncludeDir}",
        "%{Dependencies.ImGuiNodeEditor.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Entt.IncludeDir}",
        "%{Dependencies.Glm.IncludeDir}",
        "%{Dependencies.Spdlog.IncludeDir}",
        "%{Dependencies.Box2D.IncludeDir}",
        "%{Dependencies.YamlCpp.IncludeDir}",
        "%{Dependencies.Tracy.IncludeDir}",
        "%{Dependencies.NfdExtended.IncludeDir}",
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
				"%{Dependencies.Assimp.Windows.DebugLibName}",
				"zlibstaticd", -- Built by Assimp.
				"DbgHelp"
			}

		filter "configurations:Release or configurations:Dist"
			links {
				"%{Dependencies.Assimp.Windows.LibName}",
				"zlibstatic", -- Built by Assimp.
			}
        
	filter "configurations:Debug or configurations:Debug-AddressSanitize or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"
        links { "Tracy" }

    filter "kind:StaticLib"
        defines { "LK_ENGINE_STATIC_LIB" }

    filter "kind:SharedLib"
        defines { "LK_ENGINE_CORE" }
