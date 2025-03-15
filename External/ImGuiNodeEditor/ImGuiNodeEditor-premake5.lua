project "ImGuiNodeEditor"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    defines { 
		"IMGUI_DEFINE_MATH_OPERATORS" 
	}

	files {
		"imgui-node-editor/*.h",
		"imgui-node-editor/*.cpp",
	}

    includedirs {
		"imgui-node-editor",

        "%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- Allow 'imgui.h' as well as 'imgui/imgui.h' as an include.
    }

    prebuildmessage "[%{prj.name}] Starting build"

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		pic "On"

    filter "configurations:Debug or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AS"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
