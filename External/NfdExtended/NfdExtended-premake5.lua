project "NfdExtended"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

    files { 
        "nativefiledialog-extended/src/include/nfd.h", 
        "nativefiledialog-extended/src/include/nfd.hpp" 
    }

    includedirs { "nativefiledialog-extended/src/include/" }

    filter "system:windows"
		systemversion "latest"

        files { 
            "nativefiledialog-extended/src/nfd_win.cpp" 
        }

		filter "configurations:Debug-AdressSanitize"
			sanitize { "Address" }
			flags { 
				"NoRuntimeChecks", 
				"NoIncrementalLink" 
			}

    filter "system:linux"
		pic "On"
		systemversion "latest"

        files 
        { 
            "nativefiledialog-extended/src/nfd_gtk.cpp" 
        }

        result, err = os.outputof("pkg-config --cflags gtk+-3.0")
        buildoptions { result }

		filter "configurations:Debug-AdressSanitize"
			sanitize { "Address" }
			flags { 
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