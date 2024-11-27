project "LkRuntime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files {
		"%{prj.location}/Source/**.h",
		"%{prj.location}/Source/**.cpp",
	}

	includedirs {
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
        "%{Dependencies.YamlCpp.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Tracy.IncludeDir}",
	}

	links { "LkEngine" }

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AddressSanitize"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
