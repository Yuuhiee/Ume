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
	kind "SharedLib"
	language "C++"
	staticruntime "off"

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

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"UME_PLATFORM_WINDOWS",
			"UME_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox\"")
		}

	filter "configurations:Debug"
		defines "UME_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "UME_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "UME_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Ume"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"UME_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "UME_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "UME_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "UME_DIST"
		runtime "Release"
		optimize "On"