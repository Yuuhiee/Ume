workspace "Ume"
	architecture "x64"

	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories releateive to root folder (solution directory)
IncludeDir = {}
IncludeDir["spdlog"] = "Ume/vendor/spdlog/include"
IncludeDir["GLFW"] = "Ume/vendor/GLFW/include"
IncludeDir["Glad"] = "Ume/vendor/Glad/include"
IncludeDir["ImGui"] = "Ume/vendor/imgui"
IncludeDir["glm"] = "Ume/vendor/glm"

include "Ume/vendor/GLFW"
include "Ume/vendor/Glad"
include "Ume/vendor/imgui"

project "Ume"
	location "Ume"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "umepch.h"
	pchsource "Ume/src/umepch.cpp"	

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"UME_PLATFORM_WINDOWS",
			"UME_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "UME_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "UME_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "UME_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Ume/src",
		"Ume/vendor",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Ume"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"UME_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "UME_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "UME_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "UME_DIST"
		runtime "Release"
		optimize "on"