-- premake5.lua
workspace "state-machine"
	configurations { "debug", "release" }
  kind "ConsoleApp"
  language "C++"
  toolset "gcc"
  targetdir "bin/%{cfg.buildcfg}"

  includedirs {
    "src/",
  }

  rtti "Off"

  filter "configurations:debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:release"
		defines { "NDEBUG" }
		optimize "On"

project "light-state-machine"
	files {
		"examples/Light.cpp",
	}
