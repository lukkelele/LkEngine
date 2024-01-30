project "LkApplication"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	configurations { "Debug", "Release", "Dist" }

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",

        "%{wks.location}/external/VulkanMemoryAllocator/vk_mem_alloc.h",
        "%{wks.location}/external/VulkanMemoryAllocator/vk_mem_alloc.cpp",
	}

	includedirs {
		"%{wks.location}/LkEngine",
		"%{wks.location}/LkEngine/src",
		"%{wks.location}/external",

        "%{Dependencies.Glfw.IncludeDir}",
        "%{Dependencies.Glad.IncludeDir}",
        "%{Dependencies.StbImage.IncludeDir}",
        "%{Dependencies.Spdlog.IncludeDir}",
        "%{Dependencies.ImGui.IncludeDir}",
        "%{Dependencies.ImGuizmo.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",
        "%{Dependencies.Entt.IncludeDir}",
        "%{Dependencies.Glm.IncludeDir}",
        "%{Dependencies.Box2D.IncludeDir}",
        "%{Dependencies.YamlCPP.IncludeDir}",
        "%{Dependencies.Assimp.IncludeDir}",

        "%{wks.location}/external/VulkanMemoryAllocator",
	}

	links { "LkEngine" }

	filter "system:windows"
		systemversion "latest"
        includedirs { 
            "%{Dependencies.Vulkan.Windows.IncludeDir}",
        }
		libdirs {
            "%{Dependencies.Vulkan.Windows.LibDir}",
		}
		links {
            "%{Dependencies.Vulkan.Windows.LibName}",
		}
        postbuildcommands {
            "{COPY} %{Dependencies.Assimp.Windows.LibDir}/%{Dependencies.Assimp.Windows.LibName}.dll %{cfg.targetdir}"
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
