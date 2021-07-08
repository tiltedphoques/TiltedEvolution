
target("AdminProtocol")
    set_kind("static")
    set_group("common")
    add_includedirs(".", "../", {public = true})
    add_headerfiles("**.h", {prefixdir = "AdminProtocol"})
    add_files("**.cpp")
    add_packages("tiltedcore", "hopscotch-map")
