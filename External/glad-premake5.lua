project "glad"
    kind "StaticLib"
    language "C"
    location "glad"
    staticruntime "On"
    
	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files
    {
        "%{prj.location}/include/glad/glad.h",
        "%{prj.location}/src/glad.c",

        "%{prj.location}/include/KHR/khrplatform.h",
    }

    includedirs 
    { 
        "%{prj.location}/include" 
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"