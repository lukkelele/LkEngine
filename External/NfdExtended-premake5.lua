project "NfdExtended"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    location "NFD-Extended"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files 
    { 
        "%{prj.location}/src/include/nfd.h", 
        "%{prj.location}/src/include/nfd.hpp" 
    }

    includedirs { "%{prj.location}/src/include/" }

    filter "system:windows"
		systemversion "latest"

        files 
        { 
            "%{prj.location}/src/nfd_win.cpp" 
        }

		filter "configurations:Debug-AdressSanitize"
			sanitize { "Address" }
			flags 
			{ 
				"NoRuntimeChecks", 
				"NoIncrementalLink" 
			}

    filter "system:linux"
		pic "On"
		systemversion "latest"

        files 
        { 
            "%{prj.location}/src/nfd_gtk.cpp" 
        }

        result, err = os.outputof("pkg-config --cflags gtk+-3.0")
        buildoptions { result }

		filter "configurations:Debug-AdressSanitize"
			sanitize { "Address" }
			flags 
			{ 
				"NoRuntimeChecks", 
				"NoIncrementalLink" 
			}

    filter "configurations:Debug or configurations:AutomationTest"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "Full"