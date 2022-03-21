
target("TPProcess")
    set_kind("binary")
    set_group("Client")

    if is_plat("windows") then
        add_ldflags("/subsystem:windows")
    end

    add_includedirs(
        ".")
    add_headerfiles("*.h")
    add_files(
        "*.cpp",
        "process.rc")
    add_deps("UiProcess")
    add_packages("tiltedcore", "cef", "hopscotch-map")