project "yaml-cpp"
   kind "StaticLib"
   language "C++"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   { 
       "src/**.h", 
       "src/**.cpp", 
       "include/**.h"
   }

   includedirs 
   { 
       "include"
   }


filter "configurations:Debug"
	defines "LK_DEBUG"
	runtime "Debug"
	symbols "on"

filter "configurations:Release"
	defines "LK_RELEASE"
	runtime "Release"
	optimize "on"

filter "configurations:Dist"
	defines "LK_DIST"
	runtime "Release"
	optimize "on"
