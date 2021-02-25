
target("Launcher")
    set_kind("binary")
    set_group("Client")
    set_basename("TiltedOnline")
    -- disable edit and continue
    add_cxxflags("/Zi")
    set_strip("all")
    add_ldflags(
        "/IGNORE:4254",
        "/DYNAMICBASE:NO",
       -- "/STACK:\"2097152\"",
        "/SAFESEH:NO",
        "/LARGEADDRESSAWARE",
        "/INCREMENTAL:NO",
        "/MANIFEST:NO",
        "/LAST:.zdata",
        "/ENTRY:mainCRTStartup", { force = true })
    add_includedirs(
        ".",
        "../",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files(
        "**.cpp",
        "**.c",
        "launcher.rc")
    add_deps(
        "TiltedReverse",
        "TiltedHooks",
        "TiltedUi")
    add_syslinks(
        "user32", 
        "shell32",
        "comdlg32",
        "bcrypt",
        "ole32",
        "dxgi",
        "d3d11",
        "gdi32")
    add_packages(
        "tiltedcore",
        "spdlog",
        "minhook",
        "hopscotch-map",
        "glm",
        "cef")