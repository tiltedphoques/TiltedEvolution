target("Stafield")
    set_kind("static")
    set_group("Libraries")
    set_pcxxheader("GenesisPCH.h")
    set_symbols("debug", "hidden")
    add_includedirs(
        ".",
        "../",
        "../../vendor/")
    add_headerfiles("**.h")
    add_files("**.cpp")
    add_linkdirs(".", {public = true})

    add_packages(
        "tiltedcore",
        "spdlog",
        "minhook",
        "hopscotch-map")