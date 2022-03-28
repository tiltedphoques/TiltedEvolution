
target("TPProcess")
    set_kind("binary")
    set_group("Client")
    add_includedirs(
        ".",
        "../",
        "../../build/")
    add_headerfiles("*.h")
    add_files(
        "*.cpp",
        "resources/process.rc")
    add_deps("UiProcess")
    add_packages("tiltedcore", "cef", "hopscotch-map")