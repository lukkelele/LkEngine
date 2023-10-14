project "imgui_test_engine"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{ 
		"*.h", 
		"*.cpp",
		"thirdparty/**.h",
		"thirdparty/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/lib/imgui"
	}

	defines
	{
		"IMGUI_DEFINE_MATH_OPERATORS",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
