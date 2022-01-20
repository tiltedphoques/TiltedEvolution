add_requires("zlib")

component("ESLoader")
	set_kind("binary")
    set_pcxxheader("stdafx.h")
	add_headerfiles("stdafx.h", {prefixdir = "ESLoader"})
	add_files("**.cpp", "stdafx.cpp")
	add_packages("zlib")


unittest("ESLoader-Test")