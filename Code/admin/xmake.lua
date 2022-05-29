

target("Admin")
    set_kind("binary")
    set_group("Client")
    set_basename("TiltedAdmin")
    set_symbols("debug", "hidden")
    add_includedirs(
        ".",
        "../",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files(
        "**.cpp",
        "admin.rc")
    add_deps("CommonLib", "AdminProtocol", "TiltedConnect")

    add_deps("SkyrimEncoding")

    if is_plat("windows") then
        add_syslinks("opengl32", "Shell32", "Gdi32", "Winmm", "Ole32", "version", "OleAut32", "Setupapi")
    end

    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map",
        "glm",
        "magnum",
        "magnum-integration",
        "gamenetworkingsockets")
