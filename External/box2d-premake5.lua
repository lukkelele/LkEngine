project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	location "box2d"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines {
		"BOX2D_UNIT_TESTS=OFF",
		"BOX2D_SAMPLES=OFF",
	}

	files {
		"%{prj.location}/include/box2d/**.h",
		"%{prj.location}/src/**.c",
		"%{prj.location}/src/**.h",
	}

	includedirs {
		"%{prj.location}/include",
		"%{prj.location}/include/box2d",
		"%{prj.location}/src",
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
			"%{prj.location}/src/box2d.natvis",
		}

		filter "configurations:Debug or configurations:Debug-AddressSanitize or configurations:AutomationTest"
			runtime "Debug"
			symbols "On"

	filter "configurations:Debug"
        defines { "BOX2D_DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
