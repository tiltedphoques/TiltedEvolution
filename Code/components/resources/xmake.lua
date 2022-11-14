component("Resourcecs")
	set_pcxxheader("Pch.h")
	add_headerfiles("Pch.h", {prefixdir = "ESLoader"})
	add_files("stdafx.cpp")
	add_packages("zlib", "glm")

