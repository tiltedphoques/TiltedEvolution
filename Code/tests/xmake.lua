
target("TPTests")
    set_kind("binary")
    set_group("Tests")
    add_defines("TP_SKYRIM=1")
    add_includedirs(
        ".", "../encoding")
    add_headerfiles("**.h")
    add_files("*.cpp")
    add_deps("Encoding")
    add_packages(
        "tiltedcore",
        "hopscotch-map",
        "catch2",
        "mimalloc")
