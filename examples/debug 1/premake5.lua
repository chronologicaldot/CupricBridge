--[[ Debugging project file ]]

local v_cubr_path = "../../src/"
local v_copper_path = "../../../CopperLang/Copper/src/"
local v_copper_stdlib_path = "../../../CopperLang/Copper/stdlib/"
local v_irrext_path = "../../../../IrrExtensions/"
local v_irrlicht_home = "/usr/local"
local v_irrlicht_include = "/usr/local/include/irrlicht/"

-- "make" paths
local v_b_cubr_path = "../" .. v_cubr_path
local v_b_copper_path = "../" .. v_copper_path
local v_b_copper_stdlib_path = "../" .. v_copper_stdlib_path
local v_b_irrext_path = "../" .. v_irrext_path

workspace "Debug CuBridge"
	configurations { "debug" }
	location "build"
	objdir "build/obj"
	targetdir "bin"
	optimize "Off"
	--warnings "Extra"
	filter { "action:gmake" }
		buildoptions " -g"

project "Debug CuBridge"
	targetname "debug.out"
	language "C++"
	flags { "C++11" }
	kind "ConsoleApp"
	links {
		"Irrlicht",
		"GL",
		"Xxf86vm",
		"Xext",
		"X11",
		"Xcursor"
	}
	defines( "SYSTEM=Linux" )
	files {
		"debug.cpp"
		, v_cubr_path .. "**.h"
		, v_cubr_path .. "**.cpp"
		--, v_irrext_path .. "**.h"
		--, v_irrext_path .. "**.cpp"
		, v_copper_path .. "**.h"
		, v_copper_path .. "**.cpp"
		, v_copper_stdlib_path .. "**.h"
		, v_copper_stdlib_path .. "**.cpp"
	}
	removefiles {
		v_cubr_path .. "excludes/**.h"
		, v_cubr_path .. "excludes/**.cpp"
	}
	buildoptions {
		"-I" .. v_b_cubr_path
		, "-I" .. v_b_copper_path
		, "-I" .. v_b_copper_stdlib_path
		, "-I" .. v_irrlicht_include
		, "-I" .. v_b_irrext_path
	}
	linkoptions {
		" -L" .. v_irrlicht_home .. "/lib"
	}
