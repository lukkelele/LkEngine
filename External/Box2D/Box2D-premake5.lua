project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines {
		"BOX2D_UNIT_TESTS=OFF",
		"BOX2D_SAMPLES=OFF",
	}

	files {
		"box2d/include/box2d/**.h",
		"box2d/src/**.c",
		"box2d/src/**.h",
	}

	includedirs {
		"box2d/include",
		"box2d/include/box2d",
		"box2d/src",
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { 
			"/std:c11",
            "/experimental:c11atomics",
		} 
		defines { "BOX2D_MSVC" }
        warnings "Extra"

		files {
			"box2d/src/box2d.natvis",
		}

		filter "configurations:Debug or configurations:Debug-AddressSanitize or configurations:AutomationTest"
			runtime "Debug"
			symbols "On"

	filter "configurations:Debug"
        defines { "BOX2D_DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
