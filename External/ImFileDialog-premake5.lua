project "ImFileDialog"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	location "%{prj.name}"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files
	{
		"%{prj.location}/ImFileDialog.h",
		"%{prj.location}/ImFileDialog.cpp",
	}

    includedirs
    {
        "%{ExternalDirectory}",
        "%{ExternalDirectory}/imgui",
        "%{ExternalDirectory}/stb_image",
    }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		pic "On"

	filter "configurations:Debug"
		defines "LK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LK_DIST"
		runtime "Release"
		optimize "on"
