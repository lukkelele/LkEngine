project "YamlCpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines {
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS"
	}

	files { 
        "yaml-cpp/src/**.h", 
        "yaml-cpp/src/**.cpp", 
		"yaml-cpp/include/**.h"
	}

	includedirs { 
		"yaml-cpp/include",
		"yaml-cpp/include/yaml-cpp"
	}

    prebuildmessage "[%{prj.name}] Starting build"
	
	filter "system:windows"
		staticruntime "On"

	filter "system:linux"
		staticruntime "On"
	
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
