project "LkEngine_AutomationTests"
	language "C++"
	cppdialect "C++20"
    kind "ConsoleApp"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines {
        "LK_AUTOMATION_TEST_ALL",
        "LK_AUTOMATION_TEST_CORE",

		"YAML_CPP_STATIC_DEFINE",
		"NOMINMAX",
        "FMT_UNICODE",
	}

	files {
		"%{prj.location}/AutomationTest/**.h",
		"%{prj.location}/AutomationTest/**.cpp",
	}

	includedirs {
		"%{prj.location}",

		"%{wks.location}/LkEngine/Source", -- LkEngine source
		"%{ExternalDirectory}",

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

		filter "configurations:AutomationTest"
			kind "ConsoleApp"
			symbols "On"

		filter "configurations:Debug or configurations:AutomationTest"
		    runtime "Debug"

		filter { "configurations:Release or configurations:Dist" }	
		    runtime "Release"
			optimize "On"

