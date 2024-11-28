project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    
	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files {
        "glad/include/glad/glad.h",
        "glad/src/glad.c",

        "glad/include/KHR/khrplatform.h",
    }

    includedirs { 
        "glad/include" 
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AddressSanitize"
		runtime "Debug"
		symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"