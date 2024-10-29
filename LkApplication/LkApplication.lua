project "LkApplication"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	configurations { "Debug", "Release", "Dist" }

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files 
	{
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs 
	{
		"%{wks.location}/LkEngine",
		"%{wks.location}/LkEngine/Source",

		--"%{ExternalDirectory}",
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
	}

	links { "LkEngine" }

	filter "system:windows"
		systemversion "latest"

		defines 
        { 
            "LK_PLATFORM_WINDOWS",
            "_IMGUI_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
            "_GLM_WIN32",
		}

        postbuildcommands 
		{
            "{COPY} %{Dependencies.Assimp.Windows.LibDir}/%{Dependencies.Assimp.Windows.LibName}.dll %{cfg.targetdir}"
        }

		buildoptions 
        { 
            "/Zc:preprocessor",  -- Enable new standard-conforming preprocessor for MSVC.
            "/wd4312",           -- Disable warning: C4312 (type conversion from greater size)
            "/wd4244",           -- Disable warning: C4244 (conversion, possible loss of data)
        }

		linkoptions 
        { 
            "/IGNORE:4006", -- Ignore 'already defined' warning for object files.
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
