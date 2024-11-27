project "YamlCpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	location "yaml-cpp"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files { 
        "%{prj.location}/src/**.h", 
        "%{prj.location}/src/**.cpp", 
		"%{prj.location}/include/**.h"
	}

	includedirs { 
		"%{prj.location}/include",
		"%{prj.location}/include/yaml-cpp"
	}

	defines {
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS"
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
