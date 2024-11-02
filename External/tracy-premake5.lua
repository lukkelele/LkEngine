project "Tracy"
	kind "StaticLib"
	language "C++"
    location "tracy"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	files
	{
		"%{prj.location}/public/client/**.h",
		"%{prj.location}/public/client/**.hpp",
		"%{prj.location}/public/client/**.cpp",

		"%{prj.location}/public/common/**.h",
		"%{prj.location}/public/common/**.hpp",
		"%{prj.location}/public/common/**.cpp",

		"%{prj.location}/public/tracy/**.h",
		"%{prj.location}/public/tracy/**.hpp",
		"%{prj.location}/public/tracy/**.cpp",

		"%{prj.location}/public/libbacktrace/alloc.cpp",
		"%{prj.location}/public/libbacktrace/sort.cpp",
		"%{prj.location}/public/libbacktrace/state.cpp",
	}

	includedirs 
	{ 
		"%{prj.location}/public/" 
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		files 
		{
			"%{prj.location}/public/libbacktrace/posix.cpp",
			"%{prj.location}/public/libbacktrace/mmapio.cpp",
			"%{prj.location}/public/libbacktrace/macho.cpp",
			"%{prj.location}/public/libbacktrace/fileline.cpp",
			"%{prj.location}/public/libbacktrace/elf.cpp",
			"%{prj.location}/public/libbacktrace/dwarf.cpp",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		conformancemode "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		conformancemode "On"

		defines 
		{
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND"
		}

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		conformancemode "On"

		defines 
		{
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND"
		}