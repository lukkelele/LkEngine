project "box2d"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files 
	{
		"src/**.h",
		"src/**.cpp",
		"include/**.h"
	}

	includedirs 
	{
		"include",
		"src"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        	symbols "Off"
