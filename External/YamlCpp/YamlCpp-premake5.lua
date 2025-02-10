project "YamlCpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files { 
        "yaml-cpp/src/**.h", 
        "yaml-cpp/src/**.cpp", 
		"yaml-cpp/include/**.h"
	}

	includedirs { 
		"yaml-cpp/include",
		"yaml-cpp/include/yaml-cpp"
	}

	defines {
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS"
	}

    --flags { "ExcludeFromBuild" } 

    prebuildcommands {
        "{ECHO} Building: %{prj.name}",
    }
	
	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
	
    filter "configurations:Debug or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AddressSanitize"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
