
target("TPProcess")
    set_kind("binary")
    set_group("Client")
    add_includedirs(
        ".")
    add_headerfiles("*.h")
    add_files(
        "*.cpp",
        "process.rc")
    add_deps("UiProcess")
    add_packages("tiltedcore", "cef", "hopscotch-map")