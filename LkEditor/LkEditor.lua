project "LkEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	defines {
		"LK_ENGINE_OPENGL",
		"IMGUI_DEFINE_MATH_OPERATORS",
	}

	files {
		"%{prj.location}/Source/LkEditor.cpp",
		"%{prj.location}/Source/**.h",
		"%{prj.location}/Source/**.cpp",
	}

	includedirs {
		"%{prj.location}/Source",
		"%{prj.location}/Source",

		"%{wks.location}/LkEngine",
		"%{wks.location}/LkEngine/Source",

		"%{Dependency.Glfw.IncludeDir}",
		"%{Dependency.Glad.IncludeDir}",
		"%{Dependency.StbImage.IncludeDir}",
		"%{Dependency.Spdlog.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}",
		"%{Dependency.ImGui.IncludeDir}/imgui", -- Allow 'imgui.h' as well as 'imgui/imgui.h' as an include.
		"%{Dependency.ImGuizmo.IncludeDir}",
		"%{Dependency.ImGuiNodeEditor.IncludeDir}",
		"%{Dependency.Assimp.IncludeDir}",
		"%{Dependency.Entt.IncludeDir}",
		"%{Dependency.Glm.IncludeDir}",
		"%{Dependency.YamlCpp.IncludeDir}",
		"%{Dependency.Assimp.IncludeDir}",
		"%{Dependency.Tracy.IncludeDir}",
		"%{Dependency.NfdExtended.IncludeDir}",
		"%{Dependency.Box2D.IncludeDir}",
		"%{Dependency.Bullet3.IncludeDir}",
	}

	links { "LkEngine" }
	prelinkmessage "[%{prj.name}] Starting linkage"
	prebuildmessage "[%{prj.name}] Starting build"

	filter "system:windows"
		systemversion "latest"

		filter { "system:windows", "configurations:Debug or configurations:AutomationTest" }
			links {
				"%{Dependency.Assimp.Windows.LibDir}/%{Dependency.Assimp.Windows.DebugLibName}",
				"%{Dependency.Assimp.Windows.LibDir}/zlibstaticd",
			}

		filter { "system:windows", "configurations:Release or configurations:Dist" }
			links {
				"%{Dependency.Assimp.Windows.LibDir}/%{Dependency.Assimp.Windows.LibName}",
				"%{Dependency.Assimp.Windows.LibDir}/zlibstatic",
			}

	filter "system:linux"
		defines {
			"STBI_NO_SIMD",
			"STBIR_NO_SIMD",
		}

		buildoptions {
			"-Wno-attributes",
			"-Wno-delete-incomplete",
			"-Wno-int-to-pointer-cast",
			"-Wno-format-security",
		}

		links {
			"gtk-3",
			"gdk-3",
			"glib-2.0",
			"gobject-2.0",
			"X11",
			"GL",
			"dl",

			"Glfw",
			"Glad",
			"ImGui",
			"ImGuizmo",
			"ImGuiNodeEditor",
			"NfdExtended",
			"YamlCpp",
			"Box2D",
			"Bullet3",
			"Tracy",
		}

		filter { "system:linux", "configurations:Debug or configurations:AutomationTest" }
			links {
				"%{Dependency.Assimp.Linux.LibDir}/%{Dependency.Assimp.Linux.DebugLibName}",
				"%{Dependency.Assimp.Linux.LibDir}/zlibstatic",
			}

		filter { "system:linux", "configurations:Release or configurations:Dist" }
			links {
				"%{Dependency.Assimp.Linux.LibDir}/%{Dependency.Assimp.Linux.DebugLibName}",
				"%{Dependency.Assimp.Linux.LibDir}/zlibstatic",
			}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Debug-AddressSanitize"
		runtime "Debug"
		symbols "On"

	filter "configurations:AutomationTest"
		runtime "Debug"
		symbols "On"
		kind "None"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Speed"
