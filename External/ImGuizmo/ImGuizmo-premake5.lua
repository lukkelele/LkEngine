project "ImGuizmo" 
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	location "ImGuizmo"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory) 

	defines { 
		"IMGUI_DEFINE_MATH_OPERATORS" 
	}
	
	files {
		"%{prj.location}/ImGuizmo.h", 
		"%{prj.location}/ImGuizmo.cpp",
		"%{prj.location}/ImGradient.h",
		"%{prj.location}/ImGradient.cpp",
		"%{prj.location}/ImCurveEdit.h",
		"%{prj.location}/ImCurveEdit.cpp",
		"%{prj.location}/ImSequencer.h",
		"%{prj.location}/ImSequencer.cpp",
		"%{prj.location}/ImZoomSlider.cp",
		"%{prj.location}/GraphEditor.h",
		"%{prj.location}/GraphEditor.cpp", 
	} 

	includedirs { 
        "%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- Allow 'imgui.h' as well as 'imgui/imgui.h' as an include.
	}

	buildoptions {
        "/wd4005", -- Disable: 'Macro redefinition'.
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

    filter "configurations:Debug or configurations:Debug-AddressSanitize"
		staticruntime "On"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:AutomationTest"
		runtime "Debug"
		symbols "On"
