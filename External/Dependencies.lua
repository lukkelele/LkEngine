--|------------------------------------------------------------
--| [LkEngine] 
--| 
--| Dependencies
--|------------------------------------------------------------

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
			LibDir = "%{VULKAN_SDK}/External/",
		},
	},

	Glfw = {
        IncludeDir = "%{wks.location}/External/glfw/include",
		LibDir = "%{wks.location}/External/Libraries",
		Windows = { 
			LibName = "glfw3.lib", 
		},
		Linux = { 
			LibName = "glfw3.so", 
		},
	},

	ImGui = {
        IncludeDir = "%{wks.location}/External/imgui",
	},

	Glad = {
        IncludeDir = "%{wks.location}/External/glad/include",
	},

	Box2D = {
		IncludeDir = "%{wks.location}/External/box2d/include",
		LibName = "box2dd",
		LibDir = "%{wks.location}/External/box2d/build/src/%{cfg.buildcfg}",
	},

	Spdlog = {
		IncludeDir = "%{wks.location}/External/spdlog/include",
	},

	StbImage = {
		IncludeDir = "%{wks.location}/External/",
	},

	ImGuizmo = {
		IncludeDir = "%{wks.location}/External/ImGuizmo/",
	},

	ImGuiNodeEditor = {
		IncludeDir = "%{wks.location}/External/imgui-node-editor",
	},

	ImFileDialog = {
		IncludeDir = "%{wks.location}/External/ImFileDialog",
	},

	VulkanMemoryAllocator = {
		IncludeDir = "%{wks.location}/External/VulkanMemoryAllocator",
	},

	YamlCPP = {
		IncludeDir = "%{wks.location}/External/yaml-cpp/include",
		LibDir = "%{wks.location}/External/yaml-cpp/lib",
	},
	
	Glm = {
        IncludeDir = "%{wks.location}/External/glm"
	},

	Assimp = {
        IncludeDir = "%{wks.location}/External/assimp/include",
		Windows = { 
			LibName = "assimp-vc143-mt",
			DebugLibName = "assimp-vc143-mtd", 
			LibDir = "%{wks.location}/External/Libraries" 
		},
		Linux = { 
			LibName = "assimp-vc143-mt",
			DebugLibName = "assimp-vc143-mtd", 
			LibDir = "%{wks.location}/External/assimp/bin/linux/" 
		},
		Configurations = "Debug,Release"
	},

	Entt = {
        	IncludeDir = "%{wks.location}/External/entt/include" 
	},

	Tracy = {
		LibName = "Tracy",
		IncludeDir = "%{wks.location}/External/tracy/public",
		LibDir = "%{wks.location}/External/Libraries" 
	}

}

-- Libraries
include "External/glfw-premake5.lua"
include "External/glad-premake5.lua"
include "External/imgui-premake5.lua"
include "External/ImGuizmo-premake5.lua"
include "External/imgui-node-editor-premake5.lua"
include "External/ImFileDialog-premake5.lua"
include "External/yaml-cpp-premake5.lua"
include "External/box2d-premake5.lua"
include "External/tracy-premake5.lua"
-- ~Libraries

include "LkRuntime/LkRuntime.lua"

