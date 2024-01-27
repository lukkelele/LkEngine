--|===========================================================
--| LkEngine 
--|
--| Dependencies
--|===========================================================

VULKAN_SDK = os.getenv("VULKAN_SDK")

Dependencies = {
	Vulkan = {
		Windows = {
			LibName = "vulkan-1",
			IncludeDir = "%{VULKAN_SDK}/Include/",
			LibDir = "%{VULKAN_SDK}/Lib/",
		},
		Linux =  {
			LibName = "vulkan",
			IncludeDir = "%{VULKAN_SDK}/include/",
			LibDir = "%{VULKAN_SDK}/external/",
		},
	},

	Glfw = {
        IncludeDir = "%{wks.location}/external/glfw/include",
		LibDir = "%{wks.location}/external/glfw/lib",
	},

	ImGui = {
        IncludeDir = "%{wks.location}/external/imgui",
	},

	Glad = {
        IncludeDir = "%{wks.location}/external/glad/include",
	},

	Box2D = {
		LibName = "box2d",
		IncludeDir = "%{wks.location}/external/box2d/include",
	},

	Spdlog = {
		IncludeDir = "%{wks.location}/external/spdlog/",
	},

	ImGuizmo = {
		IncludeDir = "%{wks.location}/external/ImGuizmo/",
	},

	VulkanMemoryAllocator = {
		IncludeDir = "%{wks.location}/external/VulkanMemoryAllocator",
		Files = {
		}
	},

	YamlCPP = {
        IncludeDir = "%{wks.location}/external/yaml-cpp/include",
	},
	
	Glm = {
        IncludeDir = "%{wks.location}/external/glm"
	},

}

--include "external/glfw/premake5.lua"
--include "external/glad/premake5.lua"
--include "external/imgui/premake5.lua"
--include "external/ImGuizmo/premake5.lua"
--include "external/imgui-node-editor/premake5.lua"
--include "external/box2d/premake5.lua"
--include "external/yaml-cpp/premake5.lua"
--
--include "LkApplication/premake5.lua"
