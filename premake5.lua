workspace "Vulkan Learning"
	architecture "x64"

	configurations
	{
		"Debug"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "Vulkan Learning/vendor/glfw/include/GLFW/"
IncludeDir["vulkan"] = "Vulkan Learning/vendor/vulkan/Include"

include "Vulkan Learning/vendor/glfw/premake5.lua"

project "Vulkan Learning"
	staticruntime "On"
	location "Vulkan Learning"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/VkInit",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.vulkan}"
	}

	links 
	{ 
		"GLFW",
		"vulkan-1"
	}

	filter "system:windows"
		libdirs { 
			"Vulkan Learning/vendor/vulkan/Lib"
		}
		cppdialect "C++17"
		systemversion "latest"
		symbols "On"