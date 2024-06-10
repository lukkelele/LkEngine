project "ImGuizmo"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

   defines { "IMGUI_DEFINE_MATH_OPERATORS" }

	files
   {
      "ImGuizmo.h",
      "ImGuizmo.cpp",
      "ImGradient.h",
      "ImGradient.cpp",
      "ImCurveEdit.h",
      "ImCurveEdit.cpp",
      "ImSequencer.h",
      "ImSequencer.cpp",
      "ImZoomSlider.cp",
      "GraphEditor.h",
      "GraphEditor.cpp",
	}

   includedirs { "../imgui" }

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

	filter "configurations:Debug"
		staticruntime "On"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
