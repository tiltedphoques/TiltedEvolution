
target("Common")
    set_kind("static")
    set_group("common")
    add_includedirs(".", "../", {public = true})
    add_headerfiles("**.h", {prefixdir = "Common"})
    add_files("**.cpp")
    add_packages("tiltedcore", "hopscotch-map")