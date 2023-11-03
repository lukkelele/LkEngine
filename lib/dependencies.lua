

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
			LibDir = "%{VULKAN_SDK}/lib/",
		},
	},

	ImGui = {
        IncludeDir = "%{wks.location}/lib/imgui",
	},

	Box2D = {
		LibName = "box2d",
		IncludeDir = "%{wks.location}/lib/box2d/include",
	},

}