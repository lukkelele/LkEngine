--|------------------------------------------------------------
--| [LkEngine] 
--| 
--| Dependencies
--|------------------------------------------------------------

VULKAN_SDK = os.getenv("VULKAN_SDK")

Dependencies = {

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

	Glad = {
        IncludeDir = "%{wks.location}/External/glad/include",
	},

	Spdlog = {
		IncludeDir = "%{wks.location}/External/spdlog/include",
	},

	StbImage = {
		IncludeDir = "%{wks.location}/External/stb_image",
	},

	ImGui = {
        IncludeDir = "%{wks.location}/External/imgui",
	},

	ImGuizmo = {
		IncludeDir = "%{wks.location}/External/ImGuizmo/",
	},

	ImGuiNodeEditor = {
		IncludeDir = "%{wks.location}/External/imgui-node-editor",
	},

	Box2D = {
		IncludeDir = "%{wks.location}/External/box2d/include",
		Windows = {
			LibName = "box2d.lib",
			DebugLibName = "box2dd.lib",
		},
		Linux = {
			LibName = "box2d.so",
			DebugLibName = "box2dd.so",
		},
		LibDir = "%{wks.location}/External/box2d/build/lib",
		Configurations = "Debug,Release"
	},

	YamlCpp = {
		IncludeDir = "%{wks.location}/External/yaml-cpp/include",
		LibDir = "%{wks.location}/External/yaml-cpp/lib",
	},
	
	Glm = {
        IncludeDir = "%{wks.location}/External/glm"
	},

	Assimp = {
        IncludeDir = "%{wks.location}/External/assimp/include",
		Windows = { 
			LibName = "assimp-vc143-mt.lib",
			DebugLibName = "assimp-vc143-mtd.lib", 
			LibDir = "%{wks.location}/External/Libraries" 
		},
		Linux = { 
			LibName = "assimp-vc143-mt.so",
			DebugLibName = "assimp-vc143-mtd.so", 
			LibDir = "%{wks.location}/External/assimp/bin/linux/" 
		},
		Configurations = "Debug,Release"
	},

	Entt = {
		IncludeDir = "%{wks.location}/External/entt/include" 
	},

	Tracy = {
		IncludeDir = "%{wks.location}/External/tracy/public",
		LibDir = "%{wks.location}/External/Libraries" 
	},

	NfdExtended = {
		IncludeDir = "%{wks.location}/External/NFD-Extended/src/include"
	},

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

	VulkanMemoryAllocator = {
		IncludeDir = "%{wks.location}/External/VulkanMemoryAllocator",
	},

}

-- Configurations
include "External/Glfw-premake5.lua"
include "External/Glad-premake5.lua"
include "External/ImGui-premake5.lua"
include "External/ImGuizmo-premake5.lua"
include "External/ImGuiNodeEditor-premake5.lua"
include "External/YamlCpp-premake5.lua"
include "External/Box2D-premake5.lua"
include "External/Tracy-premake5.lua"
include "External/NfdExtended-premake5.lua"
-- ~Configurations

include "LkRuntime/LkRuntime.lua"

-- Test
include "LkEngine/Test/LkEngineTest.lua"
-- ~Test
