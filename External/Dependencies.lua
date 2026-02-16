--|------------------------------------------------------------
--| [LkEngine] 
--| 
--| Engine Dependencies
--|------------------------------------------------------------

VULKAN_SDK = os.getenv("VULKAN_SDK")

Dependency = {

	Glfw = {
        IncludeDir = "%{wks.location}/External/Glfw/glfw/include",
		LibDir = "%{wks.location}/External/Libraries",
		Windows = { 
			LibName = "glfw3.lib",  -- FIXME: .lib or .so does not need to be specified.
		},
		Linux = { 
			LibName = "glfw3.so", 
		},
	},

	Glad = {
        IncludeDir = "%{wks.location}/External/Glad/include",
	},

	Spdlog = {
		IncludeDir = "%{wks.location}/External/Spdlog/spdlog/include",
	},

	StbImage = {
		IncludeDir = "%{wks.location}/External/StbImage",
	},

	ImGui = { 
		IncludeDir = "%{wks.location}/External/ImGui",
	},

	ImGuiNodeEditor = {
		IncludeDir = "%{wks.location}/External/ImGuiNodeEditor",
	},

	ImGuizmo = {
		IncludeDir = "%{wks.location}/External/ImGuizmo",
	},

	Box2D = {
		IncludeDir = "%{wks.location}/External/Box2D/box2d/include",
		Windows = {
			LibName = "box2d.lib",
			DebugLibName = "box2dd.lib",
		},
		Linux = {
			LibName = "box2d.so",
			DebugLibName = "box2dd.so",
		},
		LibDir = "%{wks.location}/External/Box2D/box2d/build/lib",
		Configurations = "Debug,Release"
	},

	Bullet3 = {
		IncludeDir = "%{wks.location}/External/Bullet3/bullet3/include",
		LibDir = "%{wks.location}/External/Bullet3/bullet3/lib",
	},

	YamlCpp = {
		IncludeDir = "%{wks.location}/External/YamlCpp/yaml-cpp/include",
		LibDir = "%{wks.location}/External/YamlCpp/yaml-cpp/lib",
	},
	
	Glm = {
		IncludeDir = "%{wks.location}/External/Glm/glm"
	},

	Assimp = {
        IncludeDir = "%{wks.location}/External/Assimp/assimp/include",
		Windows = { 
			LibName = "assimp",
			DebugLibName = "assimp",
			LibDir = "%{wks.location}/External/Libraries" 
		},
		Linux = { 
			LibName = "assimp",
			DebugLibName = "assimp",
			LibDir = "%{wks.location}/External/Libraries" 
		},
		Configurations = "Debug,Release"
	},

	Entt = {
		IncludeDir = "%{wks.location}/External/Entt/entt/single_include" 
	},

	Tracy = {
		IncludeDir = "%{wks.location}/External/Tracy/tracy/public",
		LibDir = "%{wks.location}/External/Libraries" 
	},

	NfdExtended = {
		IncludeDir = "%{wks.location}/External/NfdExtended/nativefiledialog-extended/src/include"
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
include "External/Glfw/Glfw-premake5.lua"
include "External/Glad/Glad-premake5.lua"
include "External/ImGui/ImGui-premake5.lua"
include "External/ImGuizmo/ImGuizmo-premake5.lua"
include "External/ImGuiNodeEditor/ImGuiNodeEditor-premake5.lua"
include "External/YamlCpp/YamlCpp-premake5.lua"
include "External/Box2D/Box2D-premake5.lua"
include "External/Bullet3/Bullet3-premake5.lua"
include "External/NfdExtended/NfdExtended-premake5.lua"
include "External/Tracy/Tracy-premake5.lua"
-- ~Configurations
