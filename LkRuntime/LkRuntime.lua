project "LkRuntime"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines {
        "LK_ENGINE_RUNTIME",
    }

	files {
		"%{prj.location}/Source/**.h",
		"%{prj.location}/Source/**.cpp",
	}

	includedirs {
		"%{wks.location}/LkEngine",
		"%{wks.location}/LkEngine/Source",

        "%{Dependency.Glfw.IncludeDir}",
        "%{Dependency.Glad.IncludeDir}",
        "%{Dependency.StbImage.IncludeDir}",
        "%{Dependency.Spdlog.IncludeDir}",
        "%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- Allow 'imgui.h' as well as 'imgui/imgui.h' as an include.
        "%{Dependency.ImGuizmo.IncludeDir}",
        "%{Dependency.ImGuiNodeEditor.IncludeDir}",
        "%{Dependency.Assimp.IncludeDir}",
        "%{Dependency.Entt.IncludeDir}",
        "%{Dependency.Glm.IncludeDir}",
        "%{Dependency.Box2D.IncludeDir}",
        "%{Dependency.YamlCpp.IncludeDir}",
        "%{Dependency.Assimp.IncludeDir}",
        "%{Dependency.Tracy.IncludeDir}",
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
