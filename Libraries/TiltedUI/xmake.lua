
add_requires(
    "tiltedcore",
    "catch2",
    "directxtk",
    "cef",
    "hopscotch-map")

target("TiltedUi")
    set_kind("static")
    set_group("Libraries")
    add_files("ui/src/*.cpp")
    add_includedirs("ui/include/", {public = true})
    add_headerfiles("ui/include/*.hpp", {prefixdir = "TiltedUi"})
    add_syslinks("dxguid", "d3d11")
    add_packages(
        "tiltedcore",
        "cef",
        "directxtk",
        "mimalloc",
        "hopscotch-map")

target("UiProcess")
    set_kind("static")
    set_group("Libraries")
    add_files("ui_process/src/*.cpp")
    add_headerfiles("ui_process/include/*.hpp", {prefixdir = "UiProcess"})
    add_includedirs("ui_process/include/", {public = true})
    add_packages(
        "tiltedcore",
        "cef",
        "mimalloc",
        "hopscotch-map")

--target("TiltedUi_Tests")
--    set_kind("binary")
--    set_group("Tests")
--    add_files("Code/tests/src/*.cpp")
--    add_deps("TiltedUi")
--    add_packages("tiltedcore", "catch2", "hopscotch-map")
