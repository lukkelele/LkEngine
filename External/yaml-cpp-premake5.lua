project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	location "yaml-cpp"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files 
	{ 
        "yaml-cpp/src/**.h", 
        "yaml-cpp/src/**.cpp", 
		"yaml-cpp/include/**.h"
	}

	includedirs 
	{ 
		"yaml-cpp/include",
		"yaml-cpp/include/yaml-cpp"
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
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
