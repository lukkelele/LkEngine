project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    
	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files {
        "include/glad/glad.h",
        "src/glad.c",

        "include/KHR/khrplatform.h",
    }

    includedirs { 
        "include",
        "glad/include" 
    }
    
    filter "system:windows"
        systemversion "latest"
        links {
            "opengl32",
        }

    filter "system:linux"
        links {
            "GL",
            "dl"
        }

    filter "configurations:Debug or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AddressSanitize"
		runtime "Debug"
		symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"