project "ImGuizmo" 
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
		"ImGuizmo/ImGuizmo.h", 
		"ImGuizmo/ImGuizmo.cpp",
		"ImGuizmo/ImGradient.h",
		"ImGuizmo/ImGradient.cpp",
		"ImGuizmo/ImCurveEdit.h",
		"ImGuizmo/ImCurveEdit.cpp",
		"ImGuizmo/ImSequencer.h",
		"ImGuizmo/ImSequencer.cpp",
		"ImGuizmo/ImZoomSlider.cp",
		"ImGuizmo/GraphEditor.h",
		"ImGuizmo/GraphEditor.cpp", 
	} 

	includedirs { 
        "%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- Allow 'imgui.h' as well as 'imgui/imgui.h' as an include.
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		buildoptions {
			"/wd4005", -- 'Macro redefinition'.
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

		filter "system:clang"
			buildoptions {
				"-Wno-macro-redefined",
			}

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
