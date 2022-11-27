component("PapyrusVM")
	set_pcxxheader("stdafx.h")
	add_headerfiles("stdafx.h", {prefixdir = "PapyrusVM"})
	add_files("stdafx.cpp")

unittest("PapyrusVM")
	set_pcxxheader("stdafx.h")
	add_headerfiles("stdafx.h", {prefixdir = "PapyrusVM"})
	add_files("stdafx.cpp")
