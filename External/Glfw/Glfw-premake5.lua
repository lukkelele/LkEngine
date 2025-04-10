project "Glfw"
	kind "StaticLib"
	language "C"
	staticruntime "On"

	targetdir (TargetDirectory)
	objdir (IntermediateDirectory)

	includedirs {
		"glfw/include"
	}

	files {
		"glfw/include/GLFW/glfw3.h",
		"glfw/include/GLFW/glfw3native.h",
		"glfw/src/glfw_config.h",
		"glfw/src/mappings.h",
		"glfw/src/context.c",
		"glfw/src/init.c",
		"glfw/src/input.c",
		"glfw/src/monitor.c",
		"glfw/src/vulkan.c",
		"glfw/src/window.c",
		"glfw/src/platform.h",
		"glfw/src/platform.c",
		"glfw/src/osmesa_context.c",
		"glfw/src/null_init.c",
		"glfw/src/null_platform.h",
		"glfw/src/null_window.c",
		"glfw/src/null_monitor.c",
		"glfw/src/null_joystick.h",
		"glfw/src/null_joystick.c",
	}

    prebuildmessage "[%{prj.name}] Building"

	filter "system:windows"
		systemversion "latest"

		files {
			"glfw/src/win32_init.c",
			"glfw/src/win32_module.c",
			"glfw/src/win32_joystick.c",
			"glfw/src/win32_monitor.c",
			"glfw/src/win32_time.h",
			"glfw/src/win32_time.c",
			"glfw/src/win32_thread.h",
			"glfw/src/win32_thread.c",
			"glfw/src/win32_window.c",
			"glfw/src/wgl_context.c",
			"glfw/src/egl_context.c",
		}

		defines { 
			"_CRT_SECURE_NO_WARNINGS",
			"_GLFW_WIN32",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"

		files {
			"glfw/src/x11_platform.h",
			"glfw/src/xkb_unicode.h",
			"glfw/src/x11_init.c",
			"glfw/src/x11_monitor.c",
			"glfw/src/x11_window.c",
			"glfw/src/xkb_unicode.c",

			"glfw/src/posix_poll.h",
			"glfw/src/posix_poll.c",
			"glfw/src/posix_time.h",
			"glfw/src/posix_time.c",
			"glfw/src/posix_thread.h",
			"glfw/src/posix_thread.c",
			"glfw/src/posix_module.c",
			"glfw/src/glx_context.c",
			"glfw/src/egl_context.c",

			"glfw/src/linux_joystick.h",
			"glfw/src/linux_joystick.c",
		}

		defines {
			"_GLFW_X11"
		}

    filter "configurations:Debug or configurations:AutomationTest"
		runtime "Debug"
		symbols "On"

    filter "configurations:Debug-AS"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
