project "yaml-cpp"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	--staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files { 
        "src/**.h", 
        "src/**.cpp", 
		"include/**.h"
	}

	includedirs { 
		"include",
		"include/yaml-cpp"
	}
	
	defines {
		"YAML_CPP_STATIC_DEFINE",
		"YAMLCPP_USE_STATIC_LIBS"
	}

	filter "system:windows"
		systemversion "latest"
		--staticruntime "On"
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "On"
