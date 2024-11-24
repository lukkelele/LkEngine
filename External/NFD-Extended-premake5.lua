project "NFD-Extended"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    location "NFD-Extended"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files 
    { 
        "%{prj.name}/src/include/nfd.h", 
        "%{prj.name}/src/include/nfd.hpp" 
    }

    includedirs { "%{prj.name}/src/include/" }

    filter "system:windows"
		systemversion "latest"

        files 
        { 
            "%{prj.name}/src/nfd_win.cpp" 
        }

    filter "system:linux"
		pic "On"
		systemversion "latest"

        files 
        { 
            "%{prj.name}/src/nfd_gtk.cpp" 
        }

        result, err = os.outputof("pkg-config --cflags gtk+-3.0")
        buildoptions { result }

    filter "system:macosx"
		pic "On"

        files 
        { 
            "%{prj.name}/src/nfd_cocoa.m" 
        }

    filter "configurations:Debug or configurations:Debug-AS"
        runtime "Debug"
        symbols "On"

    filter { "system:windows", "configurations:Debug-AS" }	
		sanitize { "Address" }
		flags 
        { 
            "NoRuntimeChecks", 
            "NoIncrementalLink" 
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "Full"