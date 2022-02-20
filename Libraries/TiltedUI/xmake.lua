
add_requires(
    "tiltedcore",
    "catch2",
    "directxtk",
    "cef",
    "hopscotch-map")

target("TiltedUi")
    set_kind("static")
    set_group("Libraries")
    add_files("ui/*.cpp")
    add_includedirs("ui/", {public = true})
    add_headerfiles("ui/*.h", {prefixdir = "TiltedUi"})
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
    add_files("ui_process/*.cpp")
    add_headerfiles("ui_process/*.h", {prefixdir = "UiProcess"})
    add_includedirs("ui_process/", {public = true})
    add_packages(
        "tiltedcore",
        "cef",
        "mimalloc",
        "hopscotch-map")

target("UITestRunner")
    set_kind("binary")
    add_files("ui_test_runner/*.cpp")
    add_deps("TiltedUi")
    add_defines("RUNNER_PATH=LR\"($(scriptdir))\"")
    add_packages("tiltedcore", "catch2", "hopscotch-map", "cef", "directxtk")

--target("TiltedUi_Tests")
--    set_kind("binary")
--    set_group("Tests")
--    add_files("tests/src/*.cpp")
--    add_deps("TiltedUi")
--    add_packages("tiltedcore", "catch2", "hopscotch-map")
