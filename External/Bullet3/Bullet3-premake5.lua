project "Bullet3"
	kind "StaticLib"
	language "C++"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines { 
        "B3_USE_CLEW",
        "BT_USE_SSE_IN_API" 
    }

	files {
		"bullet3/src/**.cpp",
		"bullet3/src/**.h",
		"bullet3/src/BulletCollision/**.cpp",
		"bullet3/src/BulletCollision/**.h",
		"bullet3/src/BulletDynamics/**.cpp",
		"bullet3/src/BulletDynamics/**.h",
		"bullet3/src/BulletInverseDynamics/**.cpp",
		"bullet3/src/BulletInverseDynamics/**.h",
		"bullet3/src/BulletSoftBody/**.cpp",
		"bullet3/src/BulletSoftBody/**.h",
		"bullet3/src/LinearMath/**.cpp",
		"bullet3/src/LinearMath/**.h",
	}

	includedirs
	{
		"bullet3/src"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"