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
			LibDir = "%{VULKAN_SDK}/External/",
		},
	},

	Glfw = {
        IncludeDir = "%{wks.location}/External/glfw/include",
		LibDir = "%{wks.location}/External/glfw/lib",
	},

	ImGui = {
        IncludeDir = "%{wks.location}/External/imgui",
	},

	Glad = {
        IncludeDir = "%{wks.location}/External/glad/include",
	},

	Box2D = {
		LibName = "box2d",
		IncludeDir = "%{wks.location}/External/box2d/include",
	},

	Spdlog = {
		IncludeDir = "%{wks.location}/External/spdlog/",
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
			LibDir = "%{wks.location}/External/assimp/bin/windows/%{cfg.buildcfg}/" 
		},
		Linux = { 
			LibName = "assimp", 
			LibDir = "%{wks.location}/External/assimp/bin/linux/" 
		},
		Configurations = "Debug,Release"
	},

	Entt = {
        	IncludeDir = "%{wks.location}/External/entt/include"
	},

	Choc = {
        	IncludeDir = "%{wks.location}/External/choc"
	},

}


include "External/glfw/glfw-premake5.lua"
include "External/glad/glad-premake5.lua"
include "External/imgui/imgui-premake5.lua"
include "External/ImGuizmo/ImGuizmo-premake5.lua"
include "External/imgui-node-editor/imgui-node-editor-premake5.lua"
include "External/box2d/box2d-premake5.lua"
include "External/yaml-cpp/yaml-cpp-premake5.lua"

include "LkApplication/LkApplication.lua"

