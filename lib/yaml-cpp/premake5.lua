project "yaml-cpp"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{ 
        "src/**.h", 
        "src/**.cpp", 
		"include/**.h"
	}

	includedirs 
	{ 
		"include",
		"include/yaml-cpp"
	}
	
	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
	
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
