project "box2d"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "On"
	location "%{prj.name}"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files 
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp",
		"%{prj.location}/include/**.h"
	}

	includedirs 
	{
		"%{prj.location}/include",
		"%{prj.location}/src"
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
