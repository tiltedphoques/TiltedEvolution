
add_requires("zlib")

target("ESLoader")
	set_kind("binary")
	set_group("Components")
    add_configfiles("BuildInfo.h.in")
    set_pcxxheader("stdafx.h")
	add_includedirs(
		".",
		"../",
		"../../", 
		"../../../build", 
		{public = true})
	add_headerfiles("stdafx.h", "**.h", {prefixdir = "ESLoader"})
	add_files("**.cpp", "stdafx.cpp")
	add_packages(
		"tiltedcore",
        "spdlog",
        "hopscotch-map",
        "zlib",
        "gtest")


target("ESLoader-Test")
	set_kind("binary")
	set_group("Components")
	add_configfiles("BuildInfo.h.in")
    set_pcxxheader("stdafx.h")
	add_includedirs(
		".",
		"../",
		"../../", 
		"../../../build", 
		{public = true})
	add_headerfiles(
		"**.h")
	add_files(
		"**.cpp",
		"../../../build/TestMain.cpp")
	add_packages(
		"tiltedcore", 
		"hopscotch-map", 
		"gtest",
		"spdlog",
		"zlib")