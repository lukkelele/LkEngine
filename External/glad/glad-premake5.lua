project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    
	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files
    {
        "include/glad/glad.h",
        "src/glad.c",

        "include/KHR/khrplatform.h",
    }

    includedirs { "include" }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"