add_requires("zlib")

component("ESLoader")
	set_pcxxheader("stdafx.h")
	add_headerfiles("stdafx.h", {prefixdir = "ESLoader"})
	add_files("**.cpp", "stdafx.cpp")
	add_packages("zlib")

unittest("ESLoader")
	set_pcxxheader("stdafx.h")
	add_headerfiles("stdafx.h", {prefixdir = "ESLoader"})
	add_files("**.cpp", "stdafx.cpp")
	add_packages("zlib")