--- LkEngine Version ---
LK_ENGINE_MAJOR = 0
LK_ENGINE_MINOR = 3
LK_ENGINE_PATCH = 1

function EncodeVersion(Major, Minor, Patch)
	return (Major << 24) | (Minor << 16) | Patch
end

LK_ENGINE_VERSION = EncodeVersion(LK_ENGINE_MAJOR, LK_ENGINE_MINOR, LK_ENGINE_PATCH)

workspace "LkEngine"
	architecture "x86_64"
	startproject "LkEditor"

	configurations { 
		"Debug", 
		"Debug-AS", -- Address Sanitize
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
			"/ignore:4006", -- 'Already defined' warning for object files.
			"/ignore:4099", -- 'PDB not found, linking as if no debug info'.
			"/NODEFAULTLIB:MSVCRT",
		}

		disablewarnings { 
			"4068",  -- 'Unknown #pragma mark'
			"4267",  -- 'Size conversion, possible loss of data'
			"4244",  -- 'Size conversion, possible loss of data'
			"4312",  -- Conversion of greater size
			"28020", -- Warning for infinite loops: 'The expression is not true at this call'
			"33011", -- 'Unchecked upper bound'
		} 

	filter "system:windows"
		defines {
			"LK_PLATFORM_WINDOWS",
			"LK_ENGINE_MSVC",
			"SPDLOG_USE_STD_FORMAT",
			"_CRT_SECURE_NO_WARNINGS",
			"_GLM_WIN32",
			"_IMGUI_WIN32",
		}

		buildoptions { 
			"/Zc:preprocessor", -- New standard-conforming preprocessor for MSVC.
			"/Zc:__cplusplus",
			"/EHsc",			-- Explictly only allow exceptions at throw statements or in a function.
		}

	filter "system:linux"
		toolset ("gcc")
		toolsversion "14.2"
		defines { 
			"LK_PLATFORM_LINUX" 
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

	filter "toolset:msvc*"
		defines { "LK_ENGINE_MSVC" }
	filter "toolset:gcc*"
		defines { "LK_ENGINE_GCC" }
	filter "toolset:clang*"
		defines { "LK_ENGINE_CLANG" }
	filter {}


BuildOutputDirectory = "%{cfg.buildcfg}-%{cfg.system}"
TargetDirectory = "%{wks.location}/Binaries/" .. BuildOutputDirectory .. "/%{prj.name}"
IntermediateDirectory = "%{wks.location}/Intermediate/" .. BuildOutputDirectory .. "/%{prj.name}"

ExternalDirectory = "%{wks.location}/External"
LibrariesDirectory = "%{ExternalDirectory}/Libraries"
AssetsDirectory = "%{wks.location}/LkRuntime/Assets"

include "External/Dependencies.lua"

-- LTesto Framework
include "LkEngine/Test/LTesto/LTesto.lua"

-- LkEditor
include "LkEditor/LkEditor.lua"

--|---------------------------------------------
--| LkEngine
--|
--| Engine Project
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
		"%{wks.location}/LkEngine/Source/LkEngine/**.h",
		"%{wks.location}/LkEngine/Source/LkEngine/**.cpp",
		"%{wks.location}/LkEngine/Source/LkEngine/**.tpp",

		"%{Dependency.StbImage.IncludeDir}/stb/stb_image.h",
		"%{Dependency.StbImage.IncludeDir}/stb/stb_image_resize2.h",
		"%{Dependency.StbImage.IncludeDir}/stb/stb_image.cpp",

		"%{Dependency.Spdlog.IncludeDir}/spdlog/spdlog.h",
	}

	defines {
		"LK_CHAR_UTF8", --"LK_CHAR_UNICODE",
		"LK_ENGINE_OPENGL",
		"LK_OPENGL4", -- OpenGL 4

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

	prebuildmessage "[%{prj.name}] C Compiler: $(C)"
	prebuildmessage "[%{prj.name}] C++ Compiler: $(CXX)"

	filter "system:windows"
		files {
			"LkEngine/Source/LKpch.cpp",
		}

		links {
			"opengl32",
		}

		buildoptions {
			"/wd4312", -- Disable: C4312 (type conversion from greater size)
			"/wd4244", -- Disable: C4244 (conversion, possible loss of data)
		}

		filter { "system:windows", "configurations:Debug or configurations:AutomationTest" }
			links {
				"%{Dependency.Assimp.Windows.LibDir}/%{Dependency.Assimp.Windows.DebugLibName}",
				"%{Dependency.Assimp.Windows.LibDir}/zlibstaticd",
				"DbgHelp"
			}

		filter { "system:windows", "configurations:Release or configurations:Dist" }
			links {
				"%{Dependency.Assimp.Windows.LibDir}/%{Dependency.Assimp.Windows.LibName}",
				"%{Dependency.Assimp.Windows.LibDir}/zlibstatic",
			}

	filter "system:linux"
		defines {
			"STBI_NO_SIMD",
			"STBIR_NO_SIMD",
		}

		links {
			"gtk-3",
			"gdk-3",
			"glib-2.0",
			"gobject-2.0",
			"X11",
			"GL",
			"dl",
		}

		buildoptions {
			"-Wno-attributes",
			"-Wno-delete-incomplete",
			"-Wno-int-to-pointer-cast",
			"-Wno-format-security",
		}

		removefiles {
			"%{wks.location}/LkEngine/Platform/Windows/**.h",
			"%{wks.location}/LkEngine/Platform/Windows/**.cpp",
		}

		filter { "system:linux", "configurations:Debug or configurations:AutomationTest" }
			links {
				"%{Dependency.Assimp.Linux.LibDir}/%{Dependency.Assimp.Linux.DebugLibName}",
				"%{Dependency.Assimp.Linux.LibDir}/libzlibstatic",
			}

		filter { "system:linux", "configurations:Release or configurations:Dist" }
			links {
				"%{Dependency.Assimp.Linux.LibDir}/%{Dependency.Assimp.Linux.LibName}",
				"%{Dependency.Assimp.Linux.LibDir}/libzlibstatic",
			}

	filter { "configurations:Debug or configurations:Debug-AS or configurations:AutomationTest" }
		runtime "Debug"
		symbols "On"
		links { "Tracy" }

	filter { "configurations:*", "configurations:not AutomationTest" }
		defines { "LK_ENGINE_EDITOR" }

	filter "configurations:AutomationTest"
		includedirs {
			"%{wks.location}/LkEngine/Test",
			"%{wks.location}/LkEngine/LTesto",
		}

	filter { "configurations:Debug", "action:vs2022" }
		symbols "Full"

	filter "kind:StaticLib"
		defines { "LK_ENGINE_STATIC_LIB" }

	filter "kind:SharedLib"
		defines { "LK_ENGINE_SHARED_LIB" }
