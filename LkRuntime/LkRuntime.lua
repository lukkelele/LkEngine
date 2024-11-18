project "LkRuntime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	configurations 
	{ 
		"Debug", 
		"Release", 
		"Dist" 
	}

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines 
	{
		"_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING",
		"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
		"SPDLOG_USE_STD_FORMAT",
        "GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
        "IMGUI_DEFINE_MATH_OPERATORS",
		"NOMINMAX",
		"TRACY_ENABLE",
		"TRACY_ON_DEMAND",
		"TRACY_CALLSTACK=10",
	}

	files 
	{
		"%{prj.location}/Source/**.h",
		"%{prj.location}/Source/**.cpp",
	}

	includedirs 
	{
		"%{wks.location}/LkEngine",
		"%{wks.location}/LkEngine/Source",

		"%{ExternalDirectory}",

        "%{Dependencies.Glfw.IncludeDir}",
        "%{Dependencies.Glad.IncludeDir}",
        "%{Dependencies.StbImage.IncludeDir}",
        "%{Dependencies.Spdlog.IncludeDir}",
        "%{Dependencies.ImGui.IncludeDir}",
        "%{Dependencies.ImGuizmo.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Entt.IncludeDir}",
        "%{Dependencies.Glm.IncludeDir}",
        "%{Dependencies.Box2D.IncludeDir}",
        "%{Dependencies.YamlCPP.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Tracy.IncludeDir}",
	}

	links { "LkEngine" }

	filter "system:windows"
		systemversion "latest"

		defines 
        { 
            "LK_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "_GLM_WIN32",
            "_IMGUI_WIN32",
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
            "/IGNORE:4006", -- Ignore 'already defined' warning for object files.
            "/IGNORE:4099", -- Ignore warning 'PDB not found, linking as if no debug info'.
			"/NODEFAULTLIB:MSVCRT",
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
