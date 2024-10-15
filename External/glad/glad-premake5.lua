project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    location "glad"
    
	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files
    {
        "%{prj.name}/include/glad/glad.h",
        "%{prj.name}/src/glad.c",

        "%{prj.name}/include/KHR/khrplatform.h",
    }

    includedirs 
    { 
        "include" 
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"