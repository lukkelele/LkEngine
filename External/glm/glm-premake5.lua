includedirs
{ 
	"glm",
	"glm/glm"
}

files
{
	"glm/glm/**.hpp"
}

filter "system:linux"
	pic "On"

	systemversion "latest"
	staticruntime "On"

	defines
	{
		"_GLM_X11"
	}

filter "system:windows"
	systemversion "latest"
	staticruntime "On"

	defines 
	{ 
		"_GLM_WIN32",
		"_CRT_SECURE_NO_WARNINGS"
	}