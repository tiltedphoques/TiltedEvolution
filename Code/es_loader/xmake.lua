
add_requires("zlib")

target("ESLoader")
	set_kind("binary")
	set_group("common")
    set_pcxxheader("stdafx.h")
    add_headerfiles("stdafx.h")
    add_files("stdafx.cpp")
	add_includedirs(".", "../", {public = true})
	add_headerfiles("**.h", {prefixdir = "ESLoader"})
	add_files("**.cpp")
	add_packages(
		"tiltedcore",
        "spdlog",
        "hopscotch-map",
        "zlib")
